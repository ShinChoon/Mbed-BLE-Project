
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
#include "gatt_server.hpp"

class ImmediateAlertService: private mbed::NonCopyable<ImmediateAlertService>, public CGattService{
    public:
        ImmediateAlertService(BLE& ble, events::EventQueue &event_queue):_ble(ble), 
        _event_queue(event_queue), _led_3(LED3),
        CGattService(_ble, _event_queue){

        }
        ~ImmediateAlertService();
        void run(void);
        virtual void onDataWritten(const GattWriteCallbackParams &params) override;
        virtual void onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params) override;
        virtual void onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params) override;
        static void kernel_clock_update(CCharacteristic<uint32_t>* gatt_characteristic);


    private:
        //GattAttribute attr;
        // declare a value of 2 bytes within a 10 bytes buffer
        DigitalOut _led_3;
        uint8_t attribute_value[10] = {};
        GattAttribute* userDescription[3];
        GattCharacteristic* _characteristics[3];
        GattService* _service;
        ble::BLE& _ble;
        events::EventQueue& _event_queue;
    
};
