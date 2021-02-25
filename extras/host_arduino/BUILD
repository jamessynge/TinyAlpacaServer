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
    srcs = ["Arduino.cc"],
    hdrs = ["Arduino.h"],
    deps = [
        ":pgmspace",
        ":print",
        "//absl/strings",
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
    srcs = ["Print.cc"],
    hdrs = ["Print.h"],
)
