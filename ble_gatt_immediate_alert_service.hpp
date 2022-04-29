#ifndef BLE_GATT_IMMEDIATE_ALERT_SERVICE_H
#define BLE_GATT_IMMEDIATE_ALERT_SERVICE_H
#include "mbed.h"
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <events/mbed_events.h>
#include <stdio.h>
#include <chrono>
#include <ctime> 
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "gatt_server.hpp"
#include "ble_gatt_alert_notification_service.hpp"

class ImmediateAlertService: private mbed::NonCopyable<ImmediateAlertService>, public CGattService{
    public:
        ImmediateAlertService(BLE& ble, events::EventQueue &event_queue, AlertNotificationService &externService): 
        _led(LED3),
        CGattService(ble, event_queue),
        _extern_service(externService)
        
        {

        //decide the length for user description, adding uuid and characteristic
            ledOut = new PwmOut(LED3);
            ledOut->period_ms(200);
            ledOut->write(_dutycle);
            treesize = ARRAY_SIZE(_characteristics);
            for(int i=0; i < treesize; i++){
                    userDescription[i] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
                    (uint8_t *) _descriptionContent[i],
                    std::strlen(_descriptionContent[i]),
                    std::strlen(_descriptionContent[i]),
                    false);
            }

        set_long_random_UUID(uuid);

            uint8_t value0 = 0;
        _characteristics[0] = new CCharacteristic<uint8_t>(uuid[0],
                            _propertiesList[0],
                                        value0, userDescription+0, 1);
   
            std::cout << "IAS start" << std::endl;
            for(int i= 0; i<treesize; i++){
                // configure read security requirements
                _characteristics[i]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
                // configure write security requirements
                _characteristics[i]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
                // configure update security requirements
                _characteristics[i]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
            }

            _service = new GattService(GattService::UUID_IMMEDIATE_ALERT_SERVICE, _characteristics, treesize);
        }

        bool onWrite(const GattWriteCallbackParams& params){
            return params.handle == _characteristics[0]->getValueHandle();
        }

        virtual void onDataWritten(const GattWriteCallbackParams &params){
            std::cout << "call IAS" << std::endl;
            if(onWrite(params)){
                    std::cout << "EVEN value; turning " << params.data[0]  << std::endl;
                    _dutycle = 0.010f*(_levels[params.data[0]]);
                    std::cout << "Status: " << _status[params.data[0]] << std::endl;
                    ledOut->write(_dutycle);
                    _extern_service.switchLevel(_levels[params.data[0]] == 100?false:true);
    
            }
            else{
                std::cout << "call from outside" << std::endl;
                _extern_service.onDataWritten(params);
            }
         
        };

        ~ImmediateAlertService()
        {
            for (int i = 0; i < ARRAY_SIZE(userDescription); i++){
                delete userDescription[i];
            }
        
                for (int i = 0; i < ARRAY_SIZE(_characteristics); i++){
                delete _characteristics[i];
            }
        
            delete _service;
            delete ledOut;
        }
      

    private:
        //GattAttribute attr;
        // declare a value of 2 bytes within a 10 bytes buffer
        DigitalOut _led;
        PwmOut* ledOut;
        float _dutycle = 100;
        AlertNotificationService& _extern_service;        
        GattAttribute* userDescription[1];
        const char* _descriptionContent[1] = {
            IASCHARACTERISTIC_DESCRIPTION
            };
        GattCharacteristic* _characteristics[1];
        const uint8_t _propertiesList[1] = {
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | 
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE
        };
        UUID::ShortUUIDBytes_t uuid[1];
        enum options{
            NoAlert = 100,
            Medium = 75,
            Hight = 10
        };
        const options _levels[3] = {NoAlert, Medium, Hight};
        const char* _status[3] = {"NO ALERT", "MEDIUM", "HIGH"};

};

#endif