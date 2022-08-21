# Tooling to support developing Tiny Alpaca Server.
#
# Many python files in this directory have both library and binary
# targets to permit testing the library by just running it.

load("//devtools/python/blaze:pytype.bzl", "pytype_strict_binary")
load("//tools/build_defs/testing:bzl_library.bzl", "bzl_library")

bzl_library(
    name = "build_config_bzl",
    srcs = ["build_config.bzl"],
    parse_tests = False,
    visibility = ["//visibility:private"],
)

pytype_strict_binary(
    name = "swagger_yaml",
    srcs = ["swagger_yaml.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = ["//third_party/py/requests"],
)
