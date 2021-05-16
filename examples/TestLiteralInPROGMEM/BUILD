# With Arduino Builder, can be used to measure the amount of memory
# required when various objects/code is referenced by the sketch.

cc_binary(
    name = "test_literal_in_progmem",
    srcs = ["TestLiteralInPROGMEM.ino.cc"],
    tags = [
        "local",
        "manual",
    ],
    deps = [
        "//extras/host/arduino",
        "//extras/host/arduino:arduino_main",
        "//src:TinyAlpacaServer",
    ],
)
