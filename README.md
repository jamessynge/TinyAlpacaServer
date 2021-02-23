# [Tiny Alpaca Server](https://github.com/jamessynge/tiny-alpaca-server)

Author: James Synge (james.synge@gmail.com)

An HTTP request decoder (parser) and response formatter for the
[ASCOM Alpaca REST API](https://ascom-standards.org/api/), implemented in C++.
This may eventually be a more full fledged server (e.g. when combined with
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

*   Support case insensitive comparison of mixed case literals and mixed case
    allowed input (e.g. Content-Length or ClientTransactionId). That will allow
    us to have only one copy of the string in PROGMEM, and to emit JSON property
    with the correct case, and also to compare it it in a case insensitive
    manner with query parameter names.
*   Support the
    [management API](https://ascom-standards.org/api/?urls.primaryName=ASCOM%20Alpaca%20Management%20API),
    which will entail...
*   Add support for nested decoders, thus allowing each path segment to have its
    own decoder. Use to support paths starting /management and /setup.
*   Support multiple connections at once (a bounded number, e.g. as supported by
    the networking chip used by the RobotDyn MEGA 2560 ETH R3). This will
    require storing decoder state outside of the decoder instances. For example,
    using a union whereby all decoders can share the same memory.
