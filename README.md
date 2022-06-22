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

The Arduino sketch using alpaca::TinyAlpacaServer configures the server instance
with device specific information, after which the server takes care of
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

## mcucore::Status

The basic server is complete, though the server and device setup methods are
just stubs (each device implementation will need to provide its own setup
methods). Furthermore, support for query parameters is limited (i.e. only
ClientID, ClientTransactionID, Connected and SensorName are supported).

Automated tests of the entire system aren't yet implemented.

There are TODOs throughout the code representing various improvements I'd like
to make, including modifying the guarantees provided by the MCU_CHECK macro: I
think it should be changed so that the expression is always evaluated,
regardless of whether a message is logged upon failure. The same doesn't apply
to the MCU_DCHECK macro.

## Approach

This code is targetted at settings (e.g. an Arduino) where dynamic allocation of
memory (e.g. filling a std::string) is not viable. Therefore RequestDecoder is
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
    EAscomMethod and/or EParameter, as appropriate.
1.  Add printing of the name of the constant in src/constants.cpp (see
    make_enum_to_string.py).
1.  Add the string to be matched (e.g. "camera") to src/literals.inc.
1.  Add matching of the string in the appropriate method in match_literals.cc.
1.  If adding a new device type Xyz, add a new class XyzAdapter to
    src/device_type_adapters, derived from DeviceImplBase, which maps decoded
    ASCOM requests that are specific to Xyz devices to calls to virtual methods
    of XyzAdapter which can be overridden by a subclass of XyzAdapter (i.e.
    FooBarXyz, which interfaces with a FooBar device that is of general type
    Xyz).

### JSON Representation

Most Alpaca responses are JSON encoded objects with 4 or 5 properties. There are
4 properties common to all responses:

*   ClientTransactionID (uint32)
*   ServerTransactionID (uint32)
*   ErrorNumber (int32)
*   ErrorMessage (string)

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

The Arduino Mega has only 8KB of ram, so we need to preserve that space as much
as possible. One way is to put literal (const) strings into flash (PROGMEM), and
use them when matching strings in requests (e.g. the HTTP method name) against
the set of expected values, and when producing responses that include fixed
strings.

