# With Arduino Builder, can be used to measure the amount of memory
# required when various objects/code is referenced by the sketch.

cc_binary(
    name = "decoder_memory_measurements",
    srcs = ["DecoderMemoryMeasurements.ino.cc"],
    tags = [
        "local",
        "manual",
    ],
    deps = [
        ":arduino_main",
        ":tiny_alpaca_server",
    ],
)
