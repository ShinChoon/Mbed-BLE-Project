#ifndef BLE_GATT_ALERT_NOTIFICATION_SERVICE_H
#define BLE_GATT_ALERT_NOTIFICATION_SERVICE_H
#include "mbed.h"
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <events/mbed_events.h>
#include <stdio.h>
#include <string>
#include <chrono>
#include <ctime> 
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "gatt_server.hpp"



class AlertNotificationService: private mbed::NonCopyable<AlertNotificationService>, public CGattService{
    public:
        AlertNotificationService(BLE& ble, events::EventQueue &event_queue, CGattService &externService):
        _event(BUTTON1),
        CGattService(ble, event_queue),
        _extern_service(externService)
        {
            //decide the length for user description, adding uuid and characteristic
            treesize = ARRAY_SIZE(_characteristics);
            for(int i=0; i<treesize; i++){
                    userDescription[i] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
                    (uint8_t *) _descriptionContent[i],
                    std::strlen(_descriptionContent[i]),
                    std::strlen(_descriptionContent[i]),
                    false);
            }

        set_long_random_UUID(uuid);

            const char* value0 = 0;
        _characteristics[0] = new CCharacteristic<const char*>(uuid[0],
                            _propertiesList[0],
                                        value0, userDescription+0, 1);

            _button_count = 0;     
        _characteristics[1] = new CCharacteristic<uint8_t>(uuid[1],
                            _propertiesList[1],
                                        _button_count, userDescription+1, 1);

            uint8_t value2 = 0; 
        _characteristics[2] = new CCharacteristic<uint8_t>(uuid[2],
                            _propertiesList[2],
                                        value2, userDescription+2, 1);

            std::cout << "ANS start" << std::endl;

            for(int i= 0; i<treesize; i++){
                // configure read security requirements
                _characteristics[i]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
                // configure write security requirements
                _characteristics[i]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
                // configure update security requirements
                _characteristics[i]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
            }

            _service = new GattService(GattService::UUID_ALERT_NOTIFICATION_SERVICE, _characteristics, treesize);
            _event.fall(Callback<void()> (this, &AlertNotificationService::button_P_str));
            switchAlert(false);
        }

        bool onWrite(const GattWriteCallbackParams& params){
            return params.handle == _characteristics[2]->getValueHandle();
        }

        virtual void onDataWritten(const GattWriteCallbackParams &params){
            std::cout << "call NAS" << std::endl;
            if (onWrite(params))
            {
                    switchAlert(params.data[0] % 2 == 1?true:false);
                    std::cout << "EVEN value; turning " <<((_alert_switch)?"ture":"false")<< std::endl;
            }
            else{
                std::cout << "call from outside" << std::endl;
                _extern_service.onDataWritten(params);
            }

        }

        void update_button_num(void){
            printf("button num: %d\r\n", _button_count);
            CCharacteristic<uint8_t>* gatt_characteristic = (CCharacteristic<uint8_t>*)_characteristics[1];
            gatt_characteristic->set(BLE::Instance().gattServer(), _button_count);

        }

        void update_button_alert(void){
            const char* write_val = "SIMPLE ALERT";
            std::cout << Kernel::Clock::now().time_since_epoch().count() << std::endl;
            CCharacteristic<const char*>* gatt_characteristic = (CCharacteristic<const char*>*)_characteristics[0];
            gatt_characteristic->set(BLE::Instance().gattServer(), write_val);
        }        

        void button_P_str(void){
            _button_count++;
            if(_alert_switch){
            _event_queue.call(Callback<void()>(this, &AlertNotificationService::update_button_num));// SIMPLE ALERT
            _event_queue.call(Callback<void()>(this, &AlertNotificationService::update_button_alert));//button press            
            }
        }   

        void switchAlert(bool swi){
            _alert_switch = swi;
        }

        ~AlertNotificationService()
        {
            for (int i = 0; i < ARRAY_SIZE(userDescription); i++){
                delete userDescription[i];
            }

                for (int i = 0; i < ARRAY_SIZE(_characteristics); i++){
                delete _characteristics[i];
            }

            delete _service;
        }


    private:
        //GattAttribute attr;
        // declare a value of 2 bytes within a 10 bytes buffer
        bool _alert_switch = false;
        CGattService& _extern_service;
        int treesize = 0;
        InterruptIn _event;
        GattAttribute* userDescription[3];
        GattCharacteristic* _characteristics[3];
        const char* _descriptionContent[3] = {  
                        ANSCHARACTERISTIC_DESCRIPTION_1,
                        ANSCHARACTERISTIC_DESCRIPTION_2,
                        ANSCHARACTERISTIC_DESCRIPTION_3,
                    };
        const uint8_t _propertiesList[3] = {
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE
                        };
        UUID::ShortUUIDBytes_t uuid[4];
        uint8_t _button_count = 0;
    
};

#endif