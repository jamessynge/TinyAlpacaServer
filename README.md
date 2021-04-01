# [Tiny Alpaca Server](https://github.com/jamessynge/tiny-alpaca-server)

Author: James Synge (james.synge@gmail.com)

Tiny Alpaca Server is an Arduino library implementing the
[ASCOM Alpaca REST API](https://ascom-standards.org/api/)
([computer readable spec](https://www.ascom-standards.org/api/AlpacaDeviceAPI_v1.yaml)),
and the
[Alpaca Discovery Protocol](https://github.com/DanielVanNoord/AlpacaDiscoveryTests#specification).
It is intended that the main Arduino sketch configures the server with device
specific information, after which Tiny Alpaca Server takes care of responding to
requests and delegating requests for information or action to device specific
code provided by the sketch.

This library has been developed using the Ethernet3 library, which supports the
[WIZnet W5500](https://www.wiznet.io/product-item/w5500/) networking offload
chip
([datasheet](http://wizwiki.net/wiki/lib/exe/fetch.php/products:w5500:w5500_ds_v109e.pdf)),
which is present on the
[Robotdyn MEGA 2560 ETH with POE](https://robotdyn.com/catalogsearch/result/?q=GR-00000039),
a board compatible with the Arduino Mega 2560, but with built in Ethernet,
including Power-over-Ethernet.

## Approach

This code is targetted at settings where dynamic allocation of memory (e.g.
filling a std::string) is not viable. Therefore the request decoder is designed
to need only a relatively small amount of statically allocated memory: on an
AVR, under 30 bytes of RAM for the RequestDecoder and Request instances, plus a
caller provided RequestDecoderListener (minimum size 2 bytes) and a data buffer
to hold fragments of the request as they arrive (32 bytes is a good size). In
the Arduino environment, it isn't practical to allocate any of these
dynamically, so pre-allocating all of them is the norm.

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
1.  Add printing of the name of the constant in src/constants.cpp.
1.  Add the string to be matched (e.g. "camera") to src/literals.inc.
1.  Add matching of the string in the appropriate method in match_literals.cc.
1.  If adding a new device type Xyz, add a new class XyzAdapter to
    src/device_type_adapters, derived from DeviceApiHandlerBase, which maps
    decoded ASCOM requests that are specific to Xyz devices to calls to virtual
    methods of XyzAdapter which can be overridden by a subclass of XyzAdapter
    (i.e. FooBarXyz, which interfaces with a FooBar device that is of general
    type Xyz).

### JSON Representation

Alpaca responses are JSON encoded, with top-level object (aka dictionary), with
4 or 5 properties. There are 4 properties common to all responses:

*   ClientTransactionId (uint32)
*   ServerTransactionId (uint32)
*   ErrorNumber (int32)
*   ErrorMessage (string)

Some responses also include a Value property, whose value can have a variety of
types, including bool, string, integer, double and array of strings.

## Arduino Support

Tiny Alpaca Server is structured as an Arduino Library, following the Arduino
[Arduino Library specification](https://arduino.github.io/arduino-cli/library-specification/).
To make debugging easier (for me, at least), I've defined logging macros in
`src/utils/logging.h` that are similar to those in the Google Logging Library.
On the Arduino, these are disabled by default, in which case they should not
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

To simplify working with strings in PROGMEM, I've defined a class `Literal` in
`src/utils/literal.h` and macros to allow for easily defining string literals
that we want stored in PROGMEM and not in RAM.

Note: So far, class `Literal` only supports strings stored in the first 64KB of
PROGMEM.

## Planning

*   MAYBE: Support "easy" extension of the HTTP decoder to support non-standard
    paths (e.g. POST /setserverlocation?value=Mauna Kea).

*   If useful, write a tool to generate / update literals.inc based on
    DEFINE_LITERAL occurrences in source files, maybe even flag string literals
    that are in source files and not expressed as calls to Literals::FooBar().
    Consider using the
    [Shortest Superstring Greedy Approximate Algorithm](https://www.geeksforgeeks.org/shortest-superstring-problem/)
    to produce a single long literal string without extra NUL characters, and a
    table of pairs <offset, length>, with the offset being the start of a
    substring (i.e. a literal defined with DEFINE_LITERAL) within the
    superstring, and length being the number of characters in the substring. If
    we generate a dense enum starting at zero for the keywords, then we can use
    those enums to build PROGMEM tables of allowed tokens for use when decoding
    the path.

*   Look into writing a program (Python script?) that reads the
    [Alpaca Device API specification](https://www.ascom-standards.org/api/AlpacaDeviceAPI_v1.yaml)
    the
    [Alpaca Management API specification](https://www.ascom-standards.org/api/AlpacaManagementAPI_v1.yaml),
    a set of device types to support, and a set of device methods to exclude,
    and then emits a bunch of tables to drive a "generic" Alpaca HTTP request
    decoder; this might also emit code for response builders. The script would
    also generate or update literals.inc, the set of strings to be stored in
    PROGMEM.

*   Consider splitting src/utils out into its own Arduino library, or at least
    those parts that are truly not related to Alpaca.

*   DONE: Support case insensitive comparison of mixed case literals and mixed
    case allowed input (e.g. Content-Length or ClientTransactionId). That will
    allow us to have only one copy of the string in PROGMEM, and to emit JSON
    property with the correct case, and also to compare it it in a case
    insensitive manner with query parameter names.

*   DONE: Support the
    [management API](https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Management%20API).

*   DONE: Support multiple connections at once (a bounded number, e.g. as
    supported by the networking chip used by the RobotDyn MEGA 2560 ETH R3).

*   DONE: Implement a form of
    [Duck Typing](http://p-nand-q.com/programming/cplusplus/duck_typing_and_templates.html),
    where we can stream (print) any class or struct that has a printTo(Print&)
    method, or any type T for which a PrintValueTo(const T, Print&) function
    exists.

*   DONE: Implement a tool (`make_enum_to_string.py`) for generating methods for
    printing the names of enum values (e.g. EParameter::kClientId prints as
    "kClientId").

## Misc. Notes

*   I'm assuming that the networking chip has enough buffer space to handle the
    entire response (probably a few hundred bytes, assuming no image arrays),
    and therefore we don't need to design a system for incrementally returning
    responses.

*   I'm inclined to think that the SafetyMonitor::IsSafe function should be
    implemented server side, not embedded, so that it can combine multiple
    signals and calibrated parameters to make the decision. The embedded system
    should provide the raw data, but probably not policy. This implies writing
    an Alpaca Server that runs on the server (host) and provides a SafetyMonitor
    device which in turn makes Alpaca requests to a device with the actual
    sensors.

*   It can be helpful for the device to know the time (i.e. current UTC or local
    time), such as for implementing operations involving rates or periods. The
    local clock on the device is unlikely to be very accurate, nor is it likely
    to keep running when the power is off. Look into learning the time at boot,
    and possibly times thereafter. Look at:
    https://tools.ietf.org/id/draft-ogud-dhc-udp-time-option-00.html and at
    related docs.

*   For reading and writing from EEPROM, it may be useful to use an approach
    such as:

    *   All "things" that want to use EEPROM are registered with a single EEPROM
        manager.
    *   Each registrant has a unique Tag (e.g. a semi-random uint16) that is
        used when reading to identify the owner of the data.
    *   Store data in EEPROM using TLV format (Tag, Length, Value).
    *   When writing to EEPROM, re-write all data so that we avoid the need to
        defragment it.
    *   Consider using a semi-spaces approach, where we write from the bottom of
        EEPROM on one write pass, and next time we need to update it we write
        from the top (i.e. backwards). The purpose is to reduce the risk of
        losing data if writing is interrupted.
    *   We might need some temporary storage in RAM during writing, for which we
        could use an Arduino String class, which has a reserve method.

## Ultra Tiny HTTP Decoder?

The request decoder I've written is customized for Alpaca. It is possible that
makes it slightly larger (in code) than if I had a generic decoder and an Alpaca
specific handler of decoding events (OnMethod, OnPathStart, OnPathSegment,
etc.). If this were to become an issue, it *might* be worth considering this
approach.
