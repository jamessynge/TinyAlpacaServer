# Ideas for the future of Tiny Alpaca Server.

cc_library(
    name = "progmem_pointer",
    hdrs = ["progmem_pointer.h"],
    deps = [
        ":platform",
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

cc_library(
    name = "time",
    srcs = ["time.cc"],
    hdrs = ["time.h"],
    deps = [":platform"],
)
