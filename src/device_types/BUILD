# Root package of generic and device-type specific DeviceInterface implementations,
# none of which are for a specific device; i.e. ObservingConditionsAdapter does not
# interface with any particular hardware, but instead makes it easier to write
# hardware specific DeviceInterface implementations.

cc_library(
    name = "device_impl_base",
    srcs = ["device_impl_base.cc"],
    hdrs = ["device_impl_base.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":ascom_error_codes",
        ":constants",
        ":counting_print",
        ":device_info",
        ":device_interface",
        ":http_response_header",
        ":json_encoder",
        ":literals",
        ":o_print_stream",
        ":platform",
        ":status",
        ":status_or",
    ],
)
