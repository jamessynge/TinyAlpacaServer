# Tiny Alpaca Server

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
