#pragma once

#include "ss.h"

#include <seastar/core/abort_source.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/net/tcp.hh>
#include <seastar/util/log.hh>

#include <chrono>

class connecter {
public:
    connecter(
      ss::logger* logger,
      ss::socket_address remote_addr,
      std::chrono::microseconds send_interval,
      int64_t send_bytes)
      : _logger(logger)
      , _remote_addr(std::move(remote_addr))
      , _send_interval(send_interval)
      , _send_bytes(send_bytes) {}

    ss::future<> run(ss::abort_source& as);

private:
    ss::logger* _logger;
    ss::socket_address _remote_addr;
    std::chrono::microseconds _send_interval;
    int64_t _send_bytes;

    int64_t _seq_num = 0;
};
