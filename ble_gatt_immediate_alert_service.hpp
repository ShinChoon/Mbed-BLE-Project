
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

class ImmediateAlertService: private mbed::NonCopyable<ImmediateAlertService>, public CGattService{
    public:
        ImmediateAlertService(BLE& ble, events::EventQueue &event_queue): 
        _led(LED3),
        CGattService(ble, event_queue)
        {

        //decide the length for user description, adding uuid and characteristic
            ledOut = new PwmOut(LED3);
            ledOut->period(1.0f);
            ledOut->write(_dutycle);
            treesize = 2;
            for(int i=0; i<treesize-1; i++){
                    userDescription[i] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
                    (uint8_t *) _descriptionContent[i],
                    std::strlen(_descriptionContent[i]),
                    std::strlen(_descriptionContent[i]),
                    false);
            }

            for (int k = 0; k < treesize; k++) {
                for (int i = 0; i < 16; i++) {
                    uuid[k][i] = rand() % 256;
                }
            }

            const char* value0 = 0;
        _characteristics[0] = new CCharacteristic<const char*>(uuid[0],
                            _propertiesList[0],
                                        value0, userDescription+0, 1);
   
            std::cout << "IAS start" << std::endl;
            //for(int i= 0; i<treesize-1; i++){
            //    // configure read security requirements
            //    _characteristics[i]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
            //    // configure write security requirements
            //    _characteristics[i]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
            //    // configure update security requirements
            //    _characteristics[i]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
            //}

            _service = new GattService(uuid[treesize-1], _characteristics, treesize-1);
        }

        virtual void onDataWritten(const GattWriteCallbackParams &params){
            if(params.handle == _characteristics[0]->getValueHandle()){
                    std::cout << "EVEN value; turning " << std::endl;
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
        float _dutycle = 0.20f;
        GattAttribute* userDescription[1];
        const char* _descriptionContent[1] = {
            IASCHARACTERISTIC_DESCRIPTION
            };
        GattCharacteristic* _characteristics[1];
        const uint8_t _propertiesList[1] = {
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | 
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE
        };
        UUID::LongUUIDBytes_t uuid[2];
    
};
