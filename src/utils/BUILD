# Provides utility methods and classes, such as PrintCharJsonEscaped and StatusOr
# which aren't necessarily specific to Alpaca, but could be useful in any Arduino
# sketch.

cc_library(
    name = "addresses",
    srcs = ["addresses.cc"],
    hdrs = ["addresses.h"],
    deps = [
        ":eeprom_io",
        ":hex_escape",
        ":platform_ethernet",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)

cc_library(
    name = "any_printable",
    srcs = ["any_printable.cc"],
    hdrs = ["any_printable.h"],
    deps = [
        ":literal",
        ":string_view",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:progmem_string",
        "//mcucore/src:progmem_string_view",
    ],
)

cc_library(
    name = "array",
    hdrs = ["array.h"],
    deps = [
        ":array_view",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "array_view",
    hdrs = ["array_view.h"],
    deps = [
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:type_traits",
    ],
)

cc_library(
    name = "avr_timer_counter",
    srcs = ["avr_timer_counter.cc"],
    hdrs = ["avr_timer_counter.h"],
    deps = [
        ":counting_print",
        ":print_misc",
        ":stream_to_print",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:print_to_trait",
        "//mcucore/src:type_traits",
    ],
)

cc_library(
    name = "connection",
    srcs = ["connection.cc"],
    hdrs = ["connection.h"],
    deps = [
        ":platform_ethernet",
        "//mcucore/src:logging",
    ],
)

cc_library(
    name = "counting_print",
    srcs = ["counting_print.cc"],
    hdrs = ["counting_print.h"],
    deps = ["//mcucore/src:mcucore_platform"],
)

cc_library(
    name = "eeprom_io",
    srcs = ["eeprom_io.cc"],
    hdrs = ["eeprom_io.h"],
    deps = [
        ":hex_escape",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)

cc_library(
    name = "hex_escape",
    srcs = ["hex_escape.cc"],
    hdrs = ["hex_escape.h"],
    deps = [
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "ip_device",
    srcs = ["ip_device.cc"],
    hdrs = ["ip_device.h"],
    deps = [
        ":addresses",
        ":platform_ethernet",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "json_encoder",
    srcs = ["json_encoder.cc"],
    hdrs = ["json_encoder.h"],
    deps = [
        ":any_printable",
        ":array_view",
        ":counting_print",
        ":literal",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
        "//mcucore/src:progmem_string",
        "//mcucore/src:progmem_string_view",
    ],
)

cc_library(
    name = "json_encoder_helpers",
    srcs = ["json_encoder_helpers.cc"],
    hdrs = ["json_encoder_helpers.h"],
    deps = [
        ":counting_print",
        ":json_encoder",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "literal",
    srcs = ["literal.cc"],
    hdrs = ["literal.h"],
    deps = [
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:print_to_trait",
        "//mcucore/src:progmem_string",
        "//mcucore/src:progmem_string_view",
    ],
)

cc_library(
    name = "moving_average",
    srcs = ["moving_average.cc"],
    hdrs = ["moving_average.h"],
    deps = [
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "platform_ethernet",
    srcs = ["platform_ethernet.cc"],
    hdrs = ["platform_ethernet.h"],
    deps = [
        "//mcucore/extras/host/arduino:client",
        "//mcucore/extras/host/arduino:ip_address",
        "//mcucore/extras/host/ethernet5500",
        "//mcucore/extras/host/ethernet5500:host_sockets",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "print_misc",
    srcs = ["print_misc.cc"],
    hdrs = ["print_misc.h"],
    deps = [
        ":counting_print",
        "//mcucore/src:inline_literal",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "printable_cat",
    srcs = ["printable_cat.cc"],
    hdrs = ["printable_cat.h"],
    deps = [
        ":any_printable",
        ":array",
        ":array_view",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "server_socket",
    srcs = ["server_socket.cc"],
    hdrs = ["server_socket.h"],
    deps = [
        ":connection",
        ":platform_ethernet",
        ":socket_listener",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)

cc_library(
    name = "socket_listener",
    hdrs = ["socket_listener.h"],
    deps = [
        ":connection",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "status",
    srcs = ["status.cc"],
    hdrs = ["status.h"],
    deps = [
        ":counting_print",
        ":hex_escape",
        ":literal",
        "//mcucore/src:inline_literal",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)

cc_library(
    name = "status_or",
    hdrs = ["status_or.h"],
    deps = [
        ":status",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "stream_to_print",
    hdrs = ["stream_to_print.h"],
    deps = [
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)

cc_library(
    name = "string_compare",
    srcs = ["string_compare.cc"],
    hdrs = ["string_compare.h"],
    deps = [
        ":literal",
        ":string_view",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:progmem_string_view",
    ],
)

cc_library(
    name = "string_view",
    srcs = [
        "string_view.cc",
    ],
    hdrs = [
        "string_view.h",
    ],
    deps = [
        ":hex_escape",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
        "//mcucore/src:print_to_trait",
    ],
)

cc_library(
    name = "tiny_string",
    hdrs = ["tiny_string.h"],
    deps = [
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)
