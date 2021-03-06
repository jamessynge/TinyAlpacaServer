# Provides adapters for specific device types that convert generic
# HandleGetRequest or HandlePutRequest calls into a specific virtual function
# calls (e.g. double GetAveragePeriod()).

cc_library(
    name = "observing_conditions_adapter",
    hdrs = ["observing_conditions_adapter.h"],
    deps = [
        ":device_api_handler_base",
        ":platform",
        ":status_or",
    ],
)
