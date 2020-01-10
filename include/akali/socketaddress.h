﻿/*******************************************************************************
 * Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
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

#ifndef AKALI_SOCKETADDRESS_H_
#define AKALI_SOCKETADDRESS_H_

#include <iosfwd>
#include <string>
#include <vector>
#include "akali/ipaddress.h"
#include "akali/akali_export.h"

#undef SetPort

struct sockaddr_in;
struct sockaddr_storage;

namespace akali {
// Records an IP address and port.
class AKALI_API SocketAddress {
public:
  // Creates a nil address.
  SocketAddress();

  // Creates the address with the given host and port. Host may be a
  // literal IP string or a hostname to be resolved later.
  // DCHECKs that port is in valid range (0 to 2^16-1).
  SocketAddress(const std::string &hostname, int port);

  // Creates the address with the given IP and port.
  // IP is given as an integer in host byte order. V4 only, to be deprecated.
  // DCHECKs that port is in valid range (0 to 2^16-1).
  SocketAddress(uint32_t ip_as_host_order_integer, int port);

  // Creates the address with the given IP and port.
  // DCHECKs that port is in valid range (0 to 2^16-1).
  SocketAddress(const IPAddress &ip, int port);

  // Creates a copy of the given address.
  SocketAddress(const SocketAddress &addr);

  // Resets to the nil address.
  void Clear();

  // Determines if this is a nil address (empty hostname, any IP, null port)
  bool IsNil() const;

  // Returns true if ip and port are set.
  bool IsComplete() const;

  // Replaces our address with the given one.
  SocketAddress &operator=(const SocketAddress &addr);

  // Changes the IP of this address to the given one, and clears the hostname
  // IP is given as an integer in host byte order. V4 only, to be deprecated..
  void SetIP(uint32_t ip_as_host_order_integer);

  // Changes the IP of this address to the given one, and clears the hostname.
  void SetIP(const IPAddress &ip);

  // Changes the hostname of this address to the given one.
  // Does not resolve the address; use Resolve to do so.
  void SetIP(const std::string &hostname);

  // Sets the IP address while retaining the hostname.  Useful for bypassing
  // DNS for a pre-resolved IP.
  // IP is given as an integer in host byte order. V4 only, to be deprecated.
  void SetResolvedIP(uint32_t ip_as_host_order_integer);

  // Sets the IP address while retaining the hostname.  Useful for bypassing
  // DNS for a pre-resolved IP.
  void SetResolvedIP(const IPAddress &ip);

  // Changes the port of this address to the given one.
  // DCHECKs that port is in valid range (0 to 2^16-1).
  void SetPort(int port);

  // Returns the hostname.
  const std::string &hostname() const { return hostname_; }

  // Returns the IP address as a host byte order integer.
  // Returns 0 for non-v4 addresses.
  uint32_t ip() const;

  const IPAddress &ipaddr() const;

  int GetFamily() const { return ip_.GetFamily(); }

  // Returns the port part of this address.
  uint16_t port() const;

  // Returns the scope ID associated with this address. Scope IDs are a
  // necessary addition to IPv6 link-local addresses, with different network
  // interfaces having different scope-ids for their link-local addresses.
  // IPv4 address do not have scope_ids and sockaddr_in structures do not have
  // a field for them.
  int scope_id() const { return scope_id_; }
  void SetScopeID(int id) { scope_id_ = id; }

  // Returns the 'host' portion of the address (hostname or IP) in a form
  // suitable for use in a URI. If both IP and hostname are present, hostname
  // is preferred. IPv6 addresses are enclosed in square brackets ('[' and ']').
  std::string HostAsURIString() const;

  // Same as HostAsURIString but anonymizes IP addresses by hiding the last
  // part.
  std::string HostAsSensitiveURIString() const;

  // Returns the port as a string.
  std::string PortAsString() const;

  // Returns hostname:port or [hostname]:port.
  std::string ToString() const;

  // Same as ToString but anonymizes it by hiding the last part.
  std::string ToSensitiveString() const;

  // Parses hostname:port and [hostname]:port.
  bool FromString(const std::string &str);

  friend std::ostream &operator<<(std::ostream &os, const SocketAddress &addr);

  // Determines whether this represents a missing / any IP address.
  // That is, 0.0.0.0 or ::.
  // Hostname and/or port may be set.
  bool IsAnyIP() const;

  // Determines whether the IP address refers to a loopback address.
  // For v4 addresses this means the address is in the range 127.0.0.0/8.
  // For v6 addresses this means the address is ::1.
  bool IsLoopbackIP() const;

  // Determines whether the IP address is in one of the private ranges:
  // For v4: 127.0.0.0/8 10.0.0.0/8 192.168.0.0/16 172.16.0.0/12.
  // For v6: FE80::/16 and ::1.
  bool IsPrivateIP() const;

  // Determines whether the hostname has been resolved to an IP.
  bool IsUnresolvedIP() const;

  // Determines whether this address is identical to the given one.
  bool operator==(const SocketAddress &addr) const;
  inline bool operator!=(const SocketAddress &addr) const { return !this->operator==(addr); }

  // Compares based on IP and then port.
  bool operator<(const SocketAddress &addr) const;

  // Determines whether this address has the same IP as the one given.
  bool EqualIPs(const SocketAddress &addr) const;

  // Determines whether this address has the same port as the one given.
  bool EqualPorts(const SocketAddress &addr) const;

  // Hashes this address into a small number.
  size_t Hash() const;

  // Write this address to a sockaddr_in.
  // If IPv6, will zero out the sockaddr_in and sets family to AF_UNSPEC.
  void ToSockAddr(sockaddr_in *saddr) const;

  // Read this address from a sockaddr_in.
  bool FromSockAddr(const sockaddr_in &saddr);

  // Read and write the address to/from a sockaddr_storage.
  // Dual stack version always sets family to AF_INET6, and maps v4 addresses.
  // The other version doesn't map, and outputs an AF_INET address for
  // v4 or mapped addresses, and AF_INET6 addresses for others.
  // Returns the size of the sockaddr_in or sockaddr_in6 structure that is
  // written to the sockaddr_storage, or zero on failure.
  size_t ToDualStackSockAddrStorage(sockaddr_storage *saddr) const;
  size_t ToSockAddrStorage(sockaddr_storage *saddr) const;

private:
  std::string hostname_;
  IPAddress ip_;
  uint16_t port_;
  int scope_id_;
  bool literal_; // Indicates that 'hostname_' contains a literal IP string.
};

AKALI_API bool SocketAddressFromSockAddrStorage(const sockaddr_storage &saddr,
                                                  SocketAddress *out);
AKALI_API SocketAddress EmptySocketAddressWithFamily(int family);
} // namespace akali
#endif // AKALI_SOCKETADDRESS_H_