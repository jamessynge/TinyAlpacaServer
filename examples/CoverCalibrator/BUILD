# Apply Tiny Alpaca Server to the serve the AstroMakers Cover Calibrator device.

cc_binary(
    name = "CoverCalibrator",
    srcs = ["CoverCalibrator.ino.cc"],
    deps = [
        "//examples/CoverCalibrator/src:server",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
    ],
)
