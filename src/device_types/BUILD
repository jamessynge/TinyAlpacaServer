# Root package of generic and device-type specific DeviceInterface implementations,
# none of which are for a specific device; i.e. ObservingConditionsAdapter does not
# interface with any particular hardware, but instead makes it easier to write
# hardware specific DeviceInterface implementations.

cc_library(
    name = "device_impl_base",
    srcs = ["device_impl_base.cc"],
    hdrs = ["device_impl_base.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
        "//src:alpaca_request",
        "//src:alpaca_response",
        "//src:ascom_error_codes",
        "//src:constants",
        "//src:device_info",
        "//src:device_interface",
        "//src:http_response_header",
        "//src:literals",
        "//src/utils:counting_print",
        "//src/utils:json_encoder",
        "//src/utils:status",
        "//src/utils:status_or",
    ],
)
