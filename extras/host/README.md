# About host_arduino

I find that I can develop software (think, code, compile/link, test/debug,
think) much faster by iterating on a host (Linux box, in my case), rather than
re-uploading to the target Arduino for each test/debug phase. Therefore I've
recreated (including copying of APIs) just enough of the Arduino API, and
relevant libraries (esp. Ethernet5500), to enable this during the development of
Tiny Alpaca Server.

I'm not using 'namespace alpaca' when declaring things here because I don't want
the calling code to be different for the host and for Arduino, where
sufficiently convenient.
