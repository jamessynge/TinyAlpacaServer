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
    name = "set_dhcp_trait",
    hdrs = ["set_dhcp_trait.h"],
    deps = [
        ":type_traits",
        "//src/utils:platform_ethernet",
    ],
)

cc_library(
    name = "type_traits",
    hdrs = ["type_traits.h"],
    deps = ["//src/utils:platform"],
)
