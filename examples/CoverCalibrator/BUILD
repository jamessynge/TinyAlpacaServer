# Apply Tiny Alpaca Server to the serve the AstroMakers Cover Calibrator device.

load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_binary(
    name = "CoverCalibrator",
    srcs = ["CoverCalibrator.ino.cc"],
    deps = [
        "//TinyAlpacaServer/examples/CoverCalibrator/src:server",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
    ],
)
