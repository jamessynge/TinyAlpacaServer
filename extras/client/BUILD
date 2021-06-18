# Client (host) code, primarily for remote testing.

load("//devtools/python/blaze:pytype.bzl", "pytype_strict_binary")

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

pytype_strict_binary(
    name = "cover_calibrator",
    srcs = ["cover_calibrator.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        ":alpaca_http_client",
        "//third_party/py/requests",
    ],
)
