# Ideas for a new approach to mapping the HTTP request path to a handler

I want to be able to express the mapping from a literal full path or a literal
path folder prefix to a RequestDecoderListener type. For example, assuming
suitable macros:

```
REQUEST_ROUTER(
  SomeName,

  // Matches "/" as the full path:
  RR_FULL_PATH(RootPathListener),

  // Matches "/index.html" as the full path.
  RR_FULL_PATH(RootPathListener, MCU_PSD("index.html")),

  // Matches "/setup" as the full path.
  RR_FULL_PATH(SetupPathListener, MCU_PSD("setup")),

  // Matches any path starting "/setup/v1/"
  RR_PATH_PREFIX(SetupSomeDeviceListener, MCU_PSD("setup"), MCU_PSD("v1")),
  RR_PATH_PREFIX(DevicePathListener, MCU_PSD("api"), MCU_PSD("v1")),

REQUEST_ROUTER_ENTRY(AssertPathListener)

);
```

Assuming it is hard to write the C++ templates to group together the entries
with a common prefix, and to produce a class for each path prefix level, a more
explicit approach might be better:

```
class RootPathListener : public RequestDecoderListener {...};
class SetupPathListener : public RequestDecoderListener {...};
class SetupSomeDeviceListener : public RequestDecoderListener {...};
class ManagementApiVersionsListener : public RequestDecoderListener {...};
class ManagementDescriptionListener : public RequestDecoderListener {...};
class ManagementConfiguredDevicesListener : public RequestDecoderListener {...};
class DeviceApiListener : public RequestDecoderListener {...};
class AssertPathListener : public RequestDecoderListener {...};
```

Assuming literal to type mapping templates, such as:

```
// Selected if PSD matches the segment.
template <typename LISTENER, typename PSD>
class PathSegment : public RequestDecoderListener {
 public:
  // What, if anything, should be passed to the ctor? Some shared context, which
  // might contain the HTTP method, among other things.
};
// Selected if PSD matches the final segment.
template <typename LISTENER, typename PSD>
class FinalPathSegment : public RequestDecoderListener {...};

template <template <typename> FirstT, template <typename> class... Ts>
struct PathSegmentMatchers {
  // Something for checking if FirstT is a match to the currently decoded
  // segment of the request path, else falling back to the base class method.
  // If a match, then constructs the overwrites the memory provided with
};
```

```
struct PathSegmentTable; // forward decl

// Path Segment to Info
struct PathSegmentInfo {
  enum class Type : uint8_t {
    // If kNextTable, then the union contains a pointer to another table.
    kNextTable,

    // Otherwise if contains a pointer to a function that replaces the current
    // RequestDecoderListener with a new one.
    kSetRequestDecoderListener,
  };

  ProgmemStringView path_segment;
  union {
    PathSegmentTable* next_table;
    size_t SetRequestDecoderListener(void* storage, size_t storage_size);
  };
  Type type;
};

```

Generate functions from a YAML file, such as:

```
root:
  end_listener: RootPathListener
  paths:
    - name: index.html
      end_listener: RootPathListener

    - name: setup




root:
  end_listener: RootPathListener
  paths:
    - name: index.html
      end_listener: RootPathListener

    - name: setup
```

Problem: YAML not supported natively by Python.

Assuming these listeners:

```
// Base listeners that extract and validate HTTP headers (e.g. Content-Type),
// and do the same for common parameters (ClientId, ClientTransactionId).
class GetJsonBaseListener : public RequestDecoderListener {...};
class PutFormReturnJsonBaseListener : public RequestDecoderListener {...};

// Concrete listeners.
class RootPathListener : public RequestDecoderListener {...};
class SetupPathListener : public RequestDecoderListener {...};
class SetupSomeDeviceListener : public RequestDecoderListener {...};
class ManagementApiVersionsListener : public GetJsonBaseListener {...};
class ManagementDescriptionListener : public GetJsonBaseListener {...};
class ManagementConfiguredDevicesListener : public GetJsonBaseListener {...};
class DeviceApiGetListener : public GetJsonBaseListener {...};
class DeviceApiPutListener : public PutFormReturnJsonBaseListener {...};
class AssestsPathListener : public RequestDecoderListener {...};
```

Generate functions from a JSON file, such as:

```
{
  # Whatever properties.

  "http_methods": {
    "GET": {
      # Whatever properties.
      "paths": [
        { "path": "/"
          "listener": "RootPathListener" },
        { "path": "/index.html"
          "listener": "RootPathListener" },
        { "path": "/setup"
          "listener": "SetupPathListener" },
        { "path": "/setup/v1/"
          "listener": "RootPathListener" },
        { "path": "/management/apiversions"
          "listener": "ManagementApiVersionsListener" },
        { "path": "/management/v1/description"
          "listener": "ManagementApiVersionsListener" },
        { "path": "/management/v1/configureddevices"
          "listener": "ManagementConfiguredDevicesListener" },
        { "path": "/api/v1/<device>/<devnum>/<get_method>"
          "listener": "RootPathListener" },
        { "path": "/"
          "listener": "RootPathListener" },
      ],
    },
    "PUT": {

    }
  },

  "url_slots": {
    "device": {
      # Whatever properties we want in order to specify how to map this to a
      # value added to a SerialMap<pair<Source, ProgmemString>> instance. Source
      # is an enum with values kUrl, kQueryParam, kHeaderField, kBodyParam. pair
      # is really a placeholder for a struct with fields Source source and
      # ProgmemString name, and with an operator==. For example:

      "type": "literal",
      "literals": [

      ],
      "more_literals":

    },
    "device": {
      # Whatever properties we want in order to specify how to map this to a
      # value added to a SerialMap<pair<Source, ProgmemString>> instance. Source
      # is an enum with values kUrl, kQueryParam, kHeaderField, kBodyParam. pair
      # is really a placeholder for a struct with fields Source source and
      # ProgmemString name, and with an operator==. For example:

      "type": "literal|positive_integer",
      ""

    }
  },
  "literal_tables": {
    "common_put_methods": [
      "action", "commandblind", "commandbool", "commandstring", "connected",
      "", "", ""
    ],
    "common_get_methods": [
      "action", "commandblind", "commandbool", "commandstring", "connected",
      "", "", ""
    ]

  },
  "literal_tables": {
    "common_put_methods": [
      "action", "commandblind", "commandbool", "commandstring", "connected",
      "", "", ""
    ]

  },

}
```

If a path (other than "/") ends with a slash, it only applies if the ... (what
was I thinking?)

NOTE: we could break up RequestDecoder listener into a MethodDecoder, a
PathDecoder and a HeaderRemainderDecoder (everything after the end of the path,
including the "?" or space at the end of the path). AND/OR we could arrange for
the existing RequestDecoder to always return between path segments, allowing the
caller to replace the listener.
