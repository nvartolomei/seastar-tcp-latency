#include "connecter.h"
#include "listener.h"
#include "ss.h"

#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/util/later.hh>

#include <boost/program_options.hpp>

#include <chrono>

namespace po = boost::program_options;

static ss::logger logger("app");
static auto as = ss::abort_source();

int main(int argc, char** argv) {
    ss::app_template app;
    app.add_options()(
      "listen",
      po::value<ss::sstring>(),
      "ip:port to listen on for incoming connections");

    app.add_options()(
      "connect", po::value<ss::sstring>(), "target ip:port to connect to");

    app.add_options()(
      "send-interval",
      po::value<double>()->default_value(0.05),
      "seconds between sending messages (default: 0.05)");

    app.add_options()(
      "send-bytes",
      po::value<uint64_t>()->default_value(0),
      "number of bytes to send in each message (on top of the 16 bytes "
      "header)");

    return app.run(argc, argv, [&] {
        seastar::engine().at_exit([] {
            logger.info("ss-tcp-latency is shutting down");
            as.request_abort();

            return ss::now();
        });

        auto& opts = app.configuration();

        if (!opts.count("listen") && !opts.count("connect")) {
            logger.error("Please provide one of --listen ip:port, --connect "
                         "ip:port options");
            return ss::make_ready_future<int>(1);
        }

        if (opts.count("listen")) {
            logger.info(
              "Starting ss-tcp-latency in listen mode on {}",
              opts["listen"].as<ss::sstring>());

            auto l = listener(
              &logger, ss::ipv4_addr(opts["listen"].as<ss::sstring>()));

            return ss::do_with(std::move(l), [](auto& l) {
                return l.run(as).then([] { return 0; }).finally([&l] {
                    return l.close();
                });
            });
        } else if (opts.count("connect")) {
            logger.info(
              "Starting ss-tcp-latency in connect mode to {}",
              opts["connect"].as<ss::sstring>());

            auto c = connecter(
              &logger,
              ss::ipv4_addr(opts["connect"].as<ss::sstring>()),
              std::chrono::microseconds(static_cast<int64_t>(
                opts["send-interval"].as<double>() * 1e6)),
                opts["send-bytes"].as<uint64_t>());

            return ss::do_with(std::move(c), [](auto& c) {
                return c.run(as).then([] { return 0; });
            });
        } else {
            throw std::runtime_error("Unreachable branch");
        }

        return ss::make_ready_future<int>(0);
    });
}
