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
  identifying the best set of CPU cores to run the application on. I.e. to avoid
  Hyper-Threading, avoid efficiency cores if present, etc.
- For lowest latency you likely want to run with Seastar's `--poll-mode`.
- [ENA Linux Driver Best Practices and Performance Optimization Guide](https://github.com/amzn/amzn-drivers/blob/master/kernel/linux/ena/ENA_Linux_Best_Practices.rst)
- [EC2 Placement groups](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/placement-groups.html#placement-groups-cluster)
