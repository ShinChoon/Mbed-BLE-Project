#include "gap.hpp"



void CGAP::run(){
            if (_ble.hasInitialized()) {
            printf("Ble instance already initialised.\r\n");
            return;
            }
            _ble.gap().setEventHandler(this);
            _ble.init(this, &CGAP::OnBleStackInit);

            _event_queue.dispatch_forever();
        }

    
void CGAP::Setter(mbed::Callback<void(void)>& exFunction) {
        _on_ble_init_callback = exFunction;
        }

void CGAP::Setter2(mbed::Callback<void(void)>& exFunction) {
        _on_ble_init_callback2 = exFunction;
        }

void CGAP::OnBleStackInit(BLE::InitializationCompleteCallbackContext *context){
            BLE& ble_interface = context->ble;
            ble_error_t initialization_error = context->error;
        if (initialization_error != BLE_ERROR_NONE) {
            std::cout <<  "Ble initialization failed." << std::endl;
            return;
        }


        ble::own_address_type_t address_type;
        ble::address_t address;
        _ble.gap().getAddress(address_type, address);
        ble_utils::printDeviceAddress(address);
        if (_on_ble_init_callback!=nullptr)
            _on_ble_init_callback();
        if (_on_ble_init_callback2!=nullptr)
            _on_ble_init_callback2();            
        start_advertising();

}
void CGAP::start_advertising(){
    // Create advertising parameters and payload
    ble::AdvertisingParameters advParameters(
        ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
        ble::adv_interval_t(ble::millisecond_t(100)));


    _ble.gap().setAdvertisingScanResponse(
        ble::LEGACY_ADVERTISING_HANDLE,
        _adv_data_builder.getAdvertisingData()
    );

    //set advData
    _adv_data_builder.clear();
    _adv_data_builder.setFlags();
    _adv_data_builder.setLocalServiceList(mbed::make_Span(&_service_uuid, 1));
    _adv_data_builder.setName(Device_name.c_str());

    update_adv();
    
     ble_error_t error = _ble.gap().setAdvertisingParameters(
        ble::LEGACY_ADVERTISING_HANDLE,
        advParameters
    );
    std::cout << "start" << std::endl;


    if(error!=0){
        std::cout << ble::BLE::errorToString(error) << std::endl;
        return;
    }
    
    error = _ble.gap().setAdvertisingPayload(
    ble::LEGACY_ADVERTISING_HANDLE,
    _adv_data_builder.getAdvertisingData()
    );

    if(error != 0U){
        std::cout << ble::BLE::errorToString(error) << std::endl;
        return;
    }

    error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

    if(error != 0U){
        std::cout << ble::BLE::errorToString(error) << std::endl;
        return;
    }
}

void CGAP::update_adv(){
    ble_error_t error;
    error = _adv_data_builder.setServiceData(
        _service_uuid,
        mbed::make_Span(&_button_count, 1)
    );

    if (error != BLE_ERROR_NONE) {
        std::cout << error << "Updating service failed";
    }

}

void CGAP::onDisconnectionComplete(const ble::DisconnectionCompleteEvent&) {
    _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

}  


void CGAP::onConnectionComplete(const ble::ConnectionCompleteEvent &) {
    std::cout << "connected adv" << std::endl;
    _event_queue.cancel(blink_id);
    _led.write(0);

}


void CGAP::onAdvertisingStart(const ble::AdvertisingStartEvent &event){
    blink_id = _event_queue.call_every(BLINKING_RATE, this, &CGAP::ledBlinking);
    std::cout << "restart adv" << std::endl;
}


void CGAP::onAdvertisingEnd(const ble::AdvertisingEndEvent &){
    _event_queue.cancel(blink_id);
    _led.write(0);
}   
