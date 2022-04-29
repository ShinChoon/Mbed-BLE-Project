#ifndef BLE_UTILS_H
#define BLE_UTILS_H
#include <iostream>
#include "ble/BLE.h"
namespace ble_utils{
    static void print_address(const ble::address_t &addr){
        printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                       addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    }
    
    static void printError(ble_error_t error, const char *message) {
        std::cout << message << std::endl;
        ble::BLE::errorToString(error);
    }
    
    static void printDeviceAddress(ble::own_address_type_t type,
                           ble::address_t address){
        print_address(address);
    }
    
    static void printDeviceAddress(const ble::peer_address_type_t type,
                                   const ble::address_t address){
        print_address(address);
    }
    
    static void printDeviceAddress(ble::address_t& address){
        print_address(address);
    }
}
#endif