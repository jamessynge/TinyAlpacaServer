# Planning / Improvement Priorities

*This applies to
[TinyAlpacaServer](https://github.com/jamessynge/TinyAlpacaServer),
[McuNet](https://github.com/jamessynge/mcunet) and
[McuCore](https://github.com/jamessynge/mcucore).*

## #1 Priority: Serve Files From SD Card

We desire to serve files (e.g. HTML, CSS, Image and Javascript files) to be used
for providing a rich UI, such as:

*   Viewing and editing fields of the `ServerDescription`; for example, the
    Latitude and Longitude where it is located.
*   Viewing and editing fields of each of the `DeviceDescription` objects; for
    example, a device's Name or Description.
*   Viewing the properties of a device; for example, the sensor values of an
    Observing Conditions device.
*   Controlling a device; for example, triggering a Cover Calibrator to close,
    or setting the value of a Switch.

A Robotdyn Mega ETH board has an SD Card socket from which we can read files,
which gives us much more storage than if we tried to store that in Flash. We
plan to use that SD Card feature as the source for the files mentioned above.
Thus we need to extend the HTTP serving capability to support serving
*arbitrary* files from the SD Card.

These tasks help us achieve this goal:

1.  Finish developing the means of using `mcunet::http1::RequestDecoder` and
    `mcucore::SerialMap<T>` for decoding "arbitrary" HTTP/1.1 requests. In
    particular, design and implement the means by which different handlers are
    selected for different paths. This *might* mean creating a subclass of
    `mcunet::http1::RequestDecoderListener` which has a function pointer for
    handling the decoded path segments, where that function pointer can be
    replaced each time a segment is decoded.

    > *Consider* ***moving*** *the request decoder to McuCore, given that it has
    > no actual network access; it is just delivered bytes to be analyzed.*

1.  Switch to `mcunet::http1::RequestDecoder` (from `alpaca::RequestDecoder`),
    in order to support more paths, and more parameters.

1.  Develop support for reading and serving files from the SD Card, especially
    for paths under /setup and /assets. *Based on a reading of the API of the
    Arduino libraries for using the SD Card, it may be necessary to prevent
    multiple simultaneous requests from accessing the SD Card.*

## Lower Priorities

1.  Support setting/editing server attributes (e.g. location) and storing those
    in EEPROM using `mcucore::EepromTlv`. {value=4}
1.  Support setting/editing common device attributes (e.g. name) and storing
    those in EEPROM using `mcucore::EepromTlv`.

1.  Support setting/editing unique device attributes (e.g. switch name) and
    storing those in EEPROM using `mcucore::EepromTlv`.

1.  Develop the host `mcunet::PlatformNetwork` implementation such that we can
    run an Alpaca server for full host-based testing of TinyAlpacaServer.

1.  Get around to publishing Arduino libraries so that others can more easily
    make use of the code (i.e. from the Arduino IDE). It appears that I should
    be able to do this "privately" by providing a file (via github raw?) which
    is read by the Arduino IDE as a kind of index. That avoids (or delays) the
    need to formally publish the libraries.

1.  Convert my Alpaca client code, written in Python, to be based on
    [Bob Denny's Alpyca library](https://github.com/ASCOMInitiative/alpyca).

1.  Write a minimal "Conformance Test" in Python, that checks for valid and
    sensible responses to:

    *   Alpaca Discovery Protocol.
    *   Setup API
    *   Management API
    *   Common ASCOM methods applied to configured devices.
    *   Device specific GET requests

    Some thought will be needed to decide how to check the conformance of a
    server w.r.t. PUT requests (i.e. we don't want to damage anything).

## Partially Complete / In-Progress

## Candidate Tasks

*   Read the entirety of well-formed but unsupported requests (e.g. with
    parameters or headers that are too large), so that we don't *have* to close
    the connection, which may make testing easier.

*   Write a tool for gathering the literal definitions across the code base, and
    updating literals.inc accordingly.

    *   Related to that tool, consider how to recognize when literal strings can
        be split or combined such that multiple references to same character
        sequence in multiple locations can be shared.

*   If the amount of RAM dedicated to `ServerDescription` or `DeviceDescription`
    objects becomes an issue, consider changing the API so that the data is
    stored (almost) entirely in Flash; the exception would be the device number
    and device type, which can be returned by the `DeviceInterface` impl.

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

*   Maybe generate a DeviceDescription subtype for each device type, with the
    aim of automatically handling a number of requests with fixed responses
    (e.g. telescope/canpulseguide or camera/maxadu). The yaml file doesn't
    necessarily indicate which of these have fixed responses, so we might need
    to add an additional file with the list of such requests.

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

## Completed Tasks

1.  DONE: High-Resolution PWM for CoverCalibrator.

1.  DONE: Figure out how to use a timer/counter and the watchdog timer to
    produce a better seed for randomness than the Arduino `rand()` function has.
    As things stand, all of the devices that I load these sketches on to are
    getting the same MAC address and default IP.

1.  DONE: Generate an HTML status page for requests to path `/`, with support
    from each device instance (i.e. to fill in a block of html). To avoid having
    to generate the value for the Content-Length header, which requires two
    passes of generating the body of the HTTP response, I could use chunked
    encoding or similar.

*   DONE: Store the UniqueID using the mcucore::EepromTlv, with a separate
    mcucore::EepromDomain assigned to each device instance in the code.

*   DONE: Use mcucore::JitterRandom to seed the Arduino RNG, then generate
    values such as a MAC address, a link-local IP and, for each device, a
    Version 4 UUIDs (128 bits, of which about 122 are completely random), when
    such a device instance is first encountered by the server (i.e. on first run
    of the server on a particular board).
