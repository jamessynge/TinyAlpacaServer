# With Arduino Builder, can be used to measure the amount of memory
# required when various objects/code is referenced by the sketch.

cc_binary(
    name = "encoder_memory_measurements",
    srcs = ["EncoderMemoryMeasurements.ino.cc"],
    deps = [
        ":arduino_main",
        ":tiny_alpaca_server",
    ],
)
