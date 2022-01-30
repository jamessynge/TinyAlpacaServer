# Tooling to support developing Tiny Alpaca Server.
#
# Many python files in this directory have both library and binary
# targets to permit testing the library by just running it.

load("//devtools/python/blaze:pytype.bzl", "pytype_strict_binary", "pytype_strict_library")
load("//devtools/python/blaze:strict.bzl", "py_strict_test")

pytype_strict_binary(
    name = "add_flashstr",
    srcs = ["add_flashstr.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":tokenize_cpp_lib"],
)

pytype_strict_binary(
    name = "cpp_enum",
    srcs = ["cpp_enum.py"],
    deps = [":tokenize_cpp_lib"],
)

pytype_strict_library(
    name = "cpp_enum_lib",
    srcs = ["cpp_enum.py"],
    deps = [":tokenize_cpp_lib"],
)

py_strict_test(
    name = "cpp_enum_test",
    srcs = ["cpp_enum_test.py"],
    deps = [
        ":cpp_enum_lib",
        ":tokenize_cpp_lib",
        "//absltest",
        "//third_party/py/absl/flags",
    ],
)

pytype_strict_binary(
    name = "file_edit",
    srcs = ["file_edit.py"],
)

pytype_strict_library(
    name = "file_edit_lib",
    srcs = ["file_edit.py"],
)

pytype_strict_binary(
    name = "find_non_progmem_strings",
    srcs = ["find_non_progmem_strings.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":tokenize_cpp_lib"],
)

pytype_strict_binary(
    name = "make_enum_to_string",
    srcs = ["make_enum_to_string.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [
        ":cpp_enum_lib",
        ":file_edit_lib",
        ":tokenize_cpp_lib",
    ],
)

pytype_strict_binary(
    name = "swagger_yaml",
    srcs = ["swagger_yaml.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = ["//third_party/py/requests"],
)

pytype_strict_binary(
    name = "tokenize_cpp",
    srcs = ["tokenize_cpp.py"],
    python_version = "PY3",
    srcs_version = "PY3",
    deps = [":file_edit_lib"],
)

pytype_strict_library(
    name = "tokenize_cpp_lib",
    srcs = ["tokenize_cpp.py"],
    srcs_version = "PY3",
    deps = [":file_edit_lib"],
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
