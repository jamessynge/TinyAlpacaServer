# Provides utility methods and classes, such as PrintCharJsonEscaped and StatusOr
# which aren't necessarily specific to Alpaca, but could be useful in any Arduino
# sketch.

cc_library(
    name = "addresses",
    srcs = ["addresses.cc"],
    hdrs = ["addresses.h"],
    deps = [
        ":eeprom_io",
        ":logging",
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
    deps = [":platform"],
)

cc_library(
    name = "connection",
    srcs = ["connection.cc"],
    hdrs = ["connection.h"],
    deps = [
        ":platform",
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
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "escaping",
    srcs = ["escaping.cc"],
    hdrs = ["escaping.h"],
    deps = [
        ":any_printable",
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "ip_device",
    srcs = ["ip_device.cc"],
    hdrs = ["ip_device.h"],
    deps = [
        ":addresses",
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
        ":counting_bitbucket",
        ":escaping",
        ":literal",
        ":platform",
    ],
)

cc_library(
    name = "literal",
    srcs = ["literal.cc"],
    hdrs = ["literal.h"],
    deps = [
        ":logging",
        ":platform",
        ":print_ostream",
        "//absl/strings",
    ],
)

cc_library(
    name = "literal_token",
    hdrs = ["literal_token.h"],
    deps = [
        ":array_view",
        ":literal",
        ":platform",
        ":string_compare",
        ":string_view",
    ],
)

cc_library(
    name = "logging",
    srcs = ["logging.cc"],
    hdrs = ["logging.h"],
    deps = [
        ":platform",
        ":print_ostream",
        "//core:logging",
    ],
)

cc_library(
    name = "platform",
    hdrs = ["platform.h"],
    deps = [
        ":client",
        ":host_sockets",
        ":ip_address",
        ":logging",
        ":pgmspace",
        ":platform",
        ":print",
        ":stream",
    ],
)

cc_library(
    name = "print_ostream",
    hdrs = ["print_ostream.h"],
    deps = [
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
    name = "server_connection_base",
    srcs = ["server_connection_base.cc"],
    hdrs = ["server_connection_base.h"],
    deps = [
        ":connection",
        ":logging",
        ":platform",
        ":platform_ethernet",
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
        ":logging",
        ":platform",
        ":print_ostream",
        "//absl/strings",
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
