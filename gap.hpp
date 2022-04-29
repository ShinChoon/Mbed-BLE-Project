

#include "mbed.h"
#include <iostream>
#include <stdio.h>
#include <events/mbed_events.h>
#include <stdio.h>
#include <chrono>
#include <ctime> 

#include "ble/gatt/GattAttribute.h"
#include <cstdint>
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble_utils.h"

// Blinking rate in milliseconds
#define BLINKING_RATE     500ms
class CGAP :private mbed::NonCopyable<CGAP>, public ble::Gap::EventHandler{
    public:
        CGAP(BLE& ble, events::EventQueue &event_queue, const std::string & name):
            _ble(ble), _led(LED1,0), _event(BUTTON1),
            _event_queue(event_queue),
            _adv_data_builder(_adv_buffer),
            _service_uuid(0x0BA0), 
            Device_name(name)
            {}

        ~CGAP()
        {
            if (_ble.hasInitialized()) {
                _ble.shutdown();
            }
        }

        virtual void run();
        virtual void Setter(mbed::Callback<void(void)>&);
        virtual void Setter2(mbed::Callback<void(void)>&);
        
        virtual void OnBleStackInit(BLE::InitializationCompleteCallbackContext *context);


        
    protected:
        DigitalOut _led;
        InterruptIn _event;
        ble::BLE& _ble;
        events::EventQueue& _event_queue;
        uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
        ble::AdvertisingDataBuilder _adv_data_builder;
        bool switchRun = false;
        const UUID  _service_uuid;
        uint8_t _button_count = 0;

        int blink_id = 0;
        std::string Device_name;

    protected:
        mbed::Callback<void(void)> _on_ble_init_callback = nullptr;
        mbed::Callback<void(void)> _on_ble_init_callback2 = nullptr;
        void start_advertising();
        void update_adv();

    private:
        void ledBlinking(){_led = !_led;}
        virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&);
        virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &);
        virtual void onAdvertisingStart(const ble::AdvertisingStartEvent &);
        virtual void onAdvertisingEnd(const ble::AdvertisingEndEvent &);        

    
};