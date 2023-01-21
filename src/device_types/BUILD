# Root package of generic and device-type specific DeviceInterface implementations,
# none of which are for a specific device; i.e. ObservingConditionsAdapter does not
# interface with any particular hardware, but instead makes it easier to write
# hardware specific DeviceInterface implementations.

load(
    "//mcucore/extras/bazel:arduino_cc_library.bzl",
    "arduino_cc_library",
)

arduino_cc_library(
    name = "device_impl_base",
    srcs = ["device_impl_base.cc"],
    hdrs = ["device_impl_base.h"],
    deps = [
        "//TinyAlpacaServer/src:alpaca_request",
        "//TinyAlpacaServer/src:alpaca_response",
        "//TinyAlpacaServer/src:constants",
        "//TinyAlpacaServer/src:device_description",
        "//TinyAlpacaServer/src:device_interface",
        "//TinyAlpacaServer/src:literals",
        "//TinyAlpacaServer/src:server_context",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src/print:counting_print",
        "//mcucore/src/print:o_print_stream",
        "//mcucore/src/print:printable_cat",
        "//mcucore/src/status",
        "//mcucore/src/status:status_or",
        "//mcucore/src/strings:progmem_string_data",
        "//mcucore/src/strings:progmem_string_view",
    ],
)
