Bluetooth Mesh module usage guide

Brief:           This module is the implementation of the Bluetooth Mesh Profile Specification and Mesh Model Specification. It supports 
                 Mesh Relay features, Proxy features, Friend features, Mesh security, 
                 and Mesh beacons.
Usage:           GCC: Include the following items in your GCC project Makefile.
                      1. Include the header path.
                         ifeq ($(MTK_BT_MESH_ENABLE),y)
                              CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/mesh/module.mk
                         endif
                      2. Include the module for compiling.
                         ifeq ($(MTK_BT_MESH_ENABLE),y)
                              CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/mesh/module.mk
                         endif
                      3. Add MTK_BT_MESH_ENABLE = y in /GCC/feature.mk
Dependency:      This module has the dependency with Bluetooth Low Energy. Please make sure to also include Bluetooth.
                 MTK_BT_ENABLE must be set to 'y' and MTK_BLE_ONLY_ENABLE must be set to 'y' if 
                 you only support Bluetooth Low-Energy (BLE) in your project.
Notice:          N/A.
Relative doc:    N/A.
Example project: Please find the project under [sdk_root]/project/mt7697_hdk/apps/ble_mesh_switch and ble_mesh_vendor_device.


