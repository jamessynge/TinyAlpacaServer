# A fake host version of sstaub's Ethernet3 for the WIZ5500, just enough for
# running Tiny Alpaca Server on a *nix compatible host (i.e. with the usual
# socket APIs).

cc_library(
    name = "ethernet3",
    hdrs = ["ethernet3.h"],
    deps = [
        ":ethernet_class",
        ":ethernet_client",
        ":ethernet_config",
        ":ip_address",
    ],
)

cc_library(
    name = "ethernet_class",
    srcs = ["ethernet_class.cc"],
    hdrs = ["ethernet_class.h"],
    deps = [":ip_address"],
)

cc_library(
    name = "ethernet_client",
    srcs = ["ethernet_client.cc"],
    hdrs = ["ethernet_client.h"],
    deps = [
        ":client",
        ":ethernet_config",
        ":platform",
    ],
)

cc_library(
    name = "ethernet_config",
    hdrs = ["ethernet_config.h"],
)

cc_library(
    name = "host_sockets",
    srcs = ["host_sockets.cc"],
    hdrs = ["host_sockets.h"],
    deps = [
        ":ethernet_config",
        "//core:logging",
    ],
)
