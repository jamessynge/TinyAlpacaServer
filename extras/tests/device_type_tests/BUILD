# Tests of the device type adapters in src/device_types/*/
# The focus is on the generic dispatching of requests for a
# specific Alpaca device type (i.e. Switch or Camera), but
# without specific hardware.

cc_test(
    name = "switch_adapter_test",
    srcs = ["switch_adapter_test.cc"],
    deps = [
        "//absl/strings",
        "//extras/test_tools:mock_device_interface",
        "//extras/test_tools:mock_switch_group",
        "//extras/test_tools:print_to_std_string",
        "//googletest:gunit_main",
        "//src:constants",
        "//src/device_types/switch:switch_adapter",
        "//src/utils:status",
    ],
)
