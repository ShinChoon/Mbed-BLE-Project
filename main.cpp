/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <iostream>
#include <stdio.h>
#include <events/mbed_events.h>
#include <stdio.h>
#include <chrono>
#include <ctime> 

#include "ble/gatt/GattAttribute.h"
#include <cstdint>

#include "ble/GattServer.h"
#include "secure_gap.hpp"
#include "ble_gatt_alert_notification_service.hpp"
#include "ble_gatt_immediate_alert_service.hpp"
#include <cstddef>

static EventQueue event_queue(4* EVENTS_EVENT_SIZE);


void schedule_ble_processing(BLE::OnEventsToProcessCallbackContext* context){
    event_queue.call(callback(&(context->ble), &BLE::processEvents));
}





int main()
{
    BLE &ble_interface = BLE::Instance();
    ble_interface.onEventsToProcess(schedule_ble_processing);
    CGAP demo(ble_interface, event_queue, "GATTServer Secure");
    //CGattService demoservice(ble_interface, event_queue);
    AlertNotificationService AlertDemo(ble_interface, event_queue);
    ImmediateAlertService  IASDemo(ble_interface, event_queue);
    mbed::Callback<void()> cb_func,cb_func2;
    cb_func = callback(&AlertDemo, &AlertNotificationService::run);
    cb_func2 = callback(&IASDemo, &ImmediateAlertService::run);
    demo.Setter(cb_func);
    demo.Setter2(cb_func2);
    demo.run();
    return 0;
}
