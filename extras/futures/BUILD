# TODO(jamessynge): describe this package.

cc_library(
    name = "literal",
    hdrs = ["literal.h"],
    deps = [
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "literals",
    hdrs = ["literals.h"],
    textual_hdrs = [
        "cased_literals.inc",
        "lower_case_literals.inc",
    ],
    deps = [
        ":literal",
        ":platform",
    ],
)
