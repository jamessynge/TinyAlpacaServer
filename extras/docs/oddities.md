# Oddities: Things Seen That Need Explaining

## SetSwitch May Not Return

While running cover_calibrator.py to sweep individual LED channels, at one point
the SetLedChannelEnabled ran successfully, but ServerConnection::OnCanRead did
not log the final status_code that should have been produced after AlpacaDevices
dispatched to the switch device.

## Ctrl-C of Client Script Sometimes Doesn't Close Connection

I expect that Ctrl-C will close all open file descriptors, including TCP
sockets, yet I've seen at least one case where Tiny Alpaca Server did not act as
if it had learned of any change to the connection. It is possible that a
half-close or full-close was received but the server code didn't handle it
properly.
