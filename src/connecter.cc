#include "connecter.h"

#include "ss.h"

#include <seastar/core/abort_source.hh>
#include <seastar/net/tcp.hh>

ss::future<> connecter::run(ss::abort_source& as) {
    auto socket = co_await ss::connect(_remote_addr);
    _logger->info("Connected to {}", _remote_addr);

    // while (!as.abort_requested()) {
    // }

    co_return;
}
