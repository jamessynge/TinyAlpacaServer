# Notes on WIZ5500 and the Ethernet5500 library.

EthernetClient may be either client or server, seems unnecessary. Eliminating
the client portion could save us memory.

https://www.wiznet.io/product-item/w5500/ Datasheet:
http://wizwiki.net/wiki/lib/exe/fetch.php/products:w5500:w5500_ds_v109e.pdf

## WIZ5500 Socket Register Block

One register block per socket.

*   Sn_MR (aka SnMR)

    Socket n Mode Register (i.e. which protocol)

*   Sn_SR (aka SnSR)

    Socket n Status Register (i.e. which protocol)

Should introduce scoped enumerations with underlying type uint8_t, instead of
using the approach in Ethernet5500 (inherited from Ethernet3), where there are
classes for each of these collections of values.

## TODO

The API of the Arduino Ethernet libraries leaves a lot to be desired. For
example, EthernetClass and EthernetServer don't provide great control over the
way the individual sockets are used. I'd like to either find an existing API
that better suits the needs of writing a server that needs to handle both UDP
and TCP requests, and may also need to initiate some connections (e.g. to fetch
the current time).
