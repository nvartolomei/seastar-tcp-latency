#include "listener.h"

#include "ss.h"

#include <seastar/core/abort_source.hh>
#include <seastar/net/tcp.hh>

#include <exception>
#include <stdexcept>

ss::future<> listener::run(ss::abort_source& as) {
    {
        _listener = ss::listen(_addr);

        auto sub = as.subscribe(
          [this]() noexcept { _listener.abort_accept(); });

        while (!as.abort_requested()) {
            ss::accept_result conn;
            try {
                conn = co_await _listener.accept();
            } catch (const std::runtime_error& e) {
                if (as.abort_requested()) {
                    std::rethrow_exception(as.abort_requested_exception_ptr());
                } else {
                    throw;
                }
            }
            auto addr = conn.connection.remote_address();

            _logger->info("Accepted connection from {}", addr);
        }

        co_return;
    }
}
