
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

#define ANS_DESCRIPTION_1 "Supported New Alert Category"
#define ANS_DESCRIPTION_2 "New Alert"
#define ANS_DESCRIPTION_3 "Supported Unread Alert Category"
#define ANS_DESCRIPTION_4 "Unread Alert Status"
#define ANS_DESCRIPTION_5 "Alert Notification Control Point"


class AlertNotificationService: private mbed::NonCopyable<AlertNotificationService>, public CGattService{
    public:
        AlertNotificationService(BLE& ble, events::EventQueue &event_queue):
        _event(BUTTON1),
        CGattService(ble, event_queue, userDescription,
                    _characteristics, _descriptionContent,
                    _propertiesList, uuid, LED3)
    {
        std::cout << "ANS start" << std::endl;
        _event.fall(Callback<void()> (this, &AlertNotificationService::button_P_str));
    }

        ~AlertNotificationService();
        virtual void onDataWritten(const GattWriteCallbackParams &params){
            if(params.handle == _characteristics[4]->getValueHandle()){
                 if (params.data[0] % 2 == 1){
                    std::cout << "EVEN value; turning off" << std::endl;
                    _alert_switch = true;
                }
                else
                {
                    std::cout << "ODD value; turning on" << std::endl;
                    _alert_switch = false;
                }
            }
        }
        virtual void onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params){
            std::cout << "Updates are enabled. Starting writing update_button_num values.." << std::endl;
            //_kernel_event_id = _event_queue.call_every(1000ms, update_button_num, (CCharacteristic<uint32_t>*) _characteristics[2]);

        }
        virtual void onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params){
            std::cout << "Updates are disabled. Stopping writing Kernel clock values.." << std::endl;
            //_event_queue.cancel(_kernel_event_id);
        };

        virtual void onDataRead(const GattReadCallbackParams & 	params){
            if (params.handle==_characteristics[4]->getValueHandle()){
                _button_count = 0;
            }
        }
        void update_button_num(CCharacteristic<uint8_t>* gatt_characteristic){
            printf("button num: %d\r\n", _button_count);
            if(gatt_characteristic->getValueHandle()==_characteristics[2]->getValueHandle())
            {
                gatt_characteristic->set(BLE::Instance().gattServer(), _button_count);
            }
            if (gatt_characteristic->getValueHandle()==_characteristics[3]->getValueHandle()) {
                uint32_t write_val = (uint32_t)"SIMPLE ALERT" & 0xFFFFFFFF;
                gatt_characteristic->set(BLE::Instance().gattServer(), write_val);
            }
        }

        void button_P_str(void){
            _button_count++;
            if(_alert_switch){
                _event_queue.call(&AlertNotificationService:: update_button_num,(CCharacteristic<uint8_t>*) _characteristics[2]));// SIMPLE ALERT
                _event_queue.call(&AlertNotificationService:: update_button_num,(CCharacteristic<uint8_t>*) _characteristics[3]));//button press
            }
        }   


    private:
        //GattAttribute attr;
        // declare a value of 2 bytes within a 10 bytes buffer
        bool _alert_switch = true;
        InterruptIn _event;
        uint8_t attribute_value[10] = {};
        GattAttribute* userDescription[5];
        GattCharacteristic* _characteristics[5];
        const char* _descriptionContent[5] = {  
            ANSCHARACTERISTIC_DESCRIPTION_1,
            ANSCHARACTERISTIC_DESCRIPTION_2,
            ANSCHARACTERISTIC_DESCRIPTION_3,
            ANSCHARACTERISTIC_DESCRIPTION_4,
            ANSCHARACTERISTIC_DESCRIPTION_5
        };
        const uint8_t _propertiesList[5] = {
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE
                        };
        UUID::LongUUIDBytes_t uuid[6];
        static uint8_t _button_count;
    
};
