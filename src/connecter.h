#pragma once

#include "ss.h"

#include <seastar/core/abort_source.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/net/tcp.hh>
#include <seastar/util/log.hh>

class connecter {
public:
    connecter(ss::logger* logger, ss::socket_address remote_addr)
      : _logger(logger)
      , _remote_addr(std::move(remote_addr)) {}

    ss::future<> run(ss::abort_source& as);

private:
    ss::logger* _logger;
    ss::socket_address _remote_addr;

    int64_t _seq_num = 0;
};
