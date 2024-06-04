## Building

```
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -S . -B build \
    && cmake --build build \
    && ./build/src/ss-tcp-latency
```
