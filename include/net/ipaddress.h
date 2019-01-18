/*******************************************************************************
* Copyright (C) 2018 - 2020, Jeffery Jiang, <china_jeffery@163.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/

#ifndef PPX_NET_IP_ADDRESS_H_
#define PPX_NET_IP_ADDRESS_H_
#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <string>
#include <vector>
#include "ppx_export.h"

namespace ppx {
    namespace net {
        enum IPv6AddressFlag {
            IPV6_ADDRESS_FLAG_NONE = 0x00,

            // Temporary address is dynamic by nature and will not carry MAC address.
            IPV6_ADDRESS_FLAG_TEMPORARY = 1 << 0,

            // Temporary address could become deprecated once the preferred
            // lifetime is reached. It is still valid but just shouldn't be used
            // to create new connection.
            IPV6_ADDRESS_FLAG_DEPRECATED = 1 << 1,
        };

        // Version-agnostic IP address class, wraps a union of in_addr and in6_addr.
        class PPX_API IPAddress {
        public:
            IPAddress() : family_(AF_UNSPEC) {
                memset(&u_, 0, sizeof(u_));
            }

            explicit IPAddress(const in_addr &ip4) : family_(AF_INET) {
                memset(&u_, 0, sizeof(u_));
                u_.ip4 = ip4;
            }

            explicit IPAddress(const in6_addr &ip6) : family_(AF_INET6) {
                u_.ip6 = ip6;
            }

            explicit IPAddress(uint32_t ip_in_host_byte_order);

            IPAddress(const IPAddress &other) : family_(other.family_) {
                ::memcpy(&u_, &other.u_, sizeof(u_));
            }

            virtual ~IPAddress() {}

            const IPAddress &operator=(const IPAddress &other) {
                family_ = other.family_;
                memcpy(&u_, &other.u_, sizeof(u_));
                return *this;
            }

            bool operator==(const IPAddress &other) const;
            bool operator!=(const IPAddress &other) const;
            bool operator <(const IPAddress &other) const;
            bool operator >(const IPAddress &other) const;
            friend std::ostream &operator<<(std::ostream &os, const IPAddress &addr);

            int GetFamily() const {
                return family_;
            }
            in_addr GetIPv4Address() const;
            in6_addr GetIPv6Address() const;

            // Returns the number of bytes needed to store the raw address.
            size_t Size() const;

            // Wraps inet_ntop.
            std::string ToString() const;

            // Same as ToString but annoymizes it by hiding the last part.
            std::string ToSensitiveString() const;

            // Returns an unmapped address from a possibly-mapped address.
            // Returns the same address if this isn't a mapped address.
            IPAddress Normalized() const;

            // Returns this address as an IPv6 address.
            // Maps v4 addresses (as ::ffff:a.b.c.d), returns v6 addresses unchanged.
            IPAddress AsIPv6Address() const;

            // For socketaddress' benefit. Returns the IP in host byte order.
            uint32_t v4AddressAsHostOrderInteger() const;

            // Whether this is an unspecified IP address.
            bool IsUnspecifiedIP() const;

            bool IsValid() const;
        private:
            int family_;
            union {
                in_addr ip4;
                in6_addr ip6;
            } u_;
        };

        // IP class which could represent IPv6 address flags which is only
        // meaningful in IPv6 case.
        class PPX_API InterfaceAddress : public IPAddress {
        public:
            InterfaceAddress() : ipv6_flags_(IPV6_ADDRESS_FLAG_NONE) {}

            InterfaceAddress(IPAddress ip)
                : IPAddress(ip), ipv6_flags_(IPV6_ADDRESS_FLAG_NONE) {
            }

            InterfaceAddress(IPAddress addr, int ipv6_flags)
                : IPAddress(addr), ipv6_flags_(ipv6_flags) {
            }

            InterfaceAddress(const in6_addr &ip6, int ipv6_flags)
                : IPAddress(ip6), ipv6_flags_(ipv6_flags) {
            }

            const InterfaceAddress &operator=(const InterfaceAddress &other);

            bool operator==(const InterfaceAddress &other) const;
            bool operator!=(const InterfaceAddress &other) const;

            int ipv6_flags() const {
                return ipv6_flags_;
            }
            friend std::ostream &operator<<(std::ostream &os,
                const InterfaceAddress &addr);

        private:
            int ipv6_flags_;
        };

        bool IPFromAddrInfo(struct addrinfo *info, IPAddress *out);
        bool IPFromString(const std::string &str, IPAddress *out);
        bool IPFromString(const std::string &str, int flags,
            InterfaceAddress *out);
        bool IPIsAny(const IPAddress &ip);
        bool IPIsLoopback(const IPAddress &ip);
        bool IPIsPrivate(const IPAddress &ip);
        bool IPIsUnspec(const IPAddress &ip);
        size_t HashIP(const IPAddress &ip);

        // These are only really applicable for IPv6 addresses.
        bool IPIs6Bone(const IPAddress &ip);
        bool IPIs6To4(const IPAddress &ip);
        bool IPIsLinkLocal(const IPAddress &ip);
        bool IPIsMacBased(const IPAddress &ip);
        bool IPIsSiteLocal(const IPAddress &ip);
        bool IPIsTeredo(const IPAddress &ip);
        bool IPIsULA(const IPAddress &ip);
        bool IPIsV4Compatibility(const IPAddress &ip);
        bool IPIsV4Mapped(const IPAddress &ip);

        // Returns the precedence value for this IP as given in RFC3484.
        int IPAddressPrecedence(const IPAddress &ip);

        // Returns 'ip' truncated to be 'length' bits long.
        IPAddress TruncateIP(const IPAddress &ip, int length);

        IPAddress GetLoopbackIP(int family);
        IPAddress GetAnyIP(int family);

        // Returns the number of contiguously set bits, counting from the MSB in network
        // byte order, in this IPAddress. Bits after the first 0 encountered are not counted.
        int CountIPMaskBits(IPAddress mask);
    }
}

#endif // !PPX_NET_IP_ADDRESS_H_