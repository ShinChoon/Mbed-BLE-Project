#include "gatt_server.hpp"
#include <array>
#include <cstdint>


CGattService::CGattService(BLE& ble, 
                            events::EventQueue &event_queue)
                            :_ble(ble), 
                            _event_queue(event_queue)
{
//decide the length for user description, adding uuid and characteristic
    treesize = ARRAY_SIZE(userDescription);
    for(int i=0; i<treesize; i++){
            userDescription[i] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
            (uint8_t *) _descriptionContent[i],
            sizeof(_descriptionContent[i]),
            sizeof(_descriptionContent[i]),
            false);
    }

     for (int k = 0; k < treesize; k++) {
        for (int i = 0; i < 16; i++) {
            _uuid[k][i] = rand() % 256;
        }
    }
    
    uint8_t value = 0;
    for (int i=0;i<treesize; i++){
        _characteristics[i] = new CCharacteristic<uint8_t>(_uuid[i],
                                _propertiesList[i], 
                                  value, userDescription+i, 1);
    }

    for(int i= 0; i<treesize; i++){
    // configure read security requirements
    _characteristics[i]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    // configure write security requirements
    _characteristics[i]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    // configure update security requirements
    _characteristics[i]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    }

    _service = new GattService(_uuid[treesize], _characteristics, treesize);

}

CGattService::CGattService(BLE& ble, 
                    events::EventQueue &event_queue,
                    GattAttribute* userdes[3], GattCharacteristic* chara[3], const char* desc[3],
                    const uint8_t properties[3], UUID::LongUUIDBytes_t uuid[4])
                    :_ble(ble), 
                     _event_queue(event_queue)
{
//decide the length for user description, adding uuid and characteristic
    treesize = ARRAY_SIZE(desc);
    for(int i=0; i<treesize; i++){
            userdes[i] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
            (uint8_t *) desc[i],
            sizeof(desc[i]),
            sizeof(desc[i]),
            false);
    }

     for (int k = 0; k < treesize; k++) {
        for (int i = 0; i < 16; i++) {
            uuid[k][i] = rand() % 256;
        }
    }
    
    uint32_t value0 = 0;
  _characteristics[0] = new CCharacteristic<uint32_t>(uuid[0],
                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
                                  value0, userdes+0, 1);
                                  
    uint8_t value1 = 0;     
  _characteristics[1] = new CCharacteristic<uint8_t>(uuid[1],
                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
                                  value1, userdes+1, 1);

    uint8_t value2 = 0; 
  _characteristics[2] = new CCharacteristic<uint8_t>(uuid[2],
                      GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
                                  value2, userdes+2, 1);


    std::cout << "ANS start" << std::endl;
    printf("treesize: %d\n", treesize);
    for(int i= 0; i<treesize; i++){
    // configure read security requirements
    _characteristics[i]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    // configure write security requirements
    _characteristics[i]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    // configure update security requirements
    _characteristics[i]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    }

    _service = new GattService(uuid[treesize], _characteristics, treesize);
    

};

CGattService::CGattService(BLE& ble, events::EventQueue &event_queue, 
                    GattAttribute* userdes[1], GattCharacteristic* chara[1], const char* desc[1],
                    const uint8_t properties[1],UUID::LongUUIDBytes_t uuid[2], int size)
                    :_ble(ble), 
                     _event_queue(event_queue)
                     {
    //decide the length for user description, adding uuid and characteristic
    treesize = ARRAY_SIZE(userdes);
    for(int i=0; i<treesize; i++){
            userdes[i] = new GattAttribute(BLE_UUID_DESCRIPTOR_CHAR_USER_DESC,
            (uint8_t *) desc[i],
            sizeof(desc[i]),
            sizeof(desc[i]),
            false);
    }
    set_random_UUID(uuid);
    
    uint8_t value = 0;
    for (int i=0;i<treesize; i++){
        _characteristics[i] = new CCharacteristic<uint8_t>(uuid[i],
                                _propertiesList[i], 
                                  value, userDescription+i, 1);
    }

    for(int i= 0; i<treesize; i++){
    // configure read security requirements
    _characteristics[i]->setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    // configure write security requirements
    _characteristics[i]->setWriteSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    // configure update security requirements
    _characteristics[i]->setUpdateSecurityRequirement(ble::att_security_requirement_t::AUTHENTICATED);
    }

    _service = new GattService(uuid[treesize], _characteristics, treesize);

};





CGattService::~CGattService()
{
    for (int i = 0; i < ARRAY_SIZE(userDescription); i++){
        delete userDescription[i];
    }

        for (int i = 0; i < ARRAY_SIZE(_characteristics); i++){
        delete _characteristics[i];
    }

    delete _service;
}

void CGattService::set_random_UUID(UUID::LongUUIDBytes_t* uuid){ // todo randomize timer
    for (int k = 0; k < treesize; k++) {
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
    //_led_n.write(1);
}

void CGattService::kernel_clock_update(CCharacteristic<uint32_t>* gatt_characteristic) {
    uint32_t write_val = Kernel::Clock::now().time_since_epoch().count() & 0xFFFFFFFF;
    gatt_characteristic->set(BLE::Instance().gattServer(), write_val);
}


void CGattService::onDataWritten(const GattWriteCallbackParams &params){
    std::cout << "call gatt server" << std::endl;
     if (params.handle == _characteristics[0]->getValueHandle()) {
        if (params.data[0] % 2 == 0) {
            std::cout << "EVEN value; turning LED2 on" << std::endl;
            //_led_n.write(0);
        }
        else {
            std::cout << "ODD value; turning LED2 off" << std::endl;
            //_led_n.write(1);
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


    void CGattService::set_long_random_UUID(UUID::ShortUUIDBytes_t* uuid){
        
            for (int k = 0; k < treesize; k++) {
                    uuid[k] = rand() % 32;
                    printf("address: %x\n", uuid[k]);
            }
    }
