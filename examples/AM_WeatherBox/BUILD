load(
    "//mcucore/extras/bazel:arduino_cc_sketch.bzl",
    "arduino_cc_sketch",
)

arduino_cc_sketch(
    name = "am__weather_box_ino",
    srcs = ["AM_WeatherBox.ino.cc"],
    deps = [
        "//TinyAlpacaServer/examples/AM_WeatherBox/src:server",
        "//mcucore/extras/host/arduino",
        "//mcunet/extras/host/ethernet5500:host_network_main",
    ],
)
