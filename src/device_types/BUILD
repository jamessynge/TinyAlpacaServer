# Root package of generic and device-type specific DeviceInterface implementations,
# none of which are for a specific device; i.e. ObservingConditionsAdapter does not
# interface with any particular hardware, but instead makes it easier to write
# hardware specific DeviceInterface implementations.

cc_library(
    name = "device_impl_base",
    srcs = ["device_impl_base.cc"],
    hdrs = ["device_impl_base.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:ascom_error_codes",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_info",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:http_response_header",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src/utils:counting_print",
        "//TinyAlpacaServer/src/utils:json_encoder",
        "//TinyAlpacaServer/src/utils:status",
        "//TinyAlpacaServer/src/utils:status_or",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)
