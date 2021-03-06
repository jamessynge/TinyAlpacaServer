# Provides utility methods and classes, such as PrintCharJsonEscaped and StatusOr
# which aren't necessarily specific to Alpaca, but could be useful in any Arduino
# sketch.

cc_library(
    name = "status",
    hdrs = ["status.h"],
    deps = [":platform"],
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
