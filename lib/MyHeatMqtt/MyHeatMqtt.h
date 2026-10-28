#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "MyHeatDevice.h"
#include "MyHeatSave.h"
#include "MyHeatUtils.h"
#include "MyHeatAlerts.h"

#ifndef MQTT_DEVICE_NAME
#define MQTT_DEVICE_NAME "MyHeat Controller"
#endif

#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION "dev"
#endif

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
        String deviceName = MQTT_DEVICE_NAME;

        void serialize(JsonDocument &json) override;
        void deserialize(const JsonDocument &json) override;
        void manualDeserialize(const JsonDocument& json) override;
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
    extern uint32_t lastStateRevision;
    extern uint32_t lastConnectAttempt;
    extern bool wasConnected;
    extern bool isRediscoveryRequested;
    extern bool isRestartRequested;

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
    void restart();

    // ----- Discovery -----
    void publishAllDiscoveryConfigs();
    void publishTemperatureDiscovery(byte index);
    void publishRelaySwitchDiscovery(byte index);
    void publishRelayModeDiscovery(byte index);
    void publishRelayActiveDiscovery(byte index);
    void publishFunctionDiscovery(byte index);
    void publishFunctionNameChanges();
    void publishSmokeDiscovery();
    void publishDiagnosticsDiscovery();
    void publishRestartDiscovery();
    void publishAlertsDiscovery();
    void clearDiscoveryConfig(const String &component, const String &objectId);
    JsonDocument createDiscoveryDoc(const String &component, const String &objectId, const String &name);
    JsonDocument createDiagnosticDoc(const String &objectId, const String &name, const String &valueKey);
    void publishDiscovery(const String &component, const String &objectId, JsonDocument &doc);
    String getFunctionName(byte index);

    // ----- State publishing -----
    void resetTrackers();
    void publishState(MyHeatUtils::ChangeTracker &tracker, size_t slot, const char *topic, const char *payload, bool force);
    void publishAllStates(bool force);
    void publishTemperatureState(byte index, bool force);
    void publishRelayModeState(byte index, bool force);
    void publishRelayActiveState(byte index, bool force);
    void publishFunctionState(byte index, bool force);
    void publishSmokeState(bool force);
    void publishDiagnostics();

    // ----- Alerts -----
    void onAlertEvent(const AlertEvent &event);
    void publishPendingAlertEvents();

    // ----- Command handling -----
    void onMessage(char *topic, byte *payload, unsigned int length);
    void subscribeToCommands();
    void handleRelayModeCommand(byte index, const String &value);
    void handleRelaySwitchCommand(byte index, const String &value);
    void applyRelayMode(byte index, RelayMode mode);
    void handleFunctionEnabledCommand(byte index, const String &value);

    // ----- Settings -----
    String getBroker();
    uint16_t getPort();
    String getUser();
    String getPassword();
    bool getIsEnabled();
    uint32_t getPublishInterval();
    String getDeviceName();
    void setSettings(const String &broker, uint16_t port, 
                     const String &user, const String &password,
                     bool isEnabled, uint32_t publishInterval,
                     const String &deviceName);
    void save();

    // ----- Device JSON helper -----
    void addDeviceBlock(JsonDocument &doc);
    void addAvailability(JsonDocument &doc, const String& extraAvailTopic = "");
    void handleEntityCountChange();
}
