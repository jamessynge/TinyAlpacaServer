# Alpaca (Response) Encoder

Produces an HTTP Response Message Header and JSON encoded body conforming to the
ASCOM Alpaca REST API, with the goal of doing so without any memory allocation
being performed (except modest stack space).

The general idea is to have the ability to write the response payload twice,
first to a stream that counts the number of bytes, and second to the actual
output stream (e.g. TCP connection). This approach allows us to determine the
value of the Content-Length header prior to sending the payload to the client.

## JSON Representation

Alpaca responses are JSON encoded, with top-level object (aka dictionary), with
4 or 5 properties. There are 4 properties common to all responses:

*   ClientTransactionId (uint32)
*   ServerTransactionId (uint32)
*   ErrorNumber (int32)
*   ErrorMessage (string)

Some responses also include a Value property, whose value can have a variety of
types, including bool, string, integer, double and array of strings.
