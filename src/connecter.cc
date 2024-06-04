#include "connecter.h"

#include "seastar/core/sleep.hh"
#include "seastar/core/timer.hh"
#include "ss.h"

#include <seastar/core/abort_source.hh>
#include <seastar/core/byteorder.hh>
#include <seastar/net/tcp.hh>

using namespace std::chrono_literals;

ss::future<> connecter::run(ss::abort_source& as) {
    auto socket = co_await ss::connect(_remote_addr);
    socket.set_nodelay(true);
    _logger->info("Connected to {}", _remote_addr);

    auto socket_ostream = socket.output();

    auto buf = ss::temporary_buffer<char>(sizeof(_seq_num));
    while (!as.abort_requested()) {
        auto start = ss::steady_clock_type::now();

        ss::write_be(buf.get_write(), _seq_num);
        co_await socket_ostream.write(buf.share());

        // TODO(nv): By default this call doesn't flush but enqueues the
        // output_stream to be flushed by the flush poller. Experiment with
        // disabling the flush poller.
        co_await socket_ostream.flush();

        auto in_buf = co_await socket.input().read_exactly(sizeof(_seq_num));
        if (in_buf.size() != sizeof(_seq_num)) {
            throw std::runtime_error("Unexpected EOF");
        }

        auto in_seq_num = ss::read_be<int64_t>(in_buf.get());

        if (_seq_num != in_seq_num) {
            throw std::runtime_error(
              "Expected seq_num " + std::to_string(_seq_num) + " but got "
              + std::to_string(in_seq_num));
        }

        auto end = ss::steady_clock_type::now();
        auto latency = end - start;

        _logger->info("Latency: {}us", latency / 1us);

        _seq_num += 1;

        co_await ss::sleep(50ms);
    }

    co_return;
}
