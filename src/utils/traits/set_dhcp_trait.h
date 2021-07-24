#ifndef TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_SET_DHCP_TRAIT_H_
#define TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_SET_DHCP_TRAIT_H_

// Support for determining at compile time if the Ethernet instance has a
// setDhcp(DhcpClass*) method. This allows me to use either the "official"
// sstaub/Ethernet3 library, or my fork which has added that setDhcp method.
//
// Author: james.synge@gmail.com

#include "utils/platform_ethernet.h"
#include "utils/traits/type_traits.h"

namespace alpaca {
namespace set_dhcp_trait_internal {

// Matches a T that has a setDhcp(DhcpClass*) member function.
template <class T>
static auto test_set_dhcp(int)
    -> sfinae_true<decltype(declval<T>().setDhcp(declval<::DhcpClass*>()))>;

// SFINAE fallback for the case where T does not have a setDhcp(DhcpClass*)
// member function. This depends on the fact that type of the literal '0' is
// int, so the compiler will prefer a match to the prior function, but will
// fallback to this one instead of emitting an error.
template <typename T>
static auto test_set_dhcp(long) -> false_type;  // NOLINT

}  // namespace set_dhcp_trait_internal

// has_set_dhcp extends either true_type or false_type, depending on whether T
// has a setDhcp(DhcpClass*) member function.
template <typename T>
struct has_set_dhcp : decltype(set_dhcp_trait_internal::test_set_dhcp<T>(0)) {};

}  // namespace alpaca

#endif  // TINY_ALPACA_SERVER_SRC_UTILS_TRAITS_SET_DHCP_TRAIT_H_