Working with a string in PROGMEM gets a bit complicated because we can't
directly compare a string in RAM against a string in flash. Fortunately,
avr-libc includes
[`pgmspace` functions](https://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html)
for working with strings and arrays of bytes stored in PROGMEM, such as
memcpy_P, which copies a string from PROGMEM to RAM. Of special interest are:

*   strncasecmp_P, which performs a case insensitive comparison of a string in
    PROGMEM
*   memcmp_P, which performs byte comparison of an array of bytes in PROGMEM
    with an array of the same size in RAM.

Both of these functions have a parameter to indicate the length of the strings
(arrays).

These pages have some very interesting info about sections:

*   [Memory Sections](https://www.nongnu.org/avr-libc/user-manual/mem_sections.html)
*   [Maximum PROGMEM data size? - Arduino Mega Forum](https://forum.arduino.cc/t/maximum-progmem-data-size-arduino-mega/373448/12)

### ProgmemStringViews in Tiny Alpaca Server

**NOTE: This is out of date; the implementation has been moved to mcucore, so
this documentation needs updating.**

To simplify working with strings in PROGMEM, I've developed classes and macros
to allow for easily defining PROGMEM strings, for comparing and printing them
(where printing includes generating HTTP and JSON responses to Alpaca requests).
I'm currently manually defining these by:

1.  Adding appropriate entries to src/literals.inc, such as these:

    ```
    TAS_DEFINE_PROGMEM_LITERAL1(ClientTransactionID)
    TAS_DEFINE_PROGMEM_LITERAL(HttpRequestHeaderFieldsTooLarge,
                               "Request Header Fields Too Large")
    ```

    These macros define a PROGMEM char array holding the NUL terminated string,
    and a function (e.g. `ClientTransactionID()`) which returns an instance of
    the mcucore::Literal class pointing to that string, and also holding a
    member variable with the length of the string. The macros are actually
    expanded in multiple contexts: in the file `literals.h` where the function
    is declared, and in `literals.cc` where the char array and function are
    defined.

    Defining a literal in this file is appropriate if the string will be used in
    multiple files.

1.  Using the `TASLIT(value)` macro inline in expressions, such as:

    ```
    MCU_CHECK(false) << TASLIT("api group (") << group
                     << TASLIT(") is not device or setup");
    ```

    Unlike the above macros, this defines a full specialization of the variadic
    class template `ProgmemStringStorage`. The template declares a static
    function MakeProgmemStringView that returns a mcucore::ProgmemStringView
    instance. The storage class has a static array holding the characters of the
    string, without a terminating NUL. One advantage of TASLIT over
    TAS_DEFINE_LITERAL is that the compiler and linker should collapse multiple
    occurrences of TASLIT(x) with the same value of x. A downside of using
    TASLIT is that it uses some fancy compile time type deduction to determine
    the length of the string, and this slows compilation.

1.  Using the `MCU_FLASHSTR(value)` macro inline in expressions, such as:

    ```
    MCU_CHECK(false) << MCU_FLASHSTR("api group (") << group
                     << MCU_FLASHSTR(") is not device or setup");
    ```

    `MCU_FLASHSTR` shares much in common with TASLIT, but the return type is
    `const __FlashStringHelper*`, the type defined by Arduino to denote a string
    stored in flash memory, which (on Harvard architecture processors) can't be
    treated as a regular string. In general, it appears that `MCU_FLASHSTR` is
    the most convenient of these macros.

After building up the facilities described above (not including MCU_FLASHSTR), I
learned that the macro `PSTR(value)` (from AVR libc's pgmspace.h) will
(supposedly) have the effect of storing a string in PROGMEM only. Further, the
developers built on this by defining the macro `F(value)` in WString.h which
performs a typecast on a `PSTR(value)` to give it the type `const
__FlashStringHelper*`.

```
class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
```

This is good for printing a string, but not for comparing it with a
`mcucore::StringView`. Further, it doesn't capture the string length at compile
time, so it must be rediscovered each time.

When trying to use `F(value)` and `PSTR(value)`, I discovered that they do what
they say, but don't collapse multiple occurrences of a string into one. Hence
`MCU_FLASHSTR(value)` is preferred over `F(value)` if you just want to output a
string, and `TASLIT(value)` is preferred if you also want to know the length
without search the string for the terminating NUL.

NOTE: An additional problem with `F(str)` is that the symbol `F` is used in
existing libraries, so including both Arduino.h and those existing libraries can
easily lead to compilation failures or other errors. So, I've defined `FLASHSTR`
as a direct replacement of Arduino's `F` for those cases where that is what is
desired.

## Improvement Priorities

1.  DONE: High-Resolution PWM for CoverCalibrator.
1.  PARTIAL: Write Python Alpaca Client Library for the UDP and HTTP Protocols.
1.  Write a minimal "Conformance Test" in Python, that checks for valid and
    sensible responses to:

    *   Alpaca Discovery Protocol.
    *   Setup API
    *   Management API
    *   Common ASCOM methods applied to configured devices.
    *   Device specific GET requests

    Some thought will be needed to decide how to check the conformance of a
    server w.r.t. PUT requests (i.e. we don't want to damage anything).

1.  DONE: Figure out how to use a timer/counter and the watchdog timer to
    produce a better seed for randomness than the Arduino `rand()` function has.
    As things stand, all of the devices that I load these sketches on to are
    getting the same MAC address and default IP.

## Planning

*   Complete the ExtraParameters feature OR plumb unsupported parameters into
    the DeviceInterface impl of the device type. Without this methods such as
    /action or /commandstring will not work.

    NOTE: An alternate approach might be to define a DeviceApiActionInfo class,
    with instances generated from the YAML file, with info that tells us the set
    of expected and required parameters for each action, and the types of those
    parameters. To save RAM, these might be instantiated dynamically when a
    request of the approriate type arrives.

*   Add support for serving files from the SDcard, especially for paths under
    /setup and /assets, based on support to be added to McuCore.

*   MAYBE: Read the entirety of well-formed but unsupported requests (e.g. with
    parameters or headers that are too large), so that we don't *have* to close
    the connection, which will make testing easier.

*   MAYBE: Support "easy" extension of the HTTP decoder to support non-standard
    paths (e.g. POST /setserverlocation?value=Mauna+Kea). This could include
    support for requests such as GET /static/path-to/file-on/sd-card, where all
    /static/ requests result in reading from the SD Card on the Arduino, if
    available. If so, maybe generalize the decoder and move to repo McuNet.

*   MAYBE: Write a tool for gathering the literal definitions across the code
    base, and updating literals.inc accordingly.

    *   Related to that tool, consider how to recognize when literal strings can
        be split or combined such that multiple references to same character
        sequence in multiple locations can be shared.

*   To eliminate RAM consumed for alpaca::DeviceInfo objects, change the
    internal API for locating those to instead pass in a stack allocated
    DeviceInfo to be filled in when the info is needed.

*   Maybe store some attributes of the server and/or some device types (other
    than UUID) using mcucore::EepromTlv; for example, the location of the
    server, or the name of a switch.

*   DONE: the UniqueID using the mcucore::EepromTlv, with a separate
    mcucore::EepromDomain assigned to each device instance in the code.

*   DONE: Use mcucore::JitterRandom to seed the Arduino RNG, then generate
    values such as a MAC address, a link-local IP and, for each device, a
    Version 4 UUIDs (128 bits, of which about 122 are completely random), when
    such a device instance is first encountered by the server (i.e. on first run
    of the server on a particular board).

### Generate Device-Type Specific Code

The syntax of the ASCOM Alpaca Device API is specified in large part via a
[YAML](https://www.ascom-standards.org/api/AlpacaDeviceAPI_v1.yaml) file, and
hence is machine readable. This makes it reasonable to consider generating the
device-type specific code. Ideas:

*   Instead of having a single EDeviceMethod enum shared by all device types, we
    could manually or automatically define an ECommonDeviceApiMethod with the
    methods common to all device types, and then generate a EXyzDeviceApiMethod
    enum for each Xyz type of device, where the initial enumerator values are
    identical to the ECommonDeviceApiMethod enumerators.

*   Generate literal definitions in literals.inc (or in a
    device_types/{device_type}/ subdirectory)

*   Generate a XyzDeviceApiRequestParameters struct which provides storage for
    storing non-common parameters (e.g. the Axis and Rate parameters of a
    telescope/moveaxis request). These structs can be stored in a union within
    AlpacaRequest so as not to bloat memory requirements. Further, the
    DeviceApiBase could declare the methods for decoding and validating these
    fields, ala the existing RequestDecoderListener. Subclasses could then
    provide the implementation of those methods.

*   Maybe generate a DeviceInfo subtype for each device type, with the aim of
    automatically handling a number of requests with fixed responses (e.g.
    telescope/canpulseguide or camera/maxadu). The yaml file doesn't necessarily
    indicate which of these have fixed responses, so we might need to add an
    additional file with the list of such requests.

*   Generate a DeviceImplBase subclass, XyzAdapter, in the style of the manually
    written ObservingConditionsAdapter and CoverCalibratorAdapter.

    *   For each GET request whose JSON response has a Value field with a
        numeric or boolean value, we would generate a method returning the
        value, with a signature like:

        `mcucore::StatusOr<T> GetMethodName(const AlpacaRequest& request)`

    *   For each GET request returning another kind of value (e.g. Array of
        Integers or String), the generator would produce a method responsible
        for generating the value and producing the entire response, with a
        signature like:

        `bool HandleGetMethodName(const AlpacaRequest& request, Print& out)`

    *   For each PUT request, the generator would produce a method responsible
        for handling the action and producing the entire response, with a
        signature like:

        `bool HandlePutMethodName(const AlpacaRequest& request, Print& out)`

*   We might apply some of this thinking to all handling of the request after
    decoding the device type: i.e. the DeviceInterface could expose methods for
    receiving the device method name, parameter names and parameter values. If
    we don't have a handler for the device type or device type and device
    number, then we immediately return an HTTP error and close the connection.

*   The Alpaca specification uses double for floating point fields, which seems
    like overkill for quite a few fields (e.g humidity). Consider whether to
    just use double for all floating point fields, or whether to use some
    external data source to decide whether to represent some values (e.g.
    humidity and temperature) with a 32-bit float. The real question is what is
    the impact of just using double? Does it make the code much slower (since it
    is all implemented in software)?

    NOTE: According to
    [Arduino Reference](https://www.arduino.cc/reference/en/language/variables/data-types/double/),
    type double is mapped to the 4-byte float type on ATMEGA boards, so maybe it
    doesn't matter.

    NOTE: The Print class seems to only support printing of doubles, so even if
    we pass around floats, we still might not save much by using float instead
    of double on Arduino systems where float and double are different.

## Misc. Notes

*   I'm assuming that the responses are not large, in particular there are no
    image arrays; if you need to do that, you'll need a more capable computer
    than an Arduino Mega.

*   Given these small responses, I'm assuming that the networking chip has
    enough buffer space to handle the entire response (e.g. around 2KB in the
    case of the W5500), and therefore we don't need to design a system for
    incrementally returning responses.

*   IFF some DeviceInterface implementation needs to return large responses, it
    will need to generate the response incrementally (i.e. without any ability
    to store the whole thing in RAM) and provide custom response writing.

*   I'm inclined to think that the SafetyMonitor::IsSafe function should be
    implemented server side, not embedded, so that it can combine multiple
    signals and calibrated parameters to make the decision. The embedded system
    should provide the raw data, but probably not policy. This implies writing
    an Alpaca Server that runs on the server (host) and provides a SafetyMonitor
    device which in turn makes Alpaca requests to a device with the actual
    sensors.

*   The local clock on the device is fine for timing relatively short intervals
    (e.g. 10 minutes), so can be used for most rate and averaging computations.

*   It can be helpful for the device to know the wall clock time (i.e. current
    UTC or local time) for debug logs. If possible, it would be nice to get the
    time from the DHCP server. Look at:
    https://tools.ietf.org/id/draft-ogud-dhc-udp-time-option-00.html and related
    docs.

## Ultra Tiny HTTP Decoder?

The request decoder I've written is customized for just Alpaca. It is possible
that makes it slightly larger (in code) than if I had a generic decoder and an
Alpaca specific handler of decoding events (OnMethod, OnPathStart,
OnPathSegment, etc.). If this were to become an issue, it *might* be worth
considering this approach.
