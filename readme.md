# loevent

一个简单的 c++ Epoll 网络库



## tcp benchmark

## http benchmark

```sh
wrk -t4 -c1000 -d60s --latency "http://192.168.8.31:3000/api/v1/stream/?streamname=livestream1&edgename=edge240"
```

### localhost



### localhost

```
Running 30s test @ http://192.168.8.31:3006/api/v1/stream/?streamname=livestream1&edgename=edge240
  2 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     9.43ms   50.59ms   1.67s    99.52%
    Req/Sec    61.00k     6.48k   84.24k    79.43%
  3631548 requests in 30.04s, 1.16GB read
  Socket errors: connect 0, read 0, write 0, timeout 121
Requests/sec: 120895.03
Transfer/sec:     39.43MB
```

### 局域网多机

- 单线程(无锁) 1000连接

```
Running 1m test @ http://192.168.8.31:3007/api/v1/stream/?streamname=livestream1&edgename=edge240
  2 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    28.05ms   38.09ms   1.95s    99.79%
    Req/Sec    12.96k     1.09k   16.78k    66.33%
  Latency Distribution
     50%   26.82ms
     75%   27.62ms
     90%   29.75ms
     99%   31.83ms
  1547438 requests in 1.00m, 504.71MB read
  Socket errors: connect 0, read 0, write 0, timeout 82
Requests/sec:  25752.24
Transfer/sec:      8.40MB
```

- 单线程(有锁) 1000连接
```
Running 1m test @ http://192.168.8.31:3007/api/v1/stream/?streamname=livestream1&edgename=edge240
  2 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    57.59ms    6.99ms 491.84ms   72.61%
    Req/Sec     8.72k     0.94k   15.59k    76.00%
  Latency Distribution
     50%   58.03ms
     75%   61.55ms
     90%   66.00ms
     99%   70.21ms
  1041478 requests in 1.00m, 339.68MB read
Requests/sec:  17331.17
Transfer/sec:      5.65MB
```

- 双线程 1000连接
```
Running 20s test @ http://192.168.8.31:3007/api/v1/stream/?streamname=livestream1&edgename=edge240
  2 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    28.16ms   85.38ms   1.80s    97.25%
    Req/Sec    26.81k     3.44k   43.27k    81.41%
  Latency Distribution
     50%   16.03ms
     75%   23.95ms
     90%   28.27ms
     99%  303.51ms
  1061530 requests in 20.00s, 346.23MB read
  Socket errors: connect 0, read 0, write 0, timeout 46
Requests/sec:  53072.29
Transfer/sec:     17.31MB
```
