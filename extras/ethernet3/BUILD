# A fake host version of sstaub's Ethernet3 for the WIZ5500, just enough for
# running Tiny Alpaca Server on a *nix compatible host (i.e. with the usual
# socket APIs).

cc_library(
    name = "ethernet3",
    hdrs = ["ethernet3.h"],
    deps = [
        ":ethernet_class",
        ":ethernet_client",
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
    hdrs = ["ethernet_client.h"],
    deps = [
        ":client",
    ],
)
