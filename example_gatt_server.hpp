
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
        ~CGattService();
        void run(void);
        virtual void onDataWritten(const GattWriteCallbackParams &params) override;
        virtual void onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params) override;
        virtual void onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params) override;
        static void kernel_clock_update(CCharacteristic<uint32_t>* gatt_characteristic);


    private:
        //GattAttribute attr;
        // declare a value of 2 bytes within a 10 bytes buffer
        DigitalOut _led_2;
        uint8_t attribute_value[10] = {};
        GattAttribute* userDescription[3];
        GattCharacteristic* _characteristics[3];
        GattService* _service;
        UUID::LongUUIDBytes_t uuid[4];
        ble::BLE& _ble;
        events::EventQueue& _event_queue;
        void set_random_UUID(UUID::LongUUIDBytes_t* uuid);
        int _kernel_event_id;
    
};
