"""Build configuration for TinyAlpacaServer, et al, on the host.

Arduino IDE 1.x uses g++ for approximately C++ version 2011. However I'm
primarily developing for a host target (for speed of development) using Bazel
and clang. Unfortunately Bazel doesn't seem to provide a mechanism for me to
override the addition of the option "-std=g++17" that it adds to the clang
command line. So, this file adds a bunch of warnings (as errors) which help me
detect when I've used a C++14 or later feature that probably won't work with
g++11.
"""

_default_copts = [
    "-Wpre-c++14-compat",
]

build_config = struct(
    default_copts = _default_copts,
)
