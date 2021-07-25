# Client (host) code, primarily for remote testing.

load("//devtools/python/blaze:pytype.bzl", "pytype_strict_binary", "pytype_strict_library")

pytype_strict_binary(
    name = "alpaca_discovery",
    srcs = ["alpaca_discovery.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        "//third_party/py/dataclasses",
        "//third_party/py/netifaces",
    ],
)

pytype_strict_binary(
    name = "alpaca_http_client",
    srcs = ["alpaca_http_client.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = ["//third_party/py/requests"],
)

pytype_strict_library(
    name = "alpaca_http_client_lib",
    srcs = ["alpaca_http_client.py"],
    srcs_version = "PY3",
    deps = ["//third_party/py/requests"],
)

pytype_strict_binary(
    name = "am_weather_box",
    srcs = ["am_weather_box.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":alpaca_http_client_lib"],
)

pytype_strict_binary(
    name = "cover_calibrator",
    srcs = ["cover_calibrator.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":alpaca_http_client_lib"],
)

pytype_strict_binary(
    name = "get_api_versions",
    srcs = ["get_api_versions.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":alpaca_http_client_lib"],
)

pytype_strict_binary(
    name = "get_configured_devices",
    srcs = ["get_configured_devices.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":alpaca_http_client_lib"],
)

pytype_strict_binary(
    name = "set_brightness",
    srcs = ["set_brightness.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":alpaca_http_client_lib"],
)
