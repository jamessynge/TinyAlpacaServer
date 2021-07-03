# Tooling to support developing Tiny Alpaca Server.

load("//devtools/python/blaze:pytype.bzl", "pytype_strict_binary", "pytype_strict_library")
load("//devtools/python/blaze:strict.bzl", "py_strict_test")

pytype_strict_binary(
    name = "add_flashstr",
    srcs = ["add_flashstr.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        ":tokenize_cpp_lib",
        "//third_party/py/dataclasses",
    ],
)

pytype_strict_binary(
    name = "find_non_progmem_strings",
    srcs = ["find_non_progmem_strings.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        ":tokenize_cpp_lib",
        "//third_party/py/dataclasses",
    ],
)

pytype_strict_binary(
    name = "make_enum_to_string",
    srcs = ["make_enum_to_string.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":tokenize_cpp_lib"],
)

pytype_strict_binary(
    name = "make_literals_inc",
    srcs = ["make_literals_inc.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        ":tokenize_cpp_lib",
        "//third_party/py/dataclasses",
    ],
)

pytype_strict_binary(
    name = "swagger_yaml",
    srcs = ["swagger_yaml.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        "//third_party/py/dataclasses",
        "//third_party/py/requests",
    ],
)

pytype_strict_binary(
    name = "tokenize_cpp",
    srcs = ["tokenize_cpp.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = ["//third_party/py/dataclasses"],
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
        "//absltest",
        "//third_party/py/absl/flags",
    ],
)
