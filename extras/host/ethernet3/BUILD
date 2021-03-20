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
        ":ethernet_server",
        ":ip_address",
        ":w5500",
    ],
)

cc_library(
    name = "ethernet_class",
    srcs = ["ethernet_class.cc"],
    hdrs = ["ethernet_class.h"],
    deps = [
        ":ethernet_config",
        ":ip_address",
    ],
)

cc_library(
    name = "ethernet_client",
    srcs = ["ethernet_client.cc"],
    hdrs = ["ethernet_client.h"],
    deps = [
        ":client",
        ":ethernet_config",
        ":host_sockets",
        ":platform",
        ":w5500",
    ],
)

cc_library(
    name = "ethernet_config",
    hdrs = ["ethernet_config.h"],
)

cc_library(
    name = "ethernet_server",
    srcs = ["ethernet_server.cc"],
    hdrs = ["ethernet_server.h"],
    deps = [
        ":ethernet_class",
        ":ethernet_client",
        ":host_sockets",
        ":w5500",
        "//absl/time",
        "//core:logging",
    ],
)

cc_library(
    name = "host_sockets",
    srcs = ["host_sockets.cc"],
    hdrs = ["host_sockets.h"],
    deps = [
        ":ethernet_config",
        ":logging",
        ":w5500",
        "//core:logging",
    ],
)

cc_library(
    name = "w5500",
    hdrs = ["w5500.h"],
    deps = [":int_types"],
)
