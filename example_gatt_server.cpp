#include "example_gatt_server.hpp"


CGattService::CGattService(BLE& ble, events::EventQueue &event_queue):_ble(ble), _event_queue(event_queue), _led_2(LED2) {
    uint8_t value1 = 1;
    uint16_t value2 = 0;
    uint32_t value3 = 0;
    userDescription[0] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
            (uint8_t *) CHARACTERISTIC_DESCRIPTION_1,
            sizeof(CHARACTERISTIC_DESCRIPTION_1),
            sizeof(CHARACTERISTIC_DESCRIPTION_1),
            false);

    userDescription[1] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
            (uint8_t *) CHARACTERISTIC_DESCRIPTION_2,
            sizeof(CHARACTERISTIC_DESCRIPTION_2),
            sizeof(CHARACTERISTIC_DESCRIPTION_2),
            false);

    userDescription[2] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
            (uint8_t *) CHARACTERISTIC_DESCRIPTION_3,
            sizeof(CHARACTERISTIC_DESCRIPTION_3),
            sizeof(CHARACTERISTIC_DESCRIPTION_3),
            false);

    set_random_UUID(uuid);
    _characteristics[0] = new CCharacteristic<uint8_t>(uuid[0],
                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE, 
                                  value1, userDescription, 1);

    _characteristics[1] = new CCharacteristic<uint16_t> (uuid[1],
                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE, 
                                  value2, userDescription+1, 1);

    _characteristics[2] = new CCharacteristic<uint32_t> (uuid[2],
                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE, 
                                  value3, userDescription+2, 1);

    // configure read security requirements
    _characteristics[0]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    // configure write security requirements
    _characteristics[0]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    // configure update security requirements
    _characteristics[0]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);

    // configure read security requirements
    _characteristics[1]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    // configure write security requirements
    _characteristics[1]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    // configure update security requirements
    _characteristics[1]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);

    
    // configure read security requirements
    _characteristics[2]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    // configure write security requirements
    _characteristics[2]->setWriteSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    // configure update security requirements
    _characteristics[2]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    
    _service = new GattService(uuid[3], _characteristics, 3);



}

CGattService::~CGattService()
{
    delete userDescription[0];
    delete userDescription[1];
    delete userDescription[2];
    delete _characteristics[0];
    delete _characteristics[1];
    delete _characteristics[2];
    delete _service;
}


void CGattService::set_random_UUID(UUID::LongUUIDBytes_t* uuid) { // todo randomize timer
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < 16; i++) {
            uuid[k][i] = rand() % 256;
        }
    }
}

void CGattService::run(void){
    std::cout << "CGattService@@@@@@" << std::endl;
    _ble.gattServer().addService(*_service);
    _ble.gattServer().setEventHandler(this);
    return;
}

void CGattService::onUpdatesEnabled (const GattUpdatesEnabledCallbackParams &params) {
    std::cout << "Updates are enabled. Starting writing Kernel clock values.." << std::endl;
    _kernel_event_id = _event_queue.call_every(1000ms, kernel_clock_update, (CCharacteristic<uint32_t>*) _characteristics[2]);

}

void CGattService::onUpdatesDisabled (const GattUpdatesDisabledCallbackParams &params) {
    std::cout << "Updates are disabled. Stopping writing Kernel clock values.." << std::endl;
    _event_queue.cancel(_kernel_event_id);
    _led_2.write(1);
}

void CGattService::kernel_clock_update(CCharacteristic<uint32_t>* gatt_characteristic) {
    uint32_t write_val = Kernel::Clock::now().time_since_epoch().count() & 0xFFFFFFFF;
    gatt_characteristic->set(BLE::Instance().gattServer(), write_val);
}


void CGattService::onDataWritten(const GattWriteCallbackParams &params){
     if (params.handle == _characteristics[0]->getValueHandle()) {
        if (params.data[0] % 2 == 0) {
            std::cout << "EVEN value; turning LED2 on" << std::endl;
            _led_2.write(0);
        }
        else {
            std::cout << "ODD value; turning LED2 off" << std::endl;
            _led_2.write(1);
        }
    }
    if (params.handle == _characteristics[1]->getValueHandle()) {
        printf("Outputting write values to serial port: 0x");
        for (int i = 0; i < params.len; i++) {
            printf("%02x", params.data[i]);
        }
        std::cout << std::endl;
    }
}
