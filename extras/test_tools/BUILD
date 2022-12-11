# Libraries intended to simplify the task of writing tests of Tiny Alpaca Server.

package(
    # In addition to testonly, I'd also like to have a "hostonly" feature on a
    # target, so that I could readily mark those features that can be used on
    # embedded systems, and those that are only available on host systems. There
    # probably is some notion of "platform" that I can use. TBD.
    default_testonly = 1,
)

cc_library(
    name = "alpaca_response_validator",
    srcs = ["alpaca_response_validator.cc"],
    hdrs = ["alpaca_response_validator.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//absl/status",
        "//absl/status:statusor",
        "//absl/strings",
        "//mcucore/extras/test_tools:http_request",
        "//mcucore/extras/test_tools:http_response",
        "//mcucore/extras/test_tools:json_decoder",
        "//mcucore/src/log",
        "//util/task:status",
    ],
)

cc_test(
    name = "alpaca_response_validator_test",
    srcs = ["alpaca_response_validator_test.cc"],
    deps = [
        ":alpaca_response_validator",
        "//TinyAlpacaServer/src:alpaca_request",
        "//absl/status",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:http_request",
        "//mcucore/extras/test_tools:http_response",
    ],
)

cc_library(
    name = "decode_and_dispatch_test_base",
    testonly = True,
    srcs = ["decode_and_dispatch_test_base.cc"],
    hdrs = ["decode_and_dispatch_test_base.h"],
    deps = [
        ":alpaca_response_validator",
        ":test_tiny_alpaca_server",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src:server_context",
        "//TinyAlpacaServer/src:server_description",
        "//absl/status",
        "//absl/strings",
        "//googletest:gunit_headers",
        "//mcucore/extras/test_tools:http_request",
        "//mcucore/extras/test_tools:json_decoder",
        "//mcucore/extras/test_tools:status_test_utils",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/container:array_view",
        "//mcucore/src/eeprom:eeprom_tlv",
        "//mcucore/src/log",
        "//mcunet/extras/test_tools:mock_platform_network",
        "//mcunet/src:platform_network",
        "//util/task:status",
    ],
)

cc_library(
    name = "minimal_device",
    hdrs = ["minimal_device.h"],
    deps = [
        "//TinyAlpacaServer/src:device_description",
        "//TinyAlpacaServer/src:server_context",
        "//TinyAlpacaServer/src/device_types:device_impl_base",
    ],
)

cc_library(
    name = "mock_device_interface",
    hdrs = ["mock_device_interface.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_description",
        "//TinyAlpacaServer/src:device_interface",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "mock_observing_conditions",
    hdrs = ["mock_observing_conditions.h"],
    deps = [
        "//TinyAlpacaServer/src/device_types/observing_conditions:observing_conditions_adapter",
        "//googletest:gunit_headers",
        "//mcucore/src/status",
        "//mcucore/src/status:status_or",
    ],
)

cc_library(
    name = "mock_request_decoder_listener",
    hdrs = ["mock_request_decoder_listener.h"],
    deps = [
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:request_decoder_listener",
        "//googletest:gunit_headers",
        "//mcucore/src/strings:string_view",
    ],
)

cc_library(
    name = "mock_request_listener",
    hdrs = ["mock_request_listener.h"],
    deps = [
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:request_listener",
        "//googletest:gunit_headers",
    ],
)

cc_library(
    name = "mock_switch_group",
    hdrs = ["mock_switch_group.h"],
    deps = [
        "//TinyAlpacaServer/src/device_types/switch:switch_adapter",
        "//googletest:gunit_headers",
        "//mcucore/src/status",
        "//mcucore/src/status:status_or",
    ],
)

cc_library(
    name = "test_tiny_alpaca_server",
    testonly = True,
    srcs = ["test_tiny_alpaca_server.cc"],
    hdrs = ["test_tiny_alpaca_server.h"],
    deps = [
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:server_connection",
        "//TinyAlpacaServer/src:server_context",
        "//TinyAlpacaServer/src:server_description",
        "//TinyAlpacaServer/src:tiny_alpaca_device_server",
        "//absl/strings",
        "//mcucore/src/container:array_view",
        "//mcucore/src/log",
        "//mcucore/src/print:o_print_stream",
        "//mcucore/src/strings:progmem_string_data",
        "//mcunet/extras/host/ethernet5500:ethernet_config",
        "//mcunet/extras/test_tools:string_io_stream_impl",
    ],
)
