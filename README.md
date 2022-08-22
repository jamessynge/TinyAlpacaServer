# [Tiny Alpaca Server](https://github.com/jamessynge/TinyAlpacaServer)

Author: James Synge (james.synge@gmail.com)

Tiny Alpaca Server is an Arduino library supporting the creation of ASCOM Alpaca
compatible devices. In particular, the library implements Alpaca's
[Discovery Protocol](https://github.com/DanielVanNoord/AlpacaDiscoveryTests#specification),
[Management API](https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Management%20API)
([computer readable spec](https://www.ascom-standards.org/api/AlpacaManagementAPI_v1.yaml)),
and
[Device API](https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Device%20API)
([computer readable spec](https://www.ascom-standards.org/api/AlpacaDeviceAPI_v1.yaml)).

The Arduino sketch using `alpaca::TinyAlpacaServer` configures the server
instance with device specific information, after which the server takes care of
responding to requests and delegating requests for information or action to
device specific code provided by the sketch. See
[TinyAlpacaServerDemo](examples/TinyAlpacaServerDemo/TinyAlpacaServerDemo.ino)
for an example of how to use the server.

This library has been developed using the Ethernet3 library, which supports the
[WIZnet W5500](https://www.wiznet.io/product-item/w5500/) networking offload
chip
([datasheet](http://wizwiki.net/wiki/lib/exe/fetch.php/products:w5500:w5500_ds_v109e.pdf)),
which is present on the
[Robotdyn MEGA 2560 ETH with POE](https://robotdyn.com/catalogsearch/result/?q=GR-00000039),
a board compatible with the Arduino Mega 2560, but with built in Ethernet,
including Power-over-Ethernet. I've permanently forked Ethernet3 as Ethernet5500
in order to allow for fixing some API flaws that Ethernet3 inherited from the
Ethernet2.

## Status

Used now for:

*   A Cover Calibrator, with a Switch device to control the LEDs that are turned
    on when the calibrator is turned on.
*   A small [Weather Station](https://github.com/AAVSO/AM_WeatherBoxMega),
    focused really on observing and observatory safety; i.e. is it raining or
    cloudy (close the observatory!), or is the sky clear?

There is not yet support for reading static files from an SD Card. There are
many unit tests, but there are not yet integration tests of the entire server
with mock devices.

## Approach

This code is targeted at settings (e.g. an Arduino) where dynamic allocation of
memory (e.g. filling a std::string) is not viable. Therefore `RequestDecoder` is
designed to need only a relatively small amount of statically allocated memory.

The same approach applies to encoding the response. For both PUT and GET Alpaca
requests, the response body is usually a JSON message with one top-level object
with 4 standard properties, and another optional property of a variety of types.
The HTTP response headers need to contain a Content-Length header with the byte
size of the response. To avoid buffering the entire body, the JSON encoder
supports making two passes, the first counting the number of bytes (ASCII
characters) being emitted, but otherwise doing nothing with those bytes, and a
second pass that actually emits the bytes.

## ASCOM Alpaca Feature Support

In order to limit the size of the program, the decoder can recognize a subset of
ASCOM defined device types, methods, and parameters. However, it is very
straightforward to add new ones:

1.  Add the appropriate enumerators in src/constants.h to EDeviceType,
    `EAscomMethod` and/or `EParameter`, as appropriate.
1.  Add printing of the name of the constant in src/constants.cpp (see
    make_enum_to_string.py).
1.  Add the string to be matched (e.g. "camera") to src/literals.inc.
1.  Add matching of the string in the appropriate method in match_literals.cc.
1.  If adding a new device type `Xyz`, add a new class `XyzAdapter` to
    src/device_type_adapters, derived from `DeviceImplBase`, which maps decoded
    ASCOM requests that are specific to `Xyz` devices to calls to virtual
    methods of `XyzAdapter` which can be overridden by a subclass of
    `XyzAdapter` (i.e. `FooBarXyz`, which interfaces with a `FooBar` device that
    is of general type `Xyz`).

### JSON Representation

Most Alpaca responses are JSON encoded objects with 4 or 5 properties. There are
4 properties common to all responses:

*   `ClientTransactionID` (uint32)
*   `ServerTransactionID` (uint32)
*   `ErrorNumber` (int32)
*   `ErrorMessage` (string)

Some responses also include a Value property, whose value can have a variety of
types, including bool, string, integer, double and array of strings.

## Arduino Support

Tiny Alpaca Server is structured as an Arduino Library, following the Arduino
[Arduino Library specification](https://arduino.github.io/arduino-cli/library-specification/).
To make debugging easier (for me, at least), I've defined logging macros in
`src/utils/logging.h` that are similar to those in the Google Logging Library.
On the Arduino, these are disabled by default, in which case they do not
contribute to the code or RAM size of the compiled Arduino sketch. They can be
enabled by editing `src/utils/utils_config.h` (sorry, Arduino IDE doesn't offer
a better approach).

## RAM Preservation

To simplify working with strings in PROGMEM, I've developed classes and macros
to allow for easily defining PROGMEM strings, for comparing and printing them
(where printing includes generating HTTP and JSON responses to Alpaca requests).

In Tiny Alpaca Server, many literals are defined in src/literals.inc, such as:

```
  TAS_DEFINE_PROGMEM_LITERAL1(ClientTransactionID)
  TAS_DEFINE_PROGMEM_LITERAL(HttpRequestHeaderFieldsTooLarge,
                             "Request Header Fields Too Large")
```

These macros define a PROGMEM char array holding the NUL terminated string, and
a function (e.g. `ClientTransactionID()`) which returns an instance of the
`mcucore::ProgmemString` class pointing to that string. The macros are actually
expanded in multiple contexts: in the file `literals.h` where the function is
declared, and in `literals.cc` where the char array and function are defined.

Defining a literal in this file most appropriate if the string will be used in
multiple files. However, it is possible to instead use the underlying McuCore
features, such as `MCU_FLASHSTR`, `MCU_PSD`, and `MCU_PSV`, all of which provide
the sharing of strings stored in Flash memory. See
[String Literals in Arduino Sketches](https://github.com/jamessynge/mcucore/blob/main/extras/docs/ram-preservation-and-string-literals.md)
for more info.

## Misc. Notes

*   I'm assuming that the responses are not large, in particular there are no
    image arrays; if you need to do that, you'll need a more capable "computer"
    than an Arduino Mega.

*   Given these small responses, I'm assuming that the networking chip has
    enough buffer space to handle the entire response (e.g. around 2KB in the
    case of the W5500), and therefore we don't need to design a system for
    incrementally returning responses.

*   IFF some `DeviceInterface` implementation needs to return large responses,
    it will need to generate the response incrementally (i.e. without any
    ability to store the whole thing in RAM) and provide custom response
    writing.

*   I'm inclined to think that the `SafetyMonitor::IsSafe` function should be
    implemented server side, not embedded, so that it can combine multiple
    signals and calibrated parameters to make the decision. The embedded system
    should provide the raw data, but probably not policy. This implies writing
    an Alpaca Server that runs on the server (host) and provides a
    `SafetyMonitor` device which in turn makes Alpaca requests to a device with
    the actual sensors.

*   The local clock on the device is fine for timing relatively short intervals
    (e.g. 10 minutes), so can be used for most rate and averaging computations.

*   It can be helpful for the device to know the wall clock time (i.e. current
    UTC or local time) for debug logs. If possible, it would be nice to get the
    time from the DHCP server. Look at:
    https://tools.ietf.org/id/draft-ogud-dhc-udp-time-option-00.html and related
    docs.
