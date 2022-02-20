# Demonstrates listening for ASCOM Alpaca Discovery messages and responding to them.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_binary(
    name = "TestAlpacaDiscoveryServer",
    srcs = ["TestAlpacaDiscoveryServer.ino.cc"],
    deps = [
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
        "//mcucore/src:McuCore",
        "//mcunet/src:mcu_net",
    ],
)
