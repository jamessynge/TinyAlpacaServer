# Tests of the device type adapters in src/device_types/*/
# The focus is on the generic dispatching of requests for a
# specific Alpaca device type (i.e. Switch or Camera), but
# without specific hardware.

cc_test(
    name = "observing_conditions_adapter_test",
    srcs = ["observing_conditions_adapter_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:decode_and_dispatch_test_base",
        "//TinyAlpacaServer/extras/test_tools:mock_observing_conditions",
        "//TinyAlpacaServer/extras/test_tools:test_tiny_alpaca_server",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_description",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src/device_types/observing_conditions:observing_conditions_adapter",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:http_request",
        "//mcucore/extras/test_tools:http_response",
        "//mcucore/extras/test_tools:json_decoder",
        "//mcucore/extras/test_tools:uuid_utils",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:progmem_string",
        "//mcucore/src:progmem_string_data",
        "//mcunet/src:platform_network",
    ],
)

cc_test(
    name = "switch_adapter_test",
    srcs = ["switch_adapter_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:decode_and_dispatch_test_base",
        "//TinyAlpacaServer/extras/test_tools:mock_switch_group",
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_description",
        "//TinyAlpacaServer/src/device_types/switch:switch_adapter",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/src:progmem_string_data",
        "//mcucore/src:status",
        "//mcucore/src:status_code",
        "//mcucore/src:status_or",
    ],
)
