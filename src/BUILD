# Bazel BUILD file for Tiny Alpaca Server. Needs work support building on an arbitrary machine.

cc_library(
    name = "TinyAlpacaServer",
    hdrs = ["TinyAlpacaServer.h"],
    deps = [
        ":addresses",
        ":alpaca_devices",
        ":alpaca_discovery_server",
        ":alpaca_request",
        ":alpaca_response",
        ":any_printable",
        ":array",
        ":array_view",
        ":ascom_error_codes",
        ":config",
        ":connection",
        ":constants",
        ":counting_bitbucket",
        ":device_impl_base",
        ":device_info",
        ":device_interface",
        ":eeprom_io",
        ":extra_parameters",
        ":hex_escape",
        ":http_response_header",
        ":inline_literal",
        ":ip_device",
        ":json_encoder",
        ":json_encoder_helpers",
        ":json_response",
        ":literal",
        ":literals",
        ":log_sink",
        ":logging",
        ":match_literals",
        ":moving_average",
        ":o_print_stream",
        ":observing_conditions_adapter",
        ":platform",
        ":platform_ethernet",
        ":print_to_trait",
        ":printable_cat",
        ":printable_progmem_string",
        ":request_decoder",
        ":request_decoder_listener",
        ":request_listener",
        ":server_connection2",
        ":server_description",
        ":server_socket",
        ":server_socket_and_connection",
        ":server_sockets_and_connections",
        ":socket_listener",
        ":status",
        ":status_or",
        ":string_compare",
        ":string_view",
        ":tiny_alpaca_server",
        ":tiny_string",
        ":type_traits",
        ":utils_config",
    ],
)

cc_library(
    name = "alpaca_devices",
    srcs = ["alpaca_devices.cc"],
    hdrs = ["alpaca_devices.h"],
    deps = [
        ":alpaca_response",
        ":any_printable",
        ":array_view",
        ":ascom_error_codes",
        ":constants",
        ":device_impl_base",
        ":hex_escape",
        ":json_encoder",
        ":json_response",
        ":literals",
        ":logging",
        ":platform",
        ":platform_ethernet",
        ":request_listener",
        ":server_description",
        ":string_view",
    ],
)

cc_library(
    name = "alpaca_discovery_server",
    srcs = ["alpaca_discovery_server.cc"],
    hdrs = ["alpaca_discovery_server.h"],
    deps = [
        ":hex_escape",
        ":literal",
        ":o_print_stream",
        ":platform",
        ":platform_ethernet",
        ":string_compare",
        ":string_view",
    ],
)

