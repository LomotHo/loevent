# loevent

一个简单的 c++ Epoll 网络库

## http benchmark

```sh
wrk -t4 -c1000 -d60s --latency "http://192.168.8.31:3000/api/v1/stream/?streamname=livestream1&edgename=edge240"
```

- 结果

```
Running 1m test @ http://192.168.8.31:3000/api/v1/stream/?streamname=livestream1&edgename=edge240
  4 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    21.00ms   29.11ms   1.86s    99.89%
    Req/Sec     6.24k   667.86     7.97k    70.29%
  Latency Distribution
     50%   19.65ms
     75%   22.24ms
     90%   25.66ms
     99%   34.07ms
  1490373 requests in 1.00m, 95.23MB read
  Socket errors: connect 0, read 0, write 0, timeout 207
Requests/sec:  24809.32
Transfer/sec:      1.59MB
```
