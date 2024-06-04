#include "seastar/util/log.hh"
#include "ss.h"

#include <seastar/core/abort_source.hh>
#include <seastar/net/inet_address.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/net/tcp.hh>

class listener {
public:
    listener(ss::logger* logger, ss::socket_address addr)
      : _logger(logger)
      , _addr(std::move(addr)) {}

    ss::future<> run(ss::abort_source& as);

private:
    ss::logger* _logger;
    ss::socket_address _addr;
    ss::server_socket _listener;
};
