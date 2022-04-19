#ifndef CHARACTERISTIC_H
#define CHARACTERISTIC_H
#include "ble/gatt/GattAttribute.h"
#include "ble/GattServer.h"
#include "ble/gatt/GattCharacteristic.h"

#define BASECHARACTERISTIC_DESCRIPTION_1 "LED2"
#define BASECHARACTERISTIC_DESCRIPTION_2 "DummyConfig"
#define BASECHARACTERISTIC_DESCRIPTION_3 "TickCount"

#define ANSCHARACTERISTIC_DESCRIPTION_1 "Supported New Alert Category"
#define ANSCHARACTERISTIC_DESCRIPTION_2 "New Alert"
#define ANSCHARACTERISTIC_DESCRIPTION_3 "Supported Unread Alert Category"
#define ANSCHARACTERISTIC_DESCRIPTION_4 "Unread Alert Status"
#define ANSCHARACTERISTIC_DESCRIPTION_5 "Alert Notifi cation Control Point"
#define IASCHARACTERISTIC_DESCRIPTION "Alert Level"

template <typename T>
class CCharacteristic : 
public GattCharacteristic {
    public:
        CCharacteristic(const UUID &uuid,
                 uint8_t properties,
                 T &initialValue,
                 GattAttribute *descriptors[] = NULL,
                 int numOfDescriptors = 0):_uuid(uuid), _properties(properties),_value(initialValue),
                 GattCharacteristic(uuid, (uint8_t*)&initialValue, sizeof(T), sizeof(T), properties, descriptors, numOfDescriptors, false){
                 };
        void set(GattServer& server, const T &value){
            setAttributeValue(server, value);
        };
        const T& get(const GattServer &server,
                              T &dst){
            _value = getAttributeValue(server,_value);
            return _value;
        }

    private:
        T& _value;
        int _numOfDescriptors;
        uint8_t _properties;
        UUID _uuid;
        ble_error_t getAttributeValue(const GattServer &server,
                              T &dst) const {
        uint16_t valueSize = sizeof(T);
        return server.read(this->getValueHandle(), &dst, &valueSize);
        }

        ble_error_t setAttributeValue(GattServer& server,
                              const T &value,
                              bool localOnly = false) 
        const{
        return server.write(this->getValueHandle(), 
                reinterpret_cast<const uint8_t*>(&value), 
                sizeof(value), 
                localOnly);
        }
        
};

#endif