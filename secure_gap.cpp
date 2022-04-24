#include "secure_gap.hpp"


    /**      
      * \brief Called when a pairing request is received. Application should respond by      *        calling the appropriate function: acceptPairingRequest() or cancelPairingRequest().      *        This event will only trigger if setPairingRequestAuthorisation() was called with true.      *        Otherwise the stack will handle the requests.      *
      * \param[in] connectionHandle connection handle
      *
      */
    void CSecureGap::pairingRequest(ble::connection_handle_t connectionHandle){
        std::cout << "Pairing requested - authorising" << std::endl;
        _ble.securityManager().acceptPairingRequest(connectionHandle);
    }

    /**
      * \brief Triggered by change of encryption state on a link. This change can be initiated
      * locally or by the remote peer.
      *
      * \param connectionHandle connection handle
      * \param result encryption state of the link
      */
    void CSecureGap::linkEncryptionResult(ble::connection_handle_t connectionHandle,
                      ble::link_encryption_t result){
        if(result == ble::link_encryption_t::ENCRYPTED) {
          std::cout << "Link ENCRYPTED" << std::endl;
        } else if(result == ble::link_encryption_t::ENCRYPTED_WITH_MITM) {
          std::cout << "Link ENCRYPTED_WITH_MITM" << std::endl;
        } else if(result == ble::link_encryption_t::NOT_ENCRYPTED) {
          std::cout << "Link NOT_ENCRYPTED" << std::endl;
        }
    }

    /**
      * \brief Triggered during pairing based on IO capabilities of devices. Display the given
      *        passkey on the local device for user verification.
      *
      * \param connectionHandle connection handle
      * \param passkey 6 digit passkey to be displayed
      */
    void CSecureGap::passkeyDisplay(ble::connection_handle_t connectionHandle,
                  const SecurityManager::Passkey_t passkey){
        std::cout << "Input passKey: ";
        for(unsigned i = 0; i < ble::SecurityManager::PASSKEY_LEN; i++) {
          printf("%c ", passkey[ble::SecurityManager::PASSKEY_LEN - 1 - i]);
        }
        std::cout << std::endl;
    }

    /**
      * \brief Indicates to the application that a confirmation is required. This is used
      *        when the device does not have a keyboard but has a yes/no button. The device
      *        displays numbers on its display in response to passkeyDisplay and the user
      *        checks if they are the same on both devices. The application should proceed
      *        by supplying the confirmation using the confirmationEntered function.
      *
      * \param connectionHandle connection handle
      */
    void CSecureGap::confirmationRequest(ble::connection_handle_t connectionHandle){
        std::cout << "Conf required!" << std::endl;
    }

    /**
      * \brief Indicates to the application that a passkey is required. The application should
      *        proceed by supplying the passkey through the passkeyEntered function.
      *
      * \param connectionHandle connection handle
      */
    void CSecureGap::passkeyRequest(ble::connection_handle_t connectionHandle){
        std::cout << "passkeyRequest" << std::endl;
    }

    /**
      * \brief Notifies the application that a key was pressed by the peer during passkey entry.
      *        This is only informative to provide feedback to the user. These events will only
      *        be triggered if you call setKeypressNotification()
      *
      * \param connectionHandle connection handle
      * \param keypress type of keypress event
      */
    void CSecureGap::keypressNotification(ble::connection_handle_t connectionHandle,
                      SecurityManager::Keypress_t keypress){
        std::cout << "keypressNotification" << std::endl;
    }

    /**
      * \brief Delivers the signing key to the application.
      *
      * \param connectionHandle connection handle
      * \param csrk signing key, pointer only valid during call
      * \param authenticated indicates if the signing key is authenticated
      */
    void CSecureGap::signingKey(ble::connection_handle_t connectionHandle,
                const ble::csrk_t* csrk,
                bool authenticated){
        std::cout << "signingKey" << std::endl;
    }

    /**
      * \brief Indicates to the application that pairing has completed.
      *
      * \param connectionHandle connection handle
      * \param result result of the pairing indicating success or reason for failure
      */
    void CSecureGap::pairingResult(ble::connection_handle_t connectionHandle,
                  SecurityManager::SecurityCompletionStatus_t result) {
        printf("Security status 0x%02x\r\n", result);
        if(result == SecurityManager::SEC_STATUS_SUCCESS) {
          std::cout << "Security success" << std::endl;
        } else {
          std::cout << "Security failed" << std::endl;
        }
    }



    void CSecureGap::OnBleStackInit(BLE::InitializationCompleteCallbackContext *context){
        ble_error_t error;
        if(context->error) {
            std::cout << "Error during the initialisation" << std::endl;
            return;
        }
        // initialize security manager
        error =
           _ble.securityManager().init(false /* Enable bonding*/,
                                                      true /*Require MITM protection*/,
                                                      SecurityManager::IO_CAPS_DISPLAY_ONLY/*IO capabilities*/,
                                                      NULL /*Passkey*/,
                                                      false /*Support data signing*/);
        printError(error, "_ble.securityManager().init() ");
        if(error != BLE_ERROR_NONE) {
          return;
        }
        // enable legacy pairing
        _ble.securityManager().allowLegacyPairing(true);
        // set the event handler to this object
        _ble.securityManager().setSecurityManagerEventHandler(this);
        //
        _ble.securityManager().setPairingRequestAuthorisation(true);

        // start advertisement using CGAP instance of this class


        ble_error_t initialization_error = context->error;
        if (initialization_error != BLE_ERROR_NONE) {
            std::cout <<  "Ble initialization failed." << std::endl;
            return;
        }
        ble::own_address_type_t address_type;
        ble::address_t address;
        _ble.gap().getAddress(address_type, address);
        print_address(address);
        if (_on_ble_init_callback!=nullptr)
            _on_ble_init_callback();
        if (_on_ble_init_callback2!=nullptr)
            _on_ble_init_callback2();
        start_advertising();


        // Enable privacy
        error =  _ble.gap().enablePrivacy(true);

        printError(error, "_ble.gap().enablePrivacy() ");
        // Configure privacy settings
        privacyConfiguration.use_non_resolvable_random_address = false;
        privacyConfiguration.resolution_strategy = ble::peripheral_privacy_configuration_t::REJECT_NON_RESOLVED_ADDRESS;
         _ble.gap().setPeripheralPrivacyConfiguration(&privacyConfiguration);
    }


    void CSecureGap::onConnectionComplete(const ble::ConnectionCompleteEvent & event){
    printError(event.getStatus(), "onConnectionComplete() ");
    // get the connection handle
    ble::connection_handle_t handle = event.getConnectionHandle();
    /* Request a change in link security. This will be done
     * indirectly by asking the master of the connection to
     * change it. Depending on circumstances different actions
     * may be taken by the master which will trigger events
     * which the applications should deal with.
     */
    ble_error_t error = ble::BLE::Instance().
                          securityManager().
                          setLinkSecurity(handle,
                                          SecurityManager::SECURITY_MODE_ENCRYPTION_WITH_MITM);

    printError(error, "_ble.securityManager().setLinkSecurity() ");
    }



    void CSecureGap::run(){
            if (_ble.hasInitialized()) {
            printf("Ble instance already initialised.\r\n");
            return;
            }
            _ble.gap().setEventHandler(this);
            _ble.init(this, &CSecureGap::OnBleStackInit);
            printf("CSecureGap instance already initialised.\r\n");
            _event_queue.dispatch_forever();
        }


    //void CSecureGap::Setter(mbed::Callback<void ()> & exFunction){
    //    _on_ble_init_callback = exFunction;
    //}
//
//
//
    //void CSecureGap::Setter2(mbed::Callback<void ()> & exFunction){
    //    _on_ble_init_callback2 = exFunction;
    //}