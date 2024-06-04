## Building

```
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -S . -B build \
    && cmake --build build \
    && ./build/src/ss-tcp-latency
```

## Running

Tips:
- Read the excellent
  [sled theoretical performance guide](https://sled.rs/perf.html#experimental-design).
- Use `lstopo` from [hwloc](https://www.open-mpi.org/projects/hwloc/) for
  identifying the best set of CPU cores to run the application on. I.e. to
  avoid Hyper-Threading, avoid efficiency cores if present, etc.
- For lowest latency you likely want to run with `--poll-mode`.
