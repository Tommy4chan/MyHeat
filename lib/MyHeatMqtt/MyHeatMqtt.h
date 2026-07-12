#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "MyHeatDevice.h"
#include "MyHeatSave.h"
#include "MyHeatUtils.h"

namespace MyHeatMqtt
{
    // ----- Persisted settings -----
    struct MqttSettings : public MyHeatSaveInterface
    {
        String broker = MQTT_BROKER;
        uint16_t port = MQTT_PORT;
        String user = MQTT_USER;
        String password = MQTT_PASSWORD;
        bool isEnabled = MQTT_ENABLED;
        uint32_t publishInterval = MQTT_PUBLISH_INTERVAL;

        void serialize(JsonDocument &json) override;
        void deserialize(JsonDocument &json) override;
        void manualDeserialize(JsonDocument json) override;
    };

    // ----- State -----
    extern WiFiClient espClient;
    extern PubSubClient mqtt;
    extern MyHeatDevice &myHeatDevice;
    extern MyHeatSave *mqttData;
    extern MqttSettings settings;

    extern String chipId;
    extern String baseTopic;
    extern String clientId;
    extern String availTopic;

    extern uint32_t lastPublishTick;
    extern uint32_t lastConnectAttempt;
    extern bool wasConnected;

    extern byte lastTemperatureCount;
    extern byte lastRelayCount;
    extern byte lastFunctionCount;

    // ----- Lifecycle -----
    void begin();
    void tick();

    // ----- Connection -----
    void connect();
    void disconnect();
    bool isConnected();

    // ----- Discovery -----
    void publishAllDiscoveryConfigs();
    void publishTemperatureDiscovery(byte index);
    void publishRelayModeDiscovery(byte index);
    void publishRelayActiveDiscovery(byte index);
    void publishFunctionDiscovery(byte index);
    void publishSmokeDiscovery();
    void publishSmokeValueDiscovery();
    void clearDiscoveryConfig(const String &component, const String &objectId);

    // ----- State publishing -----
    void publishAllStates();
    void publishTemperatureState(byte index);
    void publishRelayModeState(byte index);
    void publishRelayActiveState(byte index);
    void publishFunctionState(byte index);
    void publishSmokeState();
    void publishSmokeValueState();

    // ----- Command handling -----
    void onMessage(char *topic, byte *payload, unsigned int length);
    void subscribeToCommands();
    void handleRelayModeCommand(byte index, const String &value);
    void handleFunctionEnabledCommand(byte index, const String &value);

    // ----- Settings -----
    String getBroker();
    uint16_t getPort();
    String getUser();
    String getPassword();
    bool getIsEnabled();
    uint32_t getPublishInterval();
    void setSettings(const String &broker, uint16_t port, 
                     const String &user, const String &password,
                     bool isEnabled, uint32_t publishInterval);
    void save();

    // ----- Device JSON helper -----
    void addDeviceBlock(JsonDocument &doc);
    void addAvailability(JsonDocument &doc, const String& extraAvailTopic = "");
    void handleEntityCountChange();
}
