# Sketch for testing that we can initialize the IpDevice (i.e. WIZnet W5500).

cc_binary(
    name = "TestIpDeviceSetup",
    srcs = ["TestIpDeviceSetup.ino.cc"],
    deps = [
        "//experimental/users/jamessynge/arduino/hostuino/extras/host/arduino",
        "//experimental/users/jamessynge/arduino/hostuino/extras/host/arduino:arduino_main",
        "//experimental/users/jamessynge/arduino/hostuino/extras/host/ethernet5500",
        "//src:TinyAlpacaServer",
    ],
)
