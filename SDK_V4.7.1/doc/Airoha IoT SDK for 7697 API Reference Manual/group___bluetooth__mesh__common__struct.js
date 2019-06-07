var group___bluetooth__mesh__common__struct =
[
    [ "bt_mesh_evt_prov_capabilities_t", "structbt__mesh__evt__prov__capabilities__t.html", [
      [ "cap", "structbt__mesh__evt__prov__capabilities__t.html#a477e561bd082acd61ae9cac088feff86", null ]
    ] ],
    [ "bt_mesh_evt_prov_request_auth_t", "structbt__mesh__evt__prov__request__auth__t.html", [
      [ "action", "structbt__mesh__evt__prov__request__auth__t.html#aa90f0d42f4f210203fb37502590a7bdb", null ],
      [ "method", "structbt__mesh__evt__prov__request__auth__t.html#a7e5575b2266da23e0d9a5dff2da9881a", null ],
      [ "size", "structbt__mesh__evt__prov__request__auth__t.html#ae5dc6ffcd9b7605c7787791e40cc6bb0", null ]
    ] ],
    [ "bt_mesh_evt_prov_show_pk_t", "structbt__mesh__evt__prov__show__pk__t.html", [
      [ "pk", "structbt__mesh__evt__prov__show__pk__t.html#a3de9f5a9164251dac8b92161fc0c13de", null ]
    ] ],
    [ "bt_mesh_evt_prov_show_auth_t", "structbt__mesh__evt__prov__show__auth__t.html", [
      [ "auth", "structbt__mesh__evt__prov__show__auth__t.html#a2e702f6976ce1aef3a4dde14872c2644", null ]
    ] ],
    [ "bt_mesh_evt_prov_done_t", "structbt__mesh__evt__prov__done__t.html", [
      [ "address", "structbt__mesh__evt__prov__done__t.html#a643d30df67a67c5915fde941c934f9f8", null ],
      [ "device_key", "structbt__mesh__evt__prov__done__t.html#adf0510d6ba4d63285526ce7a8b80abf5", null ]
    ] ],
    [ "bt_mesh_evt_prov_scan_ud_t", "structbt__mesh__evt__prov__scan__ud__t.html", [
      [ "device", "structbt__mesh__evt__prov__scan__ud__t.html#ae1eb3a98533831898bbcb747ae7c0635", null ]
    ] ],
    [ "bt_mesh_evt_bearer_gatt_status_t", "structbt__mesh__evt__bearer__gatt__status__t.html", [
      [ "handle", "structbt__mesh__evt__bearer__gatt__status__t.html#aa8bec03983fc2371ab6afa98ae6ddd45", null ],
      [ "status", "structbt__mesh__evt__bearer__gatt__status__t.html#a571ca6962b1e6a291c5baacecb137e19", null ]
    ] ],
    [ "bt_mesh_evt_friendship_status_t", "structbt__mesh__evt__friendship__status__t.html", [
      [ "address", "structbt__mesh__evt__friendship__status__t.html#a643d30df67a67c5915fde941c934f9f8", null ],
      [ "status", "structbt__mesh__evt__friendship__status__t.html#a920e9f68ded990b9bf7a5f8c578e3652", null ]
    ] ],
    [ "bt_mesh_evt_lpn_friend_offer_t", "structbt__mesh__evt__lpn__friend__offer__t.html", [
      [ "address", "structbt__mesh__evt__lpn__friend__offer__t.html#a643d30df67a67c5915fde941c934f9f8", null ],
      [ "friend_counter", "structbt__mesh__evt__lpn__friend__offer__t.html#a7a48c17fb6ab7054e1a6cb01687c8375", null ],
      [ "queue_size", "structbt__mesh__evt__lpn__friend__offer__t.html#aa94108f5f84135a0bca0eaf7277a4e1f", null ],
      [ "receive_window", "structbt__mesh__evt__lpn__friend__offer__t.html#a7961cbc3456e111b97bee1e4cf5c00a3", null ],
      [ "rssi", "structbt__mesh__evt__lpn__friend__offer__t.html#a3b962e67ba74725bd60ca3c29f785abe", null ],
      [ "subscription_list_size", "structbt__mesh__evt__lpn__friend__offer__t.html#a5b59624b11601bff9add79ba4203fadf", null ]
    ] ],
    [ "bt_mesh_evt_lpn_subscription_list_confirm_t", "structbt__mesh__evt__lpn__subscription__list__confirm__t.html", [
      [ "transaction_number", "structbt__mesh__evt__lpn__subscription__list__confirm__t.html#ac217a35935d480cf443646e52f6533be", null ]
    ] ],
    [ "bt_mesh_evt_heartbeat_t", "structbt__mesh__evt__heartbeat__t.html", [
      [ "features", "structbt__mesh__evt__heartbeat__t.html#a05394e2501b222db77c3a39194523065", null ],
      [ "hops", "structbt__mesh__evt__heartbeat__t.html#a2719bad26c6e7de2bc08439cea7111ce", null ],
      [ "init_ttl", "structbt__mesh__evt__heartbeat__t.html#af9e7b0f0cde9d98be89cf976aed12fdd", null ],
      [ "src", "structbt__mesh__evt__heartbeat__t.html#af5eca485cd2b34522478907d4c8f5bc6", null ]
    ] ],
    [ "bt_mesh_evt_iv_update_t", "structbt__mesh__evt__iv__update__t.html", [
      [ "iv_index", "structbt__mesh__evt__iv__update__t.html#ad5321663b5e6e2019fc62ea61bbdbadd", null ],
      [ "state", "structbt__mesh__evt__iv__update__t.html#a17c328c8adca4280ea1adfd906d8d3b9", null ]
    ] ],
    [ "bt_mesh_evt_key_refresh_t", "structbt__mesh__evt__key__refresh__t.html", [
      [ "netkey_index", "structbt__mesh__evt__key__refresh__t.html#a855e9ece7785408c341f0795ae49f7ce", null ],
      [ "phase", "structbt__mesh__evt__key__refresh__t.html#ae8a405c568d90ce0ffa0714043d46d15", null ]
    ] ],
    [ "bt_mesh_evt_t", "structbt__mesh__evt__t.html", [
      [ "bearer_gatt_status", "structbt__mesh__evt__t.html#a4233f230b8af977645042a63901fc53a", null ],
      [ "friendship_status", "structbt__mesh__evt__t.html#ac888f40798b76315dd047a20dbe65e49", null ],
      [ "heartbeat", "structbt__mesh__evt__t.html#a0e86cb6fff43eddb896d5408ecfa66f4", null ],
      [ "iv_update", "structbt__mesh__evt__t.html#ab758a6d89f9ce424a18035e6cb6c29df", null ],
      [ "key_refresh", "structbt__mesh__evt__t.html#a3fe1a8764dff7be3650d95d434b98ecf", null ],
      [ "lpn_friend_offer", "structbt__mesh__evt__t.html#a3eab9142ab5ee1221b474d32ced13597", null ],
      [ "lpn_subscription_list_confirm", "structbt__mesh__evt__t.html#a9fe44d2927a6312f7a084dfb1b9e1526", null ],
      [ "mesh", "structbt__mesh__evt__t.html#a0d72cab4f7fb5466a552072170b157bc", null ],
      [ "prov_cap", "structbt__mesh__evt__t.html#a9612c6a865e3ddeb95ae7fbc08ee8656", null ],
      [ "prov_done", "structbt__mesh__evt__t.html#a4c92ef85db3929b375de9e93685fe0b9", null ],
      [ "prov_request_auth", "structbt__mesh__evt__t.html#ac9bb16de1e44eed34d641c2763ec2b4c", null ],
      [ "prov_scan_ud", "structbt__mesh__evt__t.html#a0d2b33a8ebc25f36c4ff3b317f13dcb9", null ],
      [ "prov_show_auth", "structbt__mesh__evt__t.html#aea425e2ea9bd3156464be872a85bd851", null ],
      [ "prov_show_pk", "structbt__mesh__evt__t.html#a5471f1f0d892e922a6e408055af2bad6", null ]
    ] ],
    [ "bt_mesh_security_t", "structbt__mesh__security__t.html", [
      [ "appidx", "structbt__mesh__security__t.html#a65e95c73485956c39c11fe6f01c342d4", null ],
      [ "device_key", "structbt__mesh__security__t.html#a3c6ceb584fc87cd1835d83f6697b0f82", null ],
      [ "netidx", "structbt__mesh__security__t.html#abe8de0f7c3191522e773998a68b14266", null ]
    ] ],
    [ "bt_mesh_tx_params_t", "structbt__mesh__tx__params__t.html", [
      [ "data", "structbt__mesh__tx__params__t.html#a579f5164b5b16e18ddcdb0296ef81799", null ],
      [ "data_len", "structbt__mesh__tx__params__t.html#ad1a572736a10ff6b282c5f43c4ea1ccf", null ],
      [ "dst", "structbt__mesh__tx__params__t.html#ad358f44667ae97fc0086bc690e83b20a", null ],
      [ "security", "structbt__mesh__tx__params__t.html#ae44a44424b0b9e6bcc3f021e142902b8", null ],
      [ "src", "structbt__mesh__tx__params__t.html#ac87170cf133e48fc4c61ea4b90bfb515", null ],
      [ "ttl", "structbt__mesh__tx__params__t.html#a4d402503271315376ddf4e47674d4965", null ]
    ] ],
    [ "bt_mesh_init_params_t", "structbt__mesh__init__params__t.html", [
      [ "config", "structbt__mesh__init__params__t.html#adde2331c26db44ad195f13648cc0a933", null ],
      [ "friend", "structbt__mesh__init__params__t.html#ad264df3be682fae74223afdba4b2ec73", null ],
      [ "provisionee", "structbt__mesh__init__params__t.html#a8621d72e913fa2fd07f702d85e4362cc", null ],
      [ "role", "structbt__mesh__init__params__t.html#ad77936dc0fda6a67e732262784d656fc", null ]
    ] ],
    [ "bt_mesh_address_t", "structbt__mesh__address__t.html", [
      [ "type", "structbt__mesh__address__t.html#a003c6277cbc86ec7b5ce3b9f95fc45bb", null ],
      [ "value", "structbt__mesh__address__t.html#a900b69af7d674b8db3bceae754b4955a", null ],
      [ "virtual_uuid", "structbt__mesh__address__t.html#a3466f651fec025d63dc19f859acf3c08", null ]
    ] ],
    [ "bt_mesh_security_t", "group___bluetooth__mesh__common__struct.html#ga843171343aee1dff8a066b1b72e85b88", null ]
];