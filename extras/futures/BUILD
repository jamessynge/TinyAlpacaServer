# Ideas for the future of Tiny Alpaca Server.

cc_library(
    name = "pretend_devices",
    hdrs = ["pretend_devices.h"],
)

cc_library(
    name = "progmem_pointer",
    hdrs = ["progmem_pointer.h"],
    deps = [":platform"],
)

cc_library(
    name = "proposed_demo_sketch_ino",
    srcs = ["proposed_demo_sketch.ino.cc"],
    tags = [
        "local",
        "manual",
    ],
    deps = [
        ":TinyAlpacaServer",
        ":literal_token",
        ":pretend_devices",
    ],
)
