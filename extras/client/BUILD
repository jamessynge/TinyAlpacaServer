# Client (host) code, primarily for remote testing.

load("//devtools/python/blaze:pytype.bzl", "pytype_strict_binary", "pytype_strict_library")

pytype_strict_library(
    name = "alpaca_client_lib",
    srcs = ["alpaca_client.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
        ":alpaca_model",
    ],
)

pytype_strict_binary(
    name = "alpaca_client",
    srcs = ["alpaca_client.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
        ":alpaca_model",
        ":install_advice",
        "//third_party/py/netifaces",
    ],
)

pytype_strict_binary(
    name = "alpaca_discovery",
    srcs = ["alpaca_discovery.py"],
    deps = [
        ":install_advice",
        "//third_party/py/netifaces",
    ],
)

pytype_strict_library(
    name = "alpaca_discovery_lib",
    srcs = ["alpaca_discovery.py"],
    deps = [
        ":install_advice",
        "//third_party/py/netifaces",
    ],
)

pytype_strict_binary(
    name = "alpaca_http_client",
    srcs = ["alpaca_http_client.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_model",
        ":install_advice",
        "//third_party/py/requests",
    ],
)

pytype_strict_library(
    name = "alpaca_http_client_lib",
    srcs = ["alpaca_http_client.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_model",
        ":install_advice",
        "//third_party/py/requests",
    ],
)

pytype_strict_library(
    name = "alpaca_model",
    srcs = ["alpaca_model.py"],
)

pytype_strict_binary(
    name = "am_weather_box",
    srcs = ["am_weather_box.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
    ],
)

pytype_strict_binary(
    name = "cover_calibrator",
    srcs = ["cover_calibrator.py"],
    deps = [
        ":alpaca_client_lib",
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
        ":alpaca_model",
    ],
)

pytype_strict_binary(
    name = "get_api_versions",
    srcs = ["get_api_versions.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
    ],
)

pytype_strict_binary(
    name = "get_configured_devices",
    srcs = ["get_configured_devices.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
    ],
)

pytype_strict_library(
    name = "install_advice",
    srcs = ["install_advice.py"],
)

pytype_strict_binary(
    name = "set_brightness",
    srcs = ["set_brightness.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
    ],
)

pytype_strict_library(
    name = "switch_info",
    srcs = ["switch_info.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
    ],
)

pytype_strict_library(
    name = "weather_close_demo",
    srcs = ["weather_close_demo.py"],
    deps = [
        ":alpaca_discovery_lib",
        ":alpaca_http_client_lib",
        "//third_party/py/curses",
    ],
)
