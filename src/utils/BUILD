# Provides utility methods and classes, such as PrintCharJsonEscaped and mcucore::StatusOr
# which aren't necessarily specific to Alpaca, but could be useful in any Arduino
# sketch.

cc_library(
    name = "addresses",
    srcs = ["addresses.cc"],
    hdrs = ["addresses.h"],
    deps = [
        ":platform_ethernet",
        "//mcucore/src:eeprom_io",
        "//mcucore/src:hex_escape",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)

cc_library(
    name = "avr_timer_counter",
    srcs = ["avr_timer_counter.cc"],
    hdrs = ["avr_timer_counter.h"],
    deps = [
        "//mcucore/src:counting_print",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:print_misc",
        "//mcucore/src:print_to_trait",
        "//mcucore/src:stream_to_print",
        "//mcucore/src:type_traits",
    ],
)

cc_library(
    name = "connection",
    srcs = ["connection.cc"],
    hdrs = ["connection.h"],
    deps = [
        ":platform_ethernet",
        "//mcucore/src:logging",
    ],
)

cc_library(
    name = "ip_device",
    srcs = ["ip_device.cc"],
    hdrs = ["ip_device.h"],
    deps = [
        ":addresses",
        ":platform_ethernet",
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "moving_average",
    srcs = ["moving_average.cc"],
    hdrs = ["moving_average.h"],
    deps = [
        "//mcucore/src:inline_literal",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "platform_ethernet",
    srcs = ["platform_ethernet.cc"],
    hdrs = ["platform_ethernet.h"],
    deps = [
        "//mcucore/extras/host/arduino:client",
        "//mcucore/extras/host/arduino:ip_address",
        "//mcucore/extras/host/ethernet5500",
        "//mcucore/extras/host/ethernet5500:host_sockets",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
    ],
)

cc_library(
    name = "server_socket",
    srcs = ["server_socket.cc"],
    hdrs = ["server_socket.h"],
    deps = [
        ":connection",
        ":platform_ethernet",
        ":socket_listener",
        "//mcucore/src:logging",
        "//mcucore/src:mcucore_platform",
        "//mcucore/src:o_print_stream",
    ],
)

cc_library(
    name = "socket_listener",
    hdrs = ["socket_listener.h"],
    deps = [
        ":connection",
        "//mcucore/src:mcucore_platform",
    ],
)
