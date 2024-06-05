add_executable(ss-tcp-latency
    ss-tcp-latency.cc
    listener.cc
    connecter.cc
)

target_link_libraries(ss-tcp-latency fmt::fmt)
target_link_libraries(ss-tcp-latency Seastar::seastar)
