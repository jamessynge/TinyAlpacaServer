# Sketch for testing that we can initialize the IpDevice (i.e. WIZnet W5500).

cc_binary(
    name = "TestIpDeviceSetup",
    srcs = ["TestIpDeviceSetup.ino.cc"],
    deps = [
        "//TinyAlpacaServer/src:TinyAlpacaServer",
        "//mcucore/extras/host/arduino",
        "//mcucore/extras/host/arduino:arduino_main",
        "//mcunet/extras/host/ethernet5500",
    ],
)
