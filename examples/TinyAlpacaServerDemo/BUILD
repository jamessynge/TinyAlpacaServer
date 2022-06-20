# Demo of how to use Tiny Alpaca Server.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_binary(
    name = "TinyAlpacaServerDemo",
    srcs = ["TinyAlpacaServerDemo.ino.cc"],
    deps = [
        "//TinyAlpacaServer/examples/TinyAlpacaServerDemo/src:server",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
    ],
)
