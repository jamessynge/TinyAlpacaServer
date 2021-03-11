# Ideas for the future of Tiny Alpaca Server.

cc_library(
    name = "pretend_devices",
    hdrs = ["pretend_devices.h"],
)

cc_library(
    name = "progmem_pointer",
    hdrs = ["progmem_pointer.h"],
    deps = [
        ":platform",
    ],
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
        ":pretend_devices",
    ],
)

cc_library(
    name = "socket",
    hdrs = ["socket.h"],
    deps = [
        ":platform_ethernet",
        ":socket_listener",
    ],
)

cc_library(
    name = "socket_listener",
    hdrs = ["socket_listener.h"],
    deps = [
        ":platform",
        ":platform_ethernet",
    ],
)
