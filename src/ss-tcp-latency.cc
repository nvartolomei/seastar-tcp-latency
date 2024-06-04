#include "connecter.h"
#include "listener.h"
#include "ss.h"

#include <seastar/core/app-template.hh>
#include <seastar/core/reactor.hh>
#include <seastar/net/socket_defs.hh>
#include <seastar/util/later.hh>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

static ss::logger logger("app");

int main(int argc, char** argv) {
    ss::app_template app;
    app.add_options()(
      "listen",
      po::value<ss::sstring>(),
      "ip:port to listen on for incoming connections");

    app.add_options()(
      "connect", po::value<ss::sstring>(), "target ip:port to connect to");

    auto as = ss::abort_source();

    return app.run(argc, argv, [&] {
        seastar::engine().at_exit([&as] {
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

            return ss::do_with(std::move(l), [&as](auto& l) {
                return l.run(as).then([] { return 0; }).finally([&l] {
                    return l.close();
                });
            });
        } else if (opts.count("connect")) {
            logger.info(
              "Starting ss-tcp-latency in connect mode to {}",
              opts["connect"].as<ss::sstring>());

            auto c = connecter(
              &logger, ss::ipv4_addr(opts["connect"].as<ss::sstring>()));

            return ss::do_with(std::move(c), [&as](auto& c) {
                return c.run(as).then([] { return 0; });
            });
        } else {
            throw std::runtime_error("Unreachable branch");
        }

        return ss::make_ready_future<int>(0);
    });
}
