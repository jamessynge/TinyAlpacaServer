# [Tiny Alpaca Server](https://github.com/jamessynge/tiny-alpaca-server)

Author: James Synge (james.synge@gmail.com)

An HTTP request decoder (parser) and response formatter for the
[ASCOM Alpaca REST API](https://ascom-standards.org/api/)
([computer readable spec](https://www.ascom-standards.org/api/AlpacaDeviceAPI_v1.yaml)),
implemented in C++ and targetted at an Arduino with Ethernet (e.g. the
[Robotdyn MegaETH with POE](https://tinyurl.com/mega-eth-poe)).

This is growing into a full fledged server (e.g. when combined with
[SimpleHttpServer](https://github.com/jamessynge/arduino_experiments/blob/master/utilities/simple_http_server.h)).

## Approach

This code is targetted at settings where dynamic allocation of memory (e.g.
building std::maps) is not viable. Therefore the decoder is designed to need
only a relatively small amount of statically allocated memory: on an AVR, under
30 bytes for the RequestDecoder and Request instances, plus a caller provided
RequestDecoderListener (minimum size 2 bytes) and a data buffer to hold
fragments of the request as they arrive (32 bytes is a good size). In the
Arduino environment, it isn't practical to allocate any of these dynamically, so
pre-allocating all of them is the norm.

The same approach applies to encoding the response. For both PUT and GET Alpaca
requests, the response body is a JSON message with one top-level object with 4
standard properties, and another optional property of a variety of types. The
HTTP response headers need to contain a Content-Length header with the byte size
of the response. To avoid buffering the entire body, the JSON encoder supports
making two passes, the first counting the number of bytes (C++ chars) being
emitted, but otherwise doing nothing with those bytes, and a second pass that
actually emits the bytes.

## ASCOM Alpaca Feature Support

In order to limit the size of the program, the decoder can recognize a subset of
ASCOM defined device types, methods, and parameters. However, it is very
straightforward to add new ones. First edit decoder/constants.* and add the
appropriate enum to EDeviceType, EAscomMethod and/or EParameter, as appropriate.
Then edit decoder/tokens.h and the appropriate token(s) to the relevant array(s)
of tokens.

## Alpaca (Response) Encoder

Produces an HTTP Response Message Header and JSON encoded body conforming to the
ASCOM Alpaca REST API, with the goal of doing so without any memory allocation
being performed (except modest stack space).

The general idea is to have the ability to write the response payload twice,
first to a stream that counts the number of bytes, and second to the actual
output stream (e.g. TCP connection). This approach allows us to determine the
value of the Content-Length header prior to sending the payload to the client.

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

The aim is to support use from the Arduino IDE, though that is not yet complete.
I'm using the
[Library specification](https://arduino.github.io/arduino-cli/library-specification/)
in the arduino-cli documentation as a guide.

## RAM Preservation

The Arduino Mega has only 8KB of ram, so we need to preserve that space as much
as possible. One way is to put literal (const) strings into flash (PROGMEM).
This gets a bit complicated because we can't directly compare a string in RAM
against a string in flash. Fortunately, [avr-libc includes `pgmspace` functions
working with strings and arrays of bytes stored in
PROGMEM](https://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html),
such as memcpy_P, which copies a string from PROGMEM to RAM. Of special interest
are:

*   strncasecmp_P, which performs a case insensitive comparison of a string in
    PROGMEM
*   memcmp_P, which performs byte comparison of an array of bytes in PROGMEM
    with an array of the same size in RAM.

Both of these functions have a parameter to indicate the length of the strings
(arrays).

To simplify working with strings in PROGMEM, it would probably be best to wrap
them in a class that knows about `far pointers`, and provides operations for
comparing these with StringViews, and for streaming them out to a JSON Encoder
or a Print instance.

## Planning

*   DONE: Support case insensitive comparison of mixed case literals and mixed
    case allowed input (e.g. Content-Length or ClientTransactionId). That will
    allow us to have only one copy of the string in PROGMEM, and to emit JSON
    property with the correct case, and also to compare it it in a case
    insensitive manner with query parameter names.

*   DONE: Support the
    [management API](https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Management%20API).

*   MAYBE: Support "easy" extension of the HTTP decoder to support non-standard
    paths (e.g. POST /setserverlocation?value=Mauna Kea).

*   Support multiple connections at once (a bounded number, e.g. as supported by
    the networking chip used by the RobotDyn MEGA 2560 ETH R3). This will
    require storing decoder state outside of the decoder instances. For example,
    using a union whereby all decoders can share the same memory.

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

*   Start using subfolders of src/, for example:

    *   strings/ for StringView, TinyString, Literal (maybe renamed as
        LiteralString), AnyString, literals.inc, and the code that supports
        them.
    *   request_decoder/ for request_decoder*.
    *   json/ for the generic JSON encoder and supporting code.
    *   response_encoder/ for the HTTP response encoder, including Alpaca
        specific JSON support.

## Misc. Notes

*   I'm inclined to think that the SafetyMonitor::IsSafe function should be
    implemented server side, not embedded, so that it can combine multiple
    signals and calibrated parameters to make the decision. The embedded system
    should provide the raw data, but probably not policy.

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
