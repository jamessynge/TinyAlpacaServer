# Root package of generic and device-type specific DeviceInterface implementations,
# none of which are for a specific device; i.e. ObservingConditionsAdapter does not
# interface with any particular hardware, but instead makes it easier to write
# hardware specific DeviceInterface implementations.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

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
        "//mcucore/src:counting_print",
        "//mcucore/src:has_insert_into",
        "//mcucore/src:json_encoder",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
        "//mcucore/src:printable_cat",
        "//mcucore/src:progmem_string_data",
        "//mcucore/src:progmem_string_view",
        "//mcucore/src:status",
        "//mcucore/src:status_or",
        "//mcucore/src:uuid",
    ],
)
