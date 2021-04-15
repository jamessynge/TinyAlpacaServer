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
        ":inline_literal",
        ":logging",
        ":o_print_stream",
        ":platform",
        ":platform_ethernet",
    ],
)

cc_library(
    name = "any_printable",
    srcs = ["any_printable.cc"],
    hdrs = ["any_printable.h"],
    deps = [
        ":literal",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "array",
    hdrs = ["array.h"],
    deps = [
        ":array_view",
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "array_view",
    hdrs = ["array_view.h"],
    deps = [
        ":logging",
        ":platform",
        ":type_traits",
    ],
)

cc_library(
    name = "connection",
    srcs = ["connection.cc"],
    hdrs = ["connection.h"],
    deps = [
        ":logging",
        ":platform_ethernet",
    ],
)

cc_library(
    name = "counting_bitbucket",
    hdrs = ["counting_bitbucket.h"],
    deps = [
        ":platform",
    ],
)

cc_library(
    name = "eeprom_io",
    srcs = ["eeprom_io.cc"],
    hdrs = ["eeprom_io.h"],
    deps = [
        ":hex_escape",
        ":logging",
        ":o_print_stream",
        ":platform",
    ],
)

cc_library(
    name = "hex_escape",
    srcs = ["hex_escape.cc"],
    hdrs = ["hex_escape.h"],
    deps = [
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "inline_literal",
    hdrs = ["inline_literal.h"],
    copts = ["-std=c++11"],
    deps = [
        ":platform",
        ":printable_progmem_string",
    ],
)

cc_library(
    name = "ip_device",
    srcs = ["ip_device.cc"],
    hdrs = ["ip_device.h"],
    deps = [
        ":addresses",
        ":inline_literal",
        ":platform",
        ":platform_ethernet",
    ],
)

cc_library(
    name = "json_encoder",
    srcs = ["json_encoder.cc"],
    hdrs = ["json_encoder.h"],
    deps = [
        ":any_printable",
        ":array_view",
        ":counting_bitbucket",
        ":literal",
        ":o_print_stream",
        ":platform",
    ],
)

cc_library(
    name = "json_encoder_helpers",
    hdrs = ["json_encoder_helpers.h"],
    deps = [
        ":json_encoder",
        ":platform",
    ],
)

cc_library(
    name = "literal",
    srcs = ["literal.cc"],
    hdrs = ["literal.h"],
    deps = [
        ":logging",
        ":o_print_stream",
        ":platform",
        ":printable_progmem_string",
    ],
)

cc_library(
    name = "log_sink",
    srcs = ["log_sink.cc"],
    hdrs = ["log_sink.h"],
    deps = [
        ":inline_literal",
        ":o_print_stream",
        ":platform",
        "//base:logging_extensions",
        "//core:logging",
    ],
)

cc_library(
    name = "logging",
    hdrs = ["logging.h"],
    deps = [
        ":log_sink",
        ":utils_config",
    ],
)

cc_library(
    name = "moving_average",
    srcs = ["moving_average.cc"],
    hdrs = ["moving_average.h"],
    deps = [
        ":inline_literal",
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "o_print_stream",
    hdrs = ["o_print_stream.h"],
    deps = [
        ":platform",
        ":print_to_trait",
        ":type_traits",
    ],
)

cc_library(
    name = "platform",
    hdrs = ["platform.h"],
    deps = [
        ":core_headers",
        ":pgmspace",
        ":print",
        ":stream",
        ":utils_config",
    ],
)

cc_library(
    name = "platform_ethernet",
    srcs = ["platform_ethernet.cc"],
    hdrs = ["platform_ethernet.h"],
    deps = [
        ":client",
        ":host_sockets",
        ":ip_address",
        ":logging",
        ":platform",
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
        ":platform",
    ],
)

cc_library(
    name = "printable_progmem_string",
    srcs = ["printable_progmem_string.cc"],
    hdrs = ["printable_progmem_string.h"],
    deps = [":platform"],
)

cc_library(
    name = "server_socket",
    srcs = ["server_socket.cc"],
    hdrs = ["server_socket.h"],
    deps = [
        ":connection",
        ":logging",
        ":platform",
        ":platform_ethernet",
        ":socket_listener",
    ],
)

cc_library(
    name = "socket_listener",
    hdrs = ["socket_listener.h"],
    deps = [
        ":connection",
        ":platform",
    ],
)

cc_library(
    name = "status",
    hdrs = ["status.h"],
    deps = [
        ":literal",
        ":platform",
    ],
)

cc_library(
    name = "status_or",
    hdrs = ["status_or.h"],
    deps = [
        ":logging",
        ":platform",
        ":status",
    ],
)

cc_library(
    name = "string_compare",
    srcs = ["string_compare.cc"],
    hdrs = ["string_compare.h"],
    deps = [
        ":literal",
        ":platform",
        ":string_view",
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
        ":logging",
        ":o_print_stream",
        ":platform",
    ],
)

cc_library(
    name = "tiny_string",
    hdrs = ["tiny_string.h"],
    deps = [
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "utils_config",
    hdrs = ["utils_config.h"],
)
