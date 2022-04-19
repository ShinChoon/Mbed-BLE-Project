#ifndef GATT_SERVER_H
#define GATT_SERVER_H
#include "mbed.h"
#include <iostream>
#include <stdio.h>
#include <events/mbed_events.h>
#include <stdio.h>
#include <chrono>
#include <ctime> 
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "characteristic.hpp"

class CGattService: private mbed::NonCopyable<CGattService>, public ble::GattServer::EventHandler{
    public:
        CGattService(BLE& ble, events::EventQueue &event_queue);

        CGattService(BLE& ble, events::EventQueue &event_queue, 
                    GattAttribute* userdes[3], GattCharacteristic* chara[3], const char* desc[3],
                    const uint8_t properyties[3],UUID::LongUUIDBytes_t uuid[4]);
                    
        CGattService(BLE& ble, events::EventQueue &event_queue, 
                    GattAttribute* userdes[1],GattCharacteristic* chara[1], const char* desc[1],
                    const uint8_t properyties[1],UUID::LongUUIDBytes_t uuid[2], int size);
        ~CGattService();
        virtual void run(void);
        virtual void onDataWritten(const GattWriteCallbackParams &params) override;
        virtual void onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params) override;
        virtual void onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params) override;
        static void kernel_clock_update(CCharacteristic<uint32_t>* gatt_characteristic);


    protected:
        //GattAttribute attr;
        // declare a value of 2 bytes within a 10 bytes buffer
        uint8_t attribute_value[10] = {};
        GattAttribute* userDescription[3];
        GattCharacteristic* _characteristics[3];
        const char* _descriptionContent[3]={
                            BASECHARACTERISTIC_DESCRIPTION_1,
                            BASECHARACTERISTIC_DESCRIPTION_2,
                            BASECHARACTERISTIC_DESCRIPTION_3
                            };
        const uint8_t _propertiesList[3] = {GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
                                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE, 
                                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
                                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE };
        UUID::LongUUIDBytes_t _uuid[4];
        GattService* _service;
        ble::BLE& _ble;
        int _kernel_event_id;
        int treesize = 4;
        events::EventQueue& _event_queue;
        void set_random_UUID(UUID::LongUUIDBytes_t* uuid);

};

#endif