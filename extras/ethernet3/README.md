# Notes on WIZ5500 and the sstaub/Ethernet3 library.

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
using the approach in Ethernet3, where there are classes for each of these
collections of values.
