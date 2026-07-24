#include "MyHeatDevice.h"

void MyHeatDevice::begin()
{
    temperatures.begin();

    temperatures.updateTemperatures();
    customFunctions.begin();
    relays.begin();
    smokeSensor.begin();

    validateCustomFunctions();
    checkCustomFunctions();
    updateRelays();
}

void MyHeatDevice::validateCustomFunctions()
{
    // Validation is now handled dynamically in the evaluator.
    // getSensorValue safely bounds checks indices.
    
    // Clear any alerts
    for (int i = 0; i < customFunctions.getCustomFunctionCount(); i++)
    {
        customFunctions.resetFunctionAlert(i);
    }
    
    // We can still save just in case, but no logic here.
    customFunctions.save();
}

void MyHeatDevice::checkCustomFunctions()
{
    std::vector<MyHeatCustomFunction>& cfArray = customFunctions.getCustomFunctions();
    
    EvalContext ctx {
        temperatures.getTemperatures(),
        relays.getRelays(),
        smokeSensor,
        0 // currentFunctionIndex
    };

    for (int i = 0; i < customFunctions.getCustomFunctionCount(); i++)
    {
        ctx.currentFunctionIndex = i;
        ctx.discoveredAlert = FunctionAlert::FA_NONE;
        
        customFunctions.evaluateFunction(cfArray[i], ctx);
        
        // Update the alert state based on what was discovered during evaluation
        customFunctions.setFunctionAlert(i, ctx.discoveredAlert);
    }
}

void MyHeatDevice::updateRelays()
{
    std::vector<MyHeatCustomFunction>& cfArray = customFunctions.getCustomFunctions();
    std::vector<MyHeatRelay>& relaysArray = relays.getRelays();

    struct RelayTarget {
        bool isActive = false;
        int maxPriority = -1;
    };
    std::vector<RelayTarget> activeRelays(relays.getRelayCount());

    // Evaluate all functions and resolve conflicts using Priority
    for (int i = 0; i < customFunctions.getCustomFunctionCount(); i++)
    {
        if (cfArray[i].getIsEnabled())
        {
            bool C = cfArray[i].getIsActive();
            int prio = cfArray[i].getPriority();

            // For every action this function performs
            for (auto& action : cfArray[i].getActions()) {
                byte relayIdx = action.targetIndex;
                if (relayIdx < activeRelays.size()) {
                    bool assertsState = false;
                    bool requestedState = (action.value == 1);

                    if (action.type == ACT_SET_RELAY) {
                        assertsState = C;
                    } 
                    else if (action.type == ACT_SET_RELAY_TIMER) {
                        uint32_t D = action.duration * 1000UL;
                        uint32_t currentMillis = millis();

                        Serial.printf("[Timer] Function %d Relay %d | Mode: %d | C: %d | prevC: %d | isRun: %d | start: %lu | now: %lu | D: %lu\n", 
                            i, relayIdx, action.timerType, C, action.prevConditionState, action.isTimerRunning, action.timerStartMillis, currentMillis, D);

                        if (action.timerType == 0) { 
                            // Type 0: Restart (Off-Delay)
                            if (C) {
                                if (!action.prevConditionState) Serial.println("[Timer] Type 0: Condition True! Starting timer.");
                                action.timerStartMillis = currentMillis;
                                action.isTimerRunning = true;
                                assertsState = true;
                            } else {
                                if (action.isTimerRunning) {
                                    if (currentMillis - action.timerStartMillis < D) {
                                        assertsState = true;
                                        Serial.printf("[Timer] Type 0: Running... Elapsed: %lu\n", currentMillis - action.timerStartMillis);
                                    } else {
                                        Serial.println("[Timer] Type 0: Expired!");
                                        action.isTimerRunning = false;
                                        assertsState = false;
                                    }
                                } else {
                                    assertsState = false;
                                }
                            }
                        } 
                        else if (action.timerType == 1) { 
                            // Type 1: Pulse (One-Shot)
                            if (C && !action.prevConditionState) { // Rising edge
                                Serial.println("[Timer] Type 1: Rising edge detected! Starting pulse.");
                                action.timerStartMillis = currentMillis;
                                action.isTimerRunning = true;
                            }
                            if (action.isTimerRunning) {
                                if (currentMillis - action.timerStartMillis < D) {
                                    assertsState = true;
                                    Serial.printf("[Timer] Type 1: Running... Elapsed: %lu\n", currentMillis - action.timerStartMillis);
                                } else {
                                    Serial.println("[Timer] Type 1: Expired!");
                                    action.isTimerRunning = false;
                                    assertsState = false;
                                }
                            } else {
                                assertsState = false;
                            }
                        }
                        else if (action.timerType == 2) { 
                            // Type 2: Cyclic (ON for D, OFF for D)
                            if (C && !action.isTimerRunning) {
                                Serial.println("[Timer] Type 2: Condition True! Starting cycle.");
                                action.timerStartMillis = currentMillis;
                                action.isTimerRunning = true;
                            }
                            if (action.isTimerRunning) {
                                uint32_t elapsed = currentMillis - action.timerStartMillis;
                                if (elapsed < D) {
                                    assertsState = true;
                                    Serial.printf("[Timer] Type 2: ON phase... Elapsed: %lu\n", elapsed);
                                } else {
                                    if (!C) {
                                        Serial.println("[Timer] Type 2: C is false during OFF phase. Aborting cycle.");
                                        action.isTimerRunning = false;
                                        assertsState = false;
                                    } else if (elapsed < D * 2) {
                                        assertsState = false;
                                        Serial.printf("[Timer] Type 2: OFF phase... Elapsed: %lu\n", elapsed);
                                    } else {
                                        Serial.println("[Timer] Type 2: Cycle complete! Restarting.");
                                        action.timerStartMillis = currentMillis;
                                        assertsState = true;
                                    }
                                }
                            } else {
                                assertsState = false;
                            }
                        }
                        
                        action.prevConditionState = C;
                    }

                    if (assertsState) {
                        // If this function has higher priority, or same priority but we haven't set it yet
                        if (prio > activeRelays[relayIdx].maxPriority) {
                            activeRelays[relayIdx].maxPriority = prio;
                            activeRelays[relayIdx].isActive = requestedState;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < relays.getRelayCount(); i++)
    {
        if (relaysArray[i].getMode() == RelayMode::OFF)
        {
            relaysArray[i].setIsActive(false);
        }
        else if (relaysArray[i].getMode() == RelayMode::ON)
        {
            relaysArray[i].setIsActive(true);
        }
        else
        {
            // AUTO mode
            if (activeRelays[i].maxPriority != -1) {
                // An active function claimed the relay
                relaysArray[i].setIsActive(activeRelays[i].isActive);
            } else {
                // No function is active (or all are disabled), turn off
                relaysArray[i].setIsActive(false);
            }
        }
    }
}

void MyHeatDevice::manualTick()
{
    tickTimerMain = millis();
    tickTimerSecondary = millis();
    checkCustomFunctions();
    updateRelays();
}

void MyHeatDevice::tick()
{
    if (millis() - tickTimerMain >= 5000)
    {
        tickTimerMain = millis();
        checkCustomFunctions();
    }

    if (millis() - tickTimerSecondary >= 750)
    {
        tickTimerSecondary = millis();
        temperatures.updateTemperatures();
        updateRelays();
        smokeSensor.updateSmokeSensor();
    }
}