cc_library(
    name = "alpaca_request",
    srcs = ["alpaca_request.cc"],
    hdrs = ["alpaca_request.h"],
    deps = [
        ":config",
        ":constants",
        ":extra_parameters",
        ":logging",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "alpaca_response",
    srcs = ["alpaca_response.cc"],
    hdrs = ["alpaca_response.h"],
    deps = [
        ":alpaca_request",
        ":any_printable",
        ":array_view",
        ":ascom_error_codes",
        ":constants",
        ":counting_bitbucket",
        ":http_response_header",
        ":json_encoder",
        ":json_encoder_helpers",
        ":json_response",
        ":literals",
        ":platform",
        ":status_or",
    ],
)

cc_library(
    name = "ascom_error_codes",
    srcs = ["ascom_error_codes.cc"],
    hdrs = ["ascom_error_codes.h"],
    deps = [
        ":literals",
        ":platform",
        ":status",
    ],
)

# config.h specifies the set of features available in the server (e.g. storing extra
# parameters beyond those with hardcoded support in AlpacaRequest and RequestDecoder).
cc_library(
    name = "config",
    hdrs = ["config.h"],
)

cc_library(
    name = "constants",
    srcs = ["constants.cc"],
    hdrs = ["constants.h"],
    deps = [
        ":inline_literal",
        ":platform",
        ":printable_progmem_string",
    ],
)

cc_library(
    name = "device_impl_base",
    srcs = ["device_impl_base.cc"],
    hdrs = ["device_impl_base.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":ascom_error_codes",
        ":constants",
        ":counting_bitbucket",
        ":device_info",
        ":device_interface",
        ":http_response_header",
        ":json_encoder",
        ":literals",
        ":platform",
        ":status",
        ":status_or",
    ],
)

cc_library(
    name = "device_info",
    srcs = ["device_info.cc"],
    hdrs = ["device_info.h"],
    deps = [
        ":constants",
        ":json_encoder",
        ":literal",
        ":literals",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "device_interface",
    srcs = ["device_interface.cc"],
    hdrs = ["device_interface.h"],
    deps = [
        ":alpaca_request",
        ":array_view",
        ":device_info",
        ":json_encoder",
        ":json_encoder_helpers",
        ":json_response",
        ":platform",
    ],
)

cc_library(
    name = "extra_parameters",
    hdrs = ["extra_parameters.h"],
    deps = [
        ":config",
        ":constants",
        ":platform",
        ":string_view",
        ":tiny_string",
    ],
)

cc_library(
    name = "http_response_header",
    srcs = ["http_response_header.cc"],
    hdrs = ["http_response_header.h"],
    deps = [
        ":constants",
        ":literal",
        ":literals",
        ":platform",
    ],
)

cc_library(
    name = "json_response",
    hdrs = ["json_response.h"],
    deps = [
        ":alpaca_request",
        ":any_printable",
        ":json_encoder",
        ":literals",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "literals",
    srcs = ["literals.cc"],
    hdrs = ["literals.h"],
    textual_hdrs = ["literals.inc"],
    deps = [
        ":literal",
        ":platform",
    ],
)

cc_library(
    name = "log_experiment",
    hdrs = ["LogExperiment.h"],
    deps = [
        ":inline_literal",
        ":logging",
        ":platform",
    ],
)

cc_library(
    name = "match_literals",
    srcs = ["match_literals.cc"],
    hdrs = ["match_literals.h"],
    deps = [
        ":array_view",
        ":constants",
        ":literal",
        ":literals",
        ":logging",
        ":string_compare",
        ":string_view",
    ],
)

cc_library(
    name = "request_decoder",
    srcs = ["request_decoder.cc"],
    hdrs = ["request_decoder.h"],
    deps = [
        ":alpaca_request",
        ":config",
        ":constants",
        ":hex_escape",
        ":literals",
        ":logging",
        ":match_literals",
        ":platform",
        ":request_decoder_listener",
        ":string_compare",
        ":string_view",
    ],
)

cc_library(
    name = "request_decoder_listener",
    srcs = ["request_decoder_listener.cc"],
    hdrs = ["request_decoder_listener.h"],
    deps = [
        ":constants",
        ":hex_escape",
        ":logging",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "request_listener",
    hdrs = ["request_listener.h"],
    deps = [
        ":alpaca_request",
        ":constants",
        ":platform",
    ],
)

cc_library(
    name = "server_connection2",
    srcs = ["server_connection2.cc"],
    hdrs = ["server_connection2.h"],
    deps = [
        ":alpaca_request",
        ":alpaca_response",
        ":constants",
        ":literals",
        ":platform",
        ":platform_ethernet",
        ":request_decoder",
        ":request_listener",
        ":socket_listener",
        ":string_view",
    ],
)

cc_library(
    name = "server_description",
    srcs = ["server_description.cc"],
    hdrs = ["server_description.h"],
    deps = [
        ":json_encoder",
        ":literal",
        ":literals",
        ":platform",
        ":string_view",
    ],
)

cc_library(
    name = "server_socket_and_connection",
    srcs = ["server_socket_and_connection.cc"],
    hdrs = ["server_socket_and_connection.h"],
    deps = [
        ":platform",
        ":server_connection2",
        ":server_socket",
    ],
)

cc_library(
    name = "server_sockets_and_connections",
    srcs = ["server_sockets_and_connections.cc"],
    hdrs = ["server_sockets_and_connections.h"],
    deps = [
        ":platform",
        ":request_listener",
        ":server_socket_and_connection",
    ],
)

cc_library(
    name = "tiny_alpaca_server",
    srcs = ["tiny_alpaca_server.cc"],
    hdrs = ["tiny_alpaca_server.h"],
    deps = [
        ":alpaca_devices",
        ":alpaca_discovery_server",
        ":alpaca_response",
        ":array_view",
        ":constants",
        ":counting_bitbucket",
        ":device_impl_base",
        ":http_response_header",
        ":json_encoder",
        ":json_encoder_helpers",
        ":literals",
        ":platform",
        ":printable_cat",
        ":server_description",
        ":server_sockets_and_connections",
    ],
)
