#include <configuration.h>

//——————————————————————————————————————————————————————————————————————————————
//  Configuration File
//——————————————————————————————————————————————————————————————————————————————

const char *configFileName = (char *)"/configuration.json";

Configuration configuration;

// Converters

void convertFromJson(JsonVariantConst src, byte &dst)
{
    dst = strtoul(src.as<const char *>(), NULL, 16);
}

bool ReadConfiguration()
{
    // Init SPIFFS
    if (!SPIFFS.begin())
        SPIFFS.begin(true);

    if (!SPIFFS.exists(configFileName))
    {
        WriteToConsoles("Configuration file could not be found. Please upload it first.\r\n");
        return false;
    }

    File file = SPIFFS.open(configFileName);

    if (!file)
    {
        WriteToConsoles("Configuration file could not be loaded. Consider checking and reuploading it.\r\n");
        return false;
    }

    // Open and parse the file
    const int docSize = 3072;
    StaticJsonDocument<docSize> doc;
    DeserializationError error = deserializeJson(doc, file);

    if (error)
    {
        WriteToConsoles("Error processing configuration: ");
        WriteToConsoles(error.c_str());
        WriteToConsoles("\r\n");
        return false;
    }

    if (Debug)
    {

        serializeJsonPretty(doc, Serial);
    }

    strlcpy(configuration.Wifi.SSID, doc["Wifi"]["SSID"], sizeof(configuration.Wifi.SSID));             // "ssid"
    strlcpy(configuration.Wifi.Password, doc["Wifi"]["Password"], sizeof(configuration.Wifi.Password)); // "pass"
    strlcpy(configuration.Wifi.Hostname, doc["Wifi"]["Hostname"], sizeof(configuration.Wifi.Hostname)); // "CERASMARTER"

    JsonObject MQTT = doc["MQTT"];
    strlcpy(configuration.Mqtt.Server, MQTT["Server"], sizeof(configuration.Mqtt.Server));       // "192.168.1.123"
    configuration.Mqtt.Port = MQTT["Port"];                                                      // 1883
    strlcpy(configuration.Mqtt.User, MQTT["User"], sizeof(configuration.Mqtt.User));             // "user"
    strlcpy(configuration.Mqtt.Password, MQTT["Password"], sizeof(configuration.Mqtt.Password)); // "pass"

    JsonObject MQTT_Topics = MQTT["Topics"];
    strlcpy(configuration.Mqtt.Topics.HeatingValues, MQTT_Topics["HeatingValues"], sizeof(configuration.Mqtt.Topics.HeatingValues));    //"cerasmarter/heating/values"
    strlcpy(configuration.Mqtt.Topics.WaterValues, MQTT_Topics["WaterValues"], sizeof(configuration.Mqtt.Topics.WaterValues));          //"cerasmarter/water/values"
    strlcpy(configuration.Mqtt.Topics.HeatingParameters, MQTT_Topics["HeatingParameters"], sizeof(configuration.Mqtt.Topics.HeatingParameters));    //"cerasmarter/heating/parameters"
    strlcpy(configuration.Mqtt.Topics.WaterParameters, MQTT_Topics["WaterParameters"], sizeof(configuration.Mqtt.Topics.WaterParameters));          //"cerasmarter/water/parameters"
    strlcpy(configuration.Mqtt.Topics.AuxilaryValues, MQTT_Topics["AuxilaryParameters"], sizeof(configuration.Mqtt.Topics.AuxilaryValues)); //"cerasmarter/auxilary/parameters"
    strlcpy(configuration.Mqtt.Topics.Status, MQTT_Topics["Status"], sizeof(configuration.Mqtt.Topics.Status));                                     // "cerasmarter/status"
    strlcpy(configuration.Mqtt.Topics.StatusRequest, MQTT_Topics["StatusRequest"], sizeof(configuration.Mqtt.Topics.StatusRequest));                                     // "cerasmarter/status/get"

    JsonObject Features = doc["Features"];
    configuration.Features.Features_HeatingParameters = Features["HeatingParameters"];   // true
    configuration.Features.Features_WaterParameters = Features["WaterParameters"];       // false
    configuration.Features.Features_AuxilaryParameters = Features["AuxilaryParameters"]; // false

    JsonObject TimeSettings = doc["Time"];
    strlcpy(configuration.General.Time_Timezone, TimeSettings["Timezone"], sizeof(configuration.General.Time_Timezone)); // true

    JsonObject GeneralSettings = doc["General"];
    configuration.General.BusMessageTimeout = GeneralSettings["BusMessageTimeout"];
    configuration.General.Debug = GeneralSettings["Debug"];
    configuration.General.Sniffing = GeneralSettings["Sniffing"];

    JsonObject Leds = doc["LEDs"];
    configuration.LEDs.WifiLed = Leds["Wifi"];       // 26
    configuration.LEDs.StatusLed = Leds["Status"];   // 27
    configuration.LEDs.MqttLed = Leds["Mqtt"];       // 14
    configuration.LEDs.HeatingLed = Leds["Heating"]; // 25

    JsonObject CAN = doc["CAN"];
    configuration.CanModuleConfig.CAN_SCK = CAN["SCK"];
    configuration.CanModuleConfig.CAN_MISO = CAN["MISO"];
    configuration.CanModuleConfig.CAN_MOSI = CAN["MOSI"];
    configuration.CanModuleConfig.CAN_CS = CAN["CS"];
    configuration.CanModuleConfig.CAN_INT = CAN["INT"];
    configuration.CanModuleConfig.CAN_Quartz = CAN["Quartz"];

    JsonObject CAN_Addresses = CAN["Addresses"];

    JsonObject CAN_Addresses_Controller = CAN_Addresses["Controller"];
    configuration.CanAddresses.General.FlameLit = CAN_Addresses_Controller["FlameStatus"].as<byte>(); // "0x209"
    configuration.CanAddresses.General.Error = CAN_Addresses_Controller["Error"].as<byte>();          // "0x206"
    configuration.CanAddresses.General.DateTime = CAN_Addresses_Controller["DateTime"].as<byte>();    // "0x256"

    JsonObject CAN_Addresses_Heating = CAN_Addresses["Heating"];
    configuration.CanAddresses.Heating.FeedCurrent = CAN_Addresses_Heating["FeedCurrent"].as<byte>();               // "0x201"
    configuration.CanAddresses.Heating.FeedMax = CAN_Addresses_Heating["FeedMax"].as<byte>();                       // "0x200"
    configuration.CanAddresses.Heating.FeedSetpoint = CAN_Addresses_Heating["FeedSetpoint"].as<byte>();             // "0x252"
    configuration.CanAddresses.Heating.OutsideTemperature = CAN_Addresses_Heating["OutsideTemperature"].as<byte>(); // "0x207"
    configuration.CanAddresses.Heating.Pump = CAN_Addresses_Heating["Pump"].as<byte>();                             // "0x20A"
    configuration.CanAddresses.Heating.Season = CAN_Addresses_Heating["Season"].as<byte>();                         // "0x20C"
    configuration.CanAddresses.Heating.Operation = CAN_Addresses_Heating["Operation"].as<byte>();                   // "0x250"
    configuration.CanAddresses.Heating.Power = CAN_Addresses_Heating["Power"].as<byte>();                           // "0x251"

    JsonObject CAN_Addresses_HotWater = CAN_Addresses["HotWater"];
    configuration.CanAddresses.HotWater.SetpointTemperature = CAN_Addresses_HotWater["SetpointTemperature"].as<byte>(); // "0x203"
    configuration.CanAddresses.HotWater.MaxTemperature = CAN_Addresses_HotWater["MaxTemperature"].as<byte>();           // "0x204"
    configuration.CanAddresses.HotWater.CurrentTemperature = CAN_Addresses_HotWater["CurrentTemperature"].as<byte>();   // "0x205"
    configuration.CanAddresses.HotWater.Now = CAN_Addresses_HotWater["Now"].as<byte>();                                 // "0x254"
    configuration.CanAddresses.HotWater.BufferOperation = CAN_Addresses_HotWater["BufferOperation"].as<byte>();         // "0x20B"

    configuration
        .CanAddresses
        .HotWater
        .ContinousFlowSetpointTemperature = CAN_Addresses_HotWater["ContinousFlow"]["SetpointTemperature"].as<byte>(); // "0x255"

    JsonObject CAN_Addresses_MixedCircuit = CAN_Addresses["MixedCircuit"];
    configuration.CanAddresses.MixedCircuit.Pump = CAN_Addresses_MixedCircuit["Pump"].as<byte>();                 // "0x404"
    configuration.CanAddresses.MixedCircuit.FeedSetpoint = CAN_Addresses_MixedCircuit["FeedSetpoint"].as<byte>(); // "0x405"
    configuration.CanAddresses.MixedCircuit.FeedCurrent = CAN_Addresses_MixedCircuit["FeedCurrent"].as<byte>();   // "0x440"
    configuration.CanAddresses.MixedCircuit.Economy = CAN_Addresses_MixedCircuit["Economy"].as<byte>();           // "0x407"

    int AuxilarySensors_Count = doc["AuxilarySensors"]["Count"]; // 4
    // This might cause trouble if too many sensors are added... we'll have to see where this is going.
    configuration.TemperatureSensors.Sensors = (Sensor *)malloc(AuxilarySensors_Count * sizeof(Sensor));

    int curSensor = 0;
    bool tempReferenceSensorSet = false;
    for (JsonObject AuxilarySensors_Sensor : doc["AuxilarySensors"]["Sensors"].as<JsonArray>())
    {
        Sensor newSensor;
        strlcpy(newSensor.Label, AuxilarySensors_Sensor["Label"], sizeof(newSensor.Label)); // true
        newSensor.UseAsReturnValueReference = AuxilarySensors_Sensor["IsReturnValue"].as<bool>();
        JsonArray AuxilarySensors_Sensor_Address = AuxilarySensors_Sensor["Address"];
        int i = 0;
        for (JsonVariant value : AuxilarySensors_Sensor_Address)
        {
            byte addrByte = strtoul(value.as<const char *>(), NULL, 16);
            newSensor.Address[i++] = addrByte;
        }
        configuration.TemperatureSensors.Sensors[curSensor++] = newSensor;
        if(Debug)
        {
            if (newSensor.UseAsReturnValueReference && tempReferenceSensorSet)
            {
                Serial.printf("WARN: Sensor #%i is set as temperature reference but another sensor has been already set.");
            }            
            if(newSensor.UseAsReturnValueReference)
            {
                Serial.println("INFO: The following sensor will be used as a return temperature reference.");
                tempReferenceSensorSet = true;
            }
            Serial.printf("Added Sensor #%i with Label '%s'\r\n", curSensor, newSensor.Label);
        }
    }
    // Set the amount of sensors
    configuration.TemperatureSensors.SensorCount = AuxilarySensors_Count;

    // We don't need to keep it open at this point.
    SPIFFS.end();

    return true;
}
