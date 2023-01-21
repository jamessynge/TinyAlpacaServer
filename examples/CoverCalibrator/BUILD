# Apply Tiny Alpaca Server to the serve the AstroMakers Cover Calibrator device.

load(
    "//mcucore/extras/bazel:arduino_cc_sketch.bzl",
    "arduino_cc_sketch",
)

arduino_cc_sketch(
    name = "CoverCalibrator",
    srcs = ["CoverCalibrator.ino.cc"],
    deps = [
        "//TinyAlpacaServer/examples/CoverCalibrator/src:server",
        "//mcucore/extras/host/arduino",
        "//mcunet/extras/host/ethernet5500:host_network_main",
    ],
)
