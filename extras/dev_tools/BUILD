# Tooling to support developing Tiny Alpaca Server.

load("//devtools/python/blaze:strict.bzl", "py_strict_test")
load("//devtools/python/blaze:pytype.bzl", "pytype_strict_binary", "pytype_strict_library")

pytype_strict_binary(
    name = "make_enum_to_string",
    srcs = ["make_enum_to_string.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":tokenize_cpp_lib"],
)

pytype_strict_binary(
    name = "tokenize_cpp",
    srcs = ["tokenize_cpp.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = ["//third_party/py/dataclasses"],
)

pytype_strict_binary(
    name = "alpaca_client",
    srcs = ["alpaca_client.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = ["//third_party/py/requests"],
)

pytype_strict_library(
    name = "tokenize_cpp_lib",
    srcs = ["tokenize_cpp.py"],
    srcs_version = "PY3",
    deps = ["//third_party/py/dataclasses"],
)

py_strict_test(
    name = "tokenize_cpp_test",
    srcs = ["tokenize_cpp_test.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        ":tokenize_cpp_lib",
        "//testing/pybase",
        "//third_party/py/absl/flags",
    ],
)
