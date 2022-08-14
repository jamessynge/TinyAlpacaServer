# Contains tests of code that is in the src directory, to be executed on host.
# We don't put the tests there because otherwise the Arduino IDE tries to treat
# them as code to be compiled for the target microcontroller.

cc_test(
    name = "alpaca_devices_test",
    srcs = ["alpaca_devices_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:alpaca_response_validator",
        "//TinyAlpacaServer/extras/test_tools:minimal_device",
        "//TinyAlpacaServer/extras/test_tools:mock_device_interface",
        "//TinyAlpacaServer/src:alpaca_devices",
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_description",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:server_context",
        "//glog:logging",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:http_response",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:status_test_utils",
        "//mcucore/extras/test_tools:uuid_utils",
        "//mcucore/src:array_view",
        "//mcucore/src:eeprom_tag",
        "//mcucore/src:o_print_stream",
        "//mcucore/src:progmem_string_data",
    ],
)

cc_test(
    name = "alpaca_response_test",
    srcs = ["alpaca_response_test.cc"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:ascom_error_codes",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:literals",
        "//absl/strings",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:json_test_utils",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:print_value_to_std_string",
        "//mcucore/extras/test_tools:sample_printable",
        "//mcucore/src:any_printable",
        "//mcucore/src:array_view",
        "//mcucore/src:json_encoder",
        "//mcucore/src:status_or",
    ],
)

cc_test(
    name = "device_description_test",
    srcs = ["device_description_test.cc"],
    deps = [
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_description",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:json_decoder",
        "//mcucore/extras/test_tools:json_test_utils",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/extras/test_tools:status_or_test_utils",
        "//mcucore/extras/test_tools:uuid_utils",
        "//mcucore/src:eeprom_tlv",
        "//mcucore/src:json_encoder",
        "//mcucore/src:progmem_string_data",
        "//mcucore/src:uuid",
    ],
)

cc_test(
    name = "http_response_header_test",
    srcs = ["http_response_header_test.cc"],
    deps = [
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:http_response_header",
        "//TinyAlpacaServer/src:literals",
        "//absl/strings",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/src:progmem_string_data",
    ],
)

cc_test(
    name = "json_response_test",
    srcs = ["json_response_test.cc"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:json_response",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:json_test_utils",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/src:any_printable",
        "//mcucore/src:json_encoder",
        "//mcucore/src:string_view",
    ],
)

cc_test(
    name = "literals_test",
    srcs = ["literals_test.cc"],
    deps = [
        "//TinyAlpacaServer/src:literals",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:print_value_to_std_string",
        "//mcucore/extras/test_tools:string_view_utils",
        "//mcucore/src:string_compare",
        "//mcucore/src:string_view",
    ],
)

cc_test(
    name = "match_literals_test",
    srcs = ["match_literals_test.cc"],
    deps = [
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:match_literals",
        "//absl/strings",
        "//glog:logging",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:string_view_utils",
        "//mcucore/src:string_view",
    ],
)

cc_test(
    name = "request_decoder_test",
    srcs = ["request_decoder_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:mock_request_decoder_listener",
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:config",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:extra_parameters",
        "//TinyAlpacaServer/src:request_decoder",
        "//TinyAlpacaServer/src:request_decoder_listener",
        "//absl/flags:flag",
        "//absl/strings",
        "//glog:logging",
        "//googletest:gunit_main",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:string_view",
    ],
)

cc_test(
    name = "server_description_test",
    srcs = ["server_description_test.cc"],
    deps = [
        "//TinyAlpacaServer/src:server_description",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:print_to_std_string",
        "//mcucore/src:json_encoder",
        "//mcucore/src:json_encoder_helpers",
    ],
)

cc_test(
    name = "tiny_alpaca_server_base_test",
    srcs = ["tiny_alpaca_server_base_test.cc"],
    deps = [
        "//TinyAlpacaServer/extras/test_tools:decode_and_dispatch_test_base",
        "//TinyAlpacaServer/extras/test_tools:test_tiny_alpaca_server",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src:server_description",
        "//TinyAlpacaServer/src:tiny_alpaca_server",
        "//absl/strings",
        "//googletest:gunit_main",
        "//mcucore/extras/test_tools:http_request",
        "//mcucore/extras/test_tools:http_response",
        "//mcucore/src:array_view",
        "//mcucore/src:mcucore_platform",
        "//mcunet/src:platform_network",
    ],
)
