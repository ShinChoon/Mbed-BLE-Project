
#include "mbed.h"
#include <iostream>
#include <stdio.h>
#include <events/mbed_events.h>
#include <stdio.h>
#include <chrono>
#include <ctime> 

#include "ble/gatt/GattAttribute.h"
#include <cstdint>

#include "ble/gap/Types.h"
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "gap.hpp"


class CSecureGap:private mbed::NonCopyable<CSecureGap>, public CGAP, ble::SecurityManager::EventHandler{
    public:
        CSecureGap(BLE& ble, events::EventQueue &event_queue, const std::string & name):
            _ble(ble), _led(LED1,0), _event(BUTTON1),
            _event_queue(event_queue),
            _adv_data_builder(_adv_buffer),
            _service_uuid(0x0BA0), 
            Device_name(name),
            CGAP(ble, event_queue, name)
            {}

     /**
      * \brief Called when a pairing request is received. Application should respond by      *        calling the appropriate function: acceptPairingRequest() or cancelPairingRequest().      *        This event will only trigger if setPairingRequestAuthorisation() was called with true.      *        Otherwise the stack will handle the requests.      *
      * \param[in] connectionHandle connection handle
      *
      */
    virtual void pairingRequest(ble::connection_handle_t connectionHandle) override;
    /**
      * \brief Triggered by change of encryption state on a link. This change can be initiated
      * locally or by the remote peer.
      *
      * \param connectionHandle connection handle
      * \param result encryption state of the link
      */
    virtual void linkEncryptionResult(ble::connection_handle_t connectionHandle,
                      ble::link_encryption_t result) override;
    /**
      * \brief Triggered during pairing based on IO capabilities of devices. Display the given
      *        passkey on the local device for user verification.
      *
      * \param connectionHandle connection handle
      * \param passkey 6 digit passkey to be displayed
      */
    virtual void passkeyDisplay(ble::connection_handle_t connectionHandle,
                  const SecurityManager::Passkey_t passkey) override;

    /**
      * \brief Indicates to the application that a confirmation is required. This is used
      *        when the device does not have a keyboard but has a yes/no button. The device
      *        displays numbers on its display in response to passkeyDisplay and the user
      *        checks if they are the same on both devices. The application should proceed
      *        by supplying the confirmation using the confirmationEntered function.
      *
      * \param connectionHandle connection handle
      */
    virtual void confirmationRequest(ble::connection_handle_t connectionHandle) override;

    /**
      * \brief Indicates to the application that a passkey is required. The application should
      *        proceed by supplying the passkey through the passkeyEntered function.
      *
      * \param connectionHandle connection handle
      */
    virtual void passkeyRequest(ble::connection_handle_t connectionHandle) override;

    /**
      * \brief Notifies the application that a key was pressed by the peer during passkey entry.
      *        This is only informative to provide feedback to the user. These events will only
      *        be triggered if you call setKeypressNotification()
      *
      * \param connectionHandle connection handle
      * \param keypress type of keypress event
      */
    virtual void keypressNotification(ble::connection_handle_t connectionHandle,
                      SecurityManager::Keypress_t keypress) override;

    /**
      * \brief Delivers the signing key to the application.
      *
      * \param connectionHandle connection handle
      * \param csrk signing key, pointer only valid during call
      * \param authenticated indicates if the signing key is authenticated
      */
    virtual void signingKey(ble::connection_handle_t connectionHandle,
                const ble::csrk_t* csrk,
                bool authenticated) override;

    /**
      * \brief Indicates to the application that pairing has completed.
      *
      * \param connectionHandle connection handle
      * \param result result of the pairing indicating success or reason for failure
      */
    virtual void pairingResult(ble::connection_handle_t connectionHandle,
                  SecurityManager::SecurityCompletionStatus_t result) override;
        
    virtual void OnBleStackInit(BLE::InitializationCompleteCallbackContext *context) override;

    virtual void run()override;
    virtual void Setter(mbed::Callback<void(void)>&)override;

    private:
        ble::BLE& _ble;
        events::EventQueue& _event_queue;
        std::string Device_name;
        DigitalOut _led;
        InterruptIn _event;
        uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE]{};
        ble::AdvertisingDataBuilder _adv_data_builder;
        bool switchRun = false;
        const UUID  _service_uuid;
        int blink_id = 0;
        ble::peripheral_privacy_configuration_t privacyConfiguration;

    private:
            virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &) override;
};