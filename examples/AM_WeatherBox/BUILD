load(
    "//TinyAlpacaServer/extras/dev_tools:build_config.bzl",
    "build_config",
)

package(
    default_copts = build_config.default_copts,
)

cc_library(
    name = "am__weather_box_ino",
    srcs = ["AM_WeatherBox.ino.cc"],
    deps = [
        "//TinyAlpacaServer/examples/AM_WeatherBox/src:server",
        "//mcucore/extras/host/arduino",
    ],
)
