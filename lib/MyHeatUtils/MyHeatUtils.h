#ifndef MYHEATUTILS_H
#define MYHEATUTILS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <vector>


namespace MyHeatUtils
{
    String getConvertedStateToText(byte state);
    String getConvertedActiveToText(bool active);
    String getCurrentDateAndTime();
    // Why the chip started: machine-readable name for HA and Ukrainian text for messages
    const char *getResetReasonName();
    String getResetReasonText();
    // Flash writes during OTA stall the CPU and break timing sensitive reads (OneWire), the upload marks its activity here
    void markFlashUpdateActivity();
    void clearFlashUpdateActivity();
    bool isFlashUpdateInProgress();
    String getAddressToString(uint8_t address[8]);
    JsonDocument getFileContent(String path);
    bool isFloat(const String& str);

    // ----- Change detection -----
    // FNV-1a 32-bit: a few cycles per byte, no allocation. Used to detect changed payloads without storing them

    constexpr uint32_t HASH_SEED = 2166136261u;

    inline uint32_t hashBytes(const uint8_t *data, size_t length, uint32_t hash = HASH_SEED)
    {
        for (size_t i = 0; i < length; i++)
        {
            hash = (hash ^ data[i]) * 16777619u;
        }
        return hash;
    }

    inline uint32_t hashString(const char *text, uint32_t hash = HASH_SEED)
    {
        return hashBytes(reinterpret_cast<const uint8_t *>(text), strlen(text), hash);
    }

    // ArduinoJson output that hashes the serialized JSON instead of building a String
    struct HashWriter
    {
        uint32_t hash = HASH_SEED;

        size_t write(uint8_t c)
        {
            hash = hashBytes(&c, 1, hash);
            return 1;
        }

        size_t write(const uint8_t *buffer, size_t length)
        {
            hash = hashBytes(buffer, length, hash);
            return length;
        }
    };

    inline uint32_t hashJson(JsonVariantConst json)
    {
        HashWriter writer;
        serializeJson(json, writer);
        return writer.hash;
    }

    // Remembers the last hash per slot (one slot per entity or message type)
    class ChangeTracker
    {
    private:
        static constexpr uint32_t UNKNOWN = 0; // A real hash of 0 only costs one extra resend

        std::vector<uint32_t> hashes;

    public:
        // Returns true and stores the hash when it differs from the last one for this slot
        bool update(size_t slot, uint32_t hash)
        {
            if (slot >= hashes.size())
            {
                hashes.resize(slot + 1, UNKNOWN);
            }

            if (hashes[slot] != UNKNOWN && hashes[slot] == hash)
            {
                return false;
            }

            hashes[slot] = hash;
            return true;
        }

        // Forget a slot so its next update counts as changed, e.g. after a failed send
        void invalidate(size_t slot)
        {
            if (slot < hashes.size())
            {
                hashes[slot] = UNKNOWN;
            }
        }

        void reset()
        {
            std::fill(hashes.begin(), hashes.end(), UNKNOWN);
        }
    };
}

#endif
