# Tiny Alpaca Server - Test Planning

This document explores how to test Tiny Alpaca Server, and its components. It is
not yet a concrete test plan.

## Unit Testing

Where feasible there should be hermetic unit tests which run outside of the
embedded system (e.g. on a laptop or build server); for example,
string_view_test.cc and alpaca_response_test.cc. Coverage reports can help to
determine the extent of line coverage, though not all lines need to be tested,
and 100% line and branch coverage isn't proof that the code is bug free.

Unit tests should be focused on the public API of the component being tested,
not on the internal state.

Some components, especially those that act as hardware device drivers, are
difficult to test without direct access to that hardware; it is possible to
write device emulators or to mock their interfaces, but doing so tends to be
very time consuming, so the payoff needs to be substantial. For now I'm assuming
that code which does not need direct access to hardware should have unit tests,
and other tests will primarily be conducted either via specialized test sketches
(e.g. `examples/CoverCalibrator`), or via remote tests.

## Hardware Validation Testing

Validating that a device driver works correctly with its target hardware (e.g. a
Timer/Counter peripheral embedded in the AVR MCU, or the W5500 network interface
chip) is best done (or at least best done initially) in isolation from the rest
of the software, especially drivers for other devices. The only exception would
be device drivers that need to share some common hardware feature, such as an
SPI interface.

To that end, I've written several sketches located in the `examples/` folder as
part of learning how to use specific AVR features, and how to work with other
off-chip peripherals. For example:

*   `HighResPwmExperiment.ino` was used to understand how to use the 16-bit
    Timer/Counter features of the ATmega2560, as part of learning how to
    generate PWM signals for the LED outputs.
*   `TestIpDeviceSetup.ino.cc` is a test of `alpaca::IpDevice`,
    `alpaca::Mega2560Eth` and the `Ethernet3` library, just to the extent of
    setting up the W5500 chip with multiple TCP sockets.

## Remote Alpaca Discovery API Testing

The
[Alpaca Discovery Protocol](https://github.com/DanielVanNoord/AlpacaDiscoveryTests#specification)
is a UDP broadcast protocol for learning about the set of Alpaca servers on the
local network. Repeated broadcasts should result in the same Alpaca servers
responding again and again. We may want to verify that the same servers respond
each time we send a broadcast request.

## Alpaca Remote HTTP Testing

The full Tiny Alpaca Server needs to be tested via HTTP requests in the form of
the ASCOM Alpaca protocol. Most successful requests have JSON objects in the
response body (`content-type` `application/json`), though a few have natural
language text messages as their response bodies (`content-type` `text/plain`).

The API specifications (YAML files) include schemas that list the properties to
be expected in the responses, and sometimes their value values.

## Validating JSON Responses

The `ServerTransactionID` property should be a strictly increasing integer,
though there are probably some cases where multiple overlapping requests could
see the values out of order. For any one HTTP session, they must be strictly
increasing.

The `ClientTransactionId` property must match the id provided by the user IFF a
valid value was provided by the client. If the client failed to provide a value,
the the property should not be present in the response. If the client provided
an invalid value (e.g. not a uint32), then an error should be returned.

Most JSON response schemas include `ErrorNumber` and `ErrorMessage` properties.
For those schemas, we should validate that for a successful request their values
are `0` and `""` (empty string), respectively.

## Alpaca Management API

[API Specification](https://www.ascom-standards.org/api/AlpacaManagementAPI_v1.yaml)

This is a very narrow API, with only three requests starting `/management/`. The
Value field in the response should be the same each time that a server instance
responds; i.e., it may change if the Alpaca server restarts, but we don't
otherwise expect the results to change. The main thing to check is that the
`/apiversions` response specifies only version `1`,

NOTE: There is an error in the `/management/v1/description` specification. It
lists the response type as being a plain text error message, but it should be a
JSON object matching the DescriptionResponse schema.

## Alpaca Device API

[API Specification](https://www.ascom-standards.org/api/AlpacaDeviceAPI_v1.yaml)

Only those devices reported by the `/management/v1/configureddevices` should be
present. For example, we should be able to request the name of devices 0, 1 and
2 of all known device types (e.g. `telescope`), and get an error for all but
those that appear in the `configureddevices` response.

There are methods common to all devices types, including 7 that are read-only.
We should be able to query those repeatedly, and except for the `connected`
state, we should get the same answer each time.

## Alpaca Setup API

The `/setup` path and `/setup/v1/{device_type}/{device_number}/setup` paths
should return files from the SDcard. This means that the person or system
loading the software and corresponding SDcard needs to ensure compatibility
between the `configureddevices` response and the contents of the SDcard.

### Serving Static Assets

TODO(jamessynge): Add support for serving static assets (files) from the the
SDcard. In particular, all paths under /setup and /assets should come from the
SDcard. Non-normalized paths should be rejected (e.g. those with `//`, `/../`,
or `/./`).
