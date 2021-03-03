#ifndef TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_STREAM_H_
#define TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_STREAM_H_

// TODO(jamessynge): Describe why this file exists/what it provides.
// Declares the subset of Arduino's Stream needed for Tiny Alpaca Server.

#include "extras/host_arduino/print.h"

class Stream : public Print {
 protected:
  // unsigned long _timeout;  // number of milliseconds to wait for the next
  // char
  //                          // before aborting timed read
  // unsigned long _startMillis;  // used for timeout measurement
  // int timedRead();             // read stream with timeout
  // int timedPeek();             // peek stream with timeout
  // int peekNextDigit(LookaheadMode lookahead,
  //                   bool detectDecimal);  // returns the next numeric digit
  //                   in
  //                                         // the stream or -1 if timeout

 public:
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int peek() = 0;

  // Stream() { _timeout = 1000; }

  // parsing methods

  // void setTimeout(
  //     unsigned long timeout);  // sets maximum milliseconds to wait for
  //     stream
  //                              // data, default is 1 second
  // unsigned long getTimeout(void) { return _timeout; }

  //   bool find(char *target);  // reads data from the stream until the target
  //                             // string is found
  //   bool find(uint8_t *target) { return find((char *)target); }
  //   // returns true if target string is found, false if timed out (see
  //   setTimeout)

  //   bool find(char *target,
  //             size_t length);  // reads data from the stream until the target
  //                              // string of given length is found
  //   bool find(uint8_t *target, size_t length) {
  //     return find((char *)target, length);
  //   }
  //   // returns true if target string is found, false if timed out

  //   bool find(char target) { return find(&target, 1); }

  //   bool findUntil(char *target,
  //                  char *terminator);  // as find but search ends if the
  //                                      // terminator string is found
  //   bool findUntil(uint8_t *target, char *terminator) {
  //     return findUntil((char *)target, terminator);
  //   }

  //   bool findUntil(char *target, size_t targetLen, char *terminate,
  //                  size_t termLen);  // as above but search ends if the
  //                  terminate
  //                                    // string is found
  //   bool findUntil(uint8_t *target, size_t targetLen, char *terminate,
  //                  size_t termLen) {
  //     return findUntil((char *)target, targetLen, terminate, termLen);
  //   }

  //   long parseInt(LookaheadMode lookahead = SKIP_ALL,
  //                 char ignore = NO_IGNORE_CHAR);
  //   // returns the first valid (long) integer value from the current
  //   position.
  //   // lookahead determines how parseInt looks ahead in the stream.
  //   // See LookaheadMode enumeration at the top of the file.
  //   // Lookahead is terminated by the first character that is not a valid
  //   part of
  //   // an integer. Once parsing commences, 'ignore' will be skipped in the
  //   stream.

  //   float parseFloat(LookaheadMode lookahead = SKIP_ALL,
  //                    char ignore = NO_IGNORE_CHAR);
  //   // float version of parseInt

  //   size_t readBytes(char *buffer,
  //                    size_t length);  // read chars from stream into buffer
  //   size_t readBytes(uint8_t *buffer, size_t length) {
  //     return readBytes((char *)buffer, length);
  //   }
  //   // terminates if length characters have been read or timeout (see
  //   setTimeout)
  //   // returns the number of characters placed in the buffer (0 means no
  //   valid
  //   // data found)

  //   size_t readBytesUntil(
  //       char terminator, char *buffer,
  //       size_t length);  // as readBytes with terminator character
  //   size_t readBytesUntil(char terminator, uint8_t *buffer, size_t length) {
  //     return readBytesUntil(terminator, (char *)buffer, length);
  //   }
  //   // terminates if length characters have been read, timeout, or if the
  //   // terminator character  detected returns the number of characters placed
  //   in
  //   // the buffer (0 means no valid data found)

  //   // Arduino String functions to be added here
  //   String readString();
  //   String readStringUntil(char terminator);

  //  protected:
  //   long parseInt(char ignore) { return parseInt(SKIP_ALL, ignore); }
  //   float parseFloat(char ignore) { return parseFloat(SKIP_ALL, ignore); }
  //   // These overload exists for compatibility with any class that has
  //   derived
  //   // Stream and used parseFloat/Int with a custom ignore character. To keep
  //   // the public API simple, these overload remains protected.

  //   struct MultiTarget {
  //     const char *str;  // string you're searching for
  //     size_t len;       // length of string you're searching for
  //     size_t index;     // index used by the search routine.
  //   };

  //   // This allows you to search for an arbitrary number of strings.
  //   // Returns index of the target that is found first or -1 if timeout
  //   occurs. int findMulti(struct MultiTarget *targets, int tCount);
};

#endif  // TINY_ALPACA_SERVER_EXTRAS_HOST_ARDUINO_STREAM_H_
