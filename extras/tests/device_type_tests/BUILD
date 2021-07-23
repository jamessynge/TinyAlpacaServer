# Tests of the device type adapters in src/device_types/*/
# The focus is on the generic dispatching of requests for a
# specific Alpaca device type (i.e. Switch or Camera), but
# without specific hardware.

cc_test(
    name = "observing_conditions_adapter_test",
    srcs = ["observing_conditions_adapter_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:http_request",
        "//extras/test_tools:http_response",
        "//extras/test_tools:json_decoder",
        "//extras/test_tools:mock_device_interface",
        "//extras/test_tools:mock_switch_group",
        "//extras/test_tools:print_to_std_string",
        "//extras/test_tools:sample_printable",
        "//extras/test_tools:test_tiny_alpaca_server",
        "//googletest:gunit_main",
        "//src:alpaca_devices",
        "//src:alpaca_discovery_server",
        "//src:alpaca_response",
        "//src:constants",
        "//src:device_info",
        "//src:device_interface",
        "//src:literals",
        "//src:request_listener",
        "//src:server_connection",
        "//src:server_description",
        "//src:server_sockets_and_connections",
        "//src:tiny_alpaca_server",
        "//src/device_types:device_impl_base",
        "//src/device_types/observing_conditions:observing_conditions_adapter",
        "//src/utils:array_view",
        "//src/utils:platform",
        "//src/utils:platform_ethernet",
        "//src/utils:status",
        "//src/utils:string_view",
    ],
)

cc_test(
    name = "switch_adapter_test",
    srcs = ["switch_adapter_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:mock_switch_group",
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src:alpaca_request",
        "//src:constants",
        "//src:device_info",
        "//src/device_types/switch:switch_adapter",
        "//src/utils:inline_literal",
        "//src/utils:literal",
        "//src/utils:status",
        "//src/utils:status_or",
    ],
)
