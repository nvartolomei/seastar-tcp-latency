#include "connecter.h"

#include "ss.h"

#include <seastar/core/abort_source.hh>
#include <seastar/core/byteorder.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/timer.hh>
#include <seastar/net/tcp.hh>

using namespace std::chrono_literals;

ss::future<> connecter::run(ss::abort_source& as) {
    auto socket = co_await ss::connect(_remote_addr);
    socket.set_nodelay(true);
    _logger->info("Connected to {}", _remote_addr);

    auto socket_istream = socket.input();
    auto socket_ostream = socket.output();

    auto header_buf = ss::temporary_buffer<char>(
      sizeof(_seq_num) + sizeof(_send_bytes));
    auto payload = ss::temporary_buffer<char>(_send_bytes);
    auto rnd = std::mt19937(std::random_device()());
    auto dist = std::uniform_int_distribution<int>(
      0, std::numeric_limits<char>::max());
    std::generate(
      payload.get_write(), payload.get_write() + payload.size(), [&dist, &rnd] {
          return dist(rnd);
      });

    while (!as.abort_requested()) {
        auto start = ss::steady_clock_type::now();

        ss::write_be(header_buf.get_write(), _seq_num);
        ss::write_be(header_buf.get_write() + sizeof(_seq_num), _send_bytes);
        co_await socket_ostream.write(header_buf.share());
        co_await socket_ostream.write(payload.share());

        // TODO(nv): By default this call doesn't flush but enqueues the
        // output_stream to be flushed by the flush poller. Experiment with
        // disabling the flush poller.
        auto flush_fut = socket_ostream.flush();

        auto in_buf = co_await socket_istream.read_exactly(sizeof(_seq_num));
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

        if (latency < _send_interval) {
            co_await ss::sleep(_send_interval - latency);
        }
    }

    co_return;
}
