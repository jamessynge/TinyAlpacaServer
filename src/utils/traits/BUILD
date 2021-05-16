# Features like STL's <type_traits>, but for a C++ 11 environment with no STL.

cc_library(
    name = "print_to_trait",
    hdrs = ["print_to_trait.h"],
    deps = [
        ":type_traits",
        "//src/utils:platform",
    ],
)

cc_library(
    name = "type_traits",
    hdrs = ["type_traits.h"],
    deps = ["//src/utils:platform"],
)
