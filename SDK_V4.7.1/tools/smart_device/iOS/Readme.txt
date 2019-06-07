BtNotify

=================================================================================
Introduction:
    Base on SDK4.2, change lists:
    1. From libMtkBlemanager.a to libBtNotify.a
    2. Public BLE part (scan, connect, disconnect, discovery service/characteristics...)
    3. Encapsulation the DOGP profile as BtNotify transfer library

    BtNotify.h
        Header file which describe the library public APIs and delegates.
        Please inclue the header file into your project.

    libBtNotify.a
        BtNotify library, please import the library inotto your project.
    
    Mediatek SmartDevice
        Demo project which used the libBtNotify.a.
        Including the scan, connect, discovery service/characteristics, call libBtNotify library.
        Please focus the bluetooth folder.
    
    BtNotify_v1.0_document.rar
        The release documents for BtNotify and 2511 health support.
        
        BtNotify_v1.0.pdf
            Introduce the BtNotify library APIs and how to use the library (call sequence).
        
        MT2511 Health Support.pdf
            If the device support health feature, iOS App can use the BtNotify library to communicate with remote device.
            The Health commands is defined in the Support pdf.
        
=================================================================================

=================================================================================

Release Date : 2017/05/02
Initial version of the libBtNotify.

=================================================================================

=================================================================================

Release Date : 2017/05/24
Fix build error, if UI application add -ObjC into the Other linker flags.

=================================================================================

=================================================================================

Release Date : 2017/05/31
Fix customer data cannot received issue, if the customer data contains the special character.

=================================================================================

=================================================================================

Release Date : 2017/06/15
Fix issue : while connected, if device send data firstly, the lib cannot handle the data
            check receiver in the ControllerManager, if yes, handle internal, if not callback to delegate

=================================================================================

=================================================================================

Release Date : 2017/06/27
Fix issue : while loop the delegate list to callback, the application delete the delegate, crash happen

=================================================================================

=================================================================================

Release Date : 2017/08/08
Fix issue : Add GNSS Fota Support

=================================================================================
