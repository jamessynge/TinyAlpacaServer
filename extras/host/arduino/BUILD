# This package contains the subset of what Arduino.h offers and is used by
# Tiny Alpaca Server, thus allowing us to build and test the server on a host,
# instead of on an Arduino.
#
# In addition, this package helps us work around the fact that Arduino doesn't
# have the Standard Template Library, even those methods that don't require
# memory allocation, such as std::numeric_limits<int>::max(). Therefore we
# provide libraries here that expose the relevant functions to the rest of
# Tiny Alpaca Server, and do NOT require importing STL headers in most source
# files.

cc_library(
    name = "arduino",
    srcs = ["arduino.cc"],
    hdrs = ["arduino.h"],
    deps = [
        ":character",
        ":int_types",
        ":pgmspace",
        ":print",
        ":serial",
        ":stream",
        "//absl/time",
    ],
)

cc_library(
    name = "arduino_main",
    srcs = ["arduino_main.cc"],
    linkopts = [
        # Don't complain that setup and loop aren't defined. They'll be provided
        # by the cc_binary that links in this library, by way of the setup and
        # loop functions in the source of the sketch (.ino file).
        "-Wl,--undefined=setup",
        "-Wl,--undefined=loop",
    ],
    deps = [
        "//absl/flags:flag",
        "//base",
    ],
)

cc_library(
    name = "character",
    srcs = ["character.cc"],
    hdrs = ["character.h"],
    deps = ["//absl/strings"],
)

cc_library(
    name = "client",
    hdrs = ["client.h"],
    deps = [
        ":ip_address",
        ":stream",
    ],
)

cc_library(
    name = "int_types",
    hdrs = ["int_types.h"],
)

cc_library(
    name = "ip_address",
    hdrs = ["ip_address.h"],
    deps = [
        ":int_types",
        ":print",
    ],
)

cc_library(
    name = "pgmspace",
    srcs = ["pgmspace.cc"],
    hdrs = ["pgmspace.h"],
    deps = ["//absl/strings"],
)

cc_library(
    name = "print",
    srcs = ["print.cc"],
    hdrs = ["print.h"],
)

cc_library(
    name = "serial",
    srcs = ["serial.cc"],
    hdrs = ["serial.h"],
    deps = [":print"],
)

cc_library(
    name = "stream",
    hdrs = ["stream.h"],
    deps = [":print"],
)
