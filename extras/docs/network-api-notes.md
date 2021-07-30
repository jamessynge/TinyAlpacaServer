# Network API Notes

I started by using Ethernet3 for Tiny Alpaca Server, which has worked pretty
well, but has some issues:

*   We can't soft-reset the network chip until we've called Ethernet.begin,
    which can take a while. The reset is necessary because restarting the sketch
    (e.g. by uploading new code or hitting the reset button on the Arduino
    board) doesn't reset the network chip. We could resolve this with an
    initialization function that just tells the networking code where to find
    the chip, and does nothing else.

*   EthernetClient exposes too many methods (e.g. for creating new connections
    as a client). I hid these with ServerConnection, etc., but would like that
    in the base library.

*   The EthernetClient::write methods copy the data bytes into the chip's TX
    buffers, but also tell the chip to send the data. That should be under
    sender control (otherwise we send too many small packets).

*   (**My mistake**) I made too much use of the SnSR status values, and should
    probably have limited writing to when ESTABLISHED, and ignored CLOSE_WAIT
    (i.e. the TCP Half-Open state).

I'd like to design a better Hardware Abstraction Layer (HAL) for working with
TCP/IP offload chips, such as the W5500 that is on the Robotdyn Mega ETH board
that we're using. I need to start by studying existing ones beyond that in the
Ethernet3 library, especially those used in the latest version of the
[WIZnet ioLibrary](https://github.com/Wiznet/ioLibrary_Driver).
