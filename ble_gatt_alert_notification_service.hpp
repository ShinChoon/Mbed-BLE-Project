
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
        AlertNotificationService(BLE& ble, events::EventQueue &event_queue):
        _event(BUTTON1),
        CGattService(ble, event_queue)
    {
        //decide the length for user description, adding uuid and characteristic
    treesize = 4;
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
                                  
    _button_count = 0;     
  _characteristics[1] = new CCharacteristic<uint8_t>(uuid[1],
                      _propertiesList[1],
                                  _button_count, userDescription+1, 1);

    uint8_t value2 = 0; 
  _characteristics[2] = new CCharacteristic<uint8_t>(uuid[2],
                      _propertiesList[2],
                                  value2, userDescription+2, 1);

    std::cout << "ANS start" << std::endl;
    //for(int i= 0; i<treesize-1; i++){
    //    // configure read security requirements
    //    _characteristics[i]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    //    // configure write security requirements
    //    _characteristics[i]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    //    // configure update security requirements
    //    _characteristics[i]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    //}

    _service = new GattService(uuid[treesize-1], _characteristics, treesize-1);
    

        _event.fall(Callback<void()> (this, &AlertNotificationService::button_P_str));
    }

        //~AlertNotificationService();
        virtual void onDataWritten(const GattWriteCallbackParams &params){
            if(params.handle == _characteristics[2]->getValueHandle()){
                    switchAlert(params.data[0] % 2 == 1?true:false);
                    std::cout << "EVEN value; turning " <<((_alert_switch)?"ture":"false")<< std::endl;
            }
        }
        virtual void onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params){
            std::cout << "Updates are enabled. Starting writing update_button_num values.." << std::endl;

        }
        virtual void onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params){
            std::cout << "Updates are disabled. Stopping writing values.." << std::endl;
        };

        void update_button_num(void){
            printf("button num: %d\r\n", _button_count);
            CCharacteristic<uint8_t>* gatt_characteristic = (CCharacteristic<uint8_t>*)_characteristics[1];
            gatt_characteristic->set(BLE::Instance().gattServer(), _button_count);

        }

        void update_button_alert(void){
            std::string write_val = "28";
            std::cout << Kernel::Clock::now().time_since_epoch().count() << std::endl;
            CCharacteristic<const char*>* gatt_characteristic = (CCharacteristic<const char*>*)_characteristics[0];
            gatt_characteristic->set(BLE::Instance().gattServer(), write_val.c_str());
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
        UUID::LongUUIDBytes_t uuid[4];
        uint8_t _button_count = 0;
    
};
