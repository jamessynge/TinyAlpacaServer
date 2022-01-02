# Tests of the device type adapters in src/device_types/*/
# The focus is on the generic dispatching of requests for a
# specific Alpaca device type (i.e. Switch or Camera), but
# without specific hardware.

cc_test(
    name = "observing_conditions_adapter_test",
    srcs = ["observing_conditions_adapter_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:decode_and_dispatch_test_base",
        "//TinyAlpacaServer/extras/test_tools:mock_device_interface",
        "//TinyAlpacaServer/extras/test_tools:mock_observing_conditions",
        "//TinyAlpacaServer/extras/test_tools:test_tiny_alpaca_server",
        "//TinyAlpacaServer/src:alpaca_devices",
        "//TinyAlpacaServer/src:alpaca_discovery_server",
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_info",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src:request_listener",
        "//TinyAlpacaServer/src:server_connection",
        "//TinyAlpacaServer/src:server_description",
        "//TinyAlpacaServer/src:server_sockets_and_connections",
        "//TinyAlpacaServer/src:tiny_alpaca_server",
        "//TinyAlpacaServer/src/device_types:device_impl_base",
        "//TinyAlpacaServer/src/device_types/observing_conditions:observing_conditions_adapter",
        "//absl/strings",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:http_request",
        "//mcucore/extras/test_tools:http_response",
        "//mcucore/extras/test_tools:json_decoder",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:sample_printable",
        "//mcucore/src:array_view",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:progmem_string",
        "//mcucore/src:progmem_string_data",
        "//mcucore/src:status",
        "//mcucore/src:string_view",
        "//mcunet/src:platform_ethernet",
    ],
)

cc_test(
    name = "switch_adapter_test",
    srcs = ["switch_adapter_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:mock_switch_group",
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_info",
        "//TinyAlpacaServer/src/device_types/switch:switch_adapter",
        "//absl/strings",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/src:progmem_string_data",
        "//mcucore/src:status",
        "//mcucore/src:status_or",
    ],
)
