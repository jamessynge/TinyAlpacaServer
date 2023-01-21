# Demonstrates listening for ASCOM Alpaca Discovery messages and responding to them.

load(
    "//mcucore/extras/bazel:arduino_cc_sketch.bzl",
    "arduino_cc_sketch",
)

arduino_cc_sketch(
    name = "TestAlpacaDiscoveryServer",
    srcs = ["TestAlpacaDiscoveryServer.ino.cc"],
    deps = [
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/src:McuCore",
        "//mcunet/extras/host/ethernet5500:host_network_main",
        "//mcunet/src:mcu_net",
    ],
)
