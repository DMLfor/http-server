## http-server

linux C 实现的Tiny-http服务器, 支持高并发。
支持GET方法,可配置，可写日志，可启用文件缓存。

## 服务器模型

服务器采用的是多线程，并且每一个线程内都有一个高效的I/O多路复用的epoll, 适合
密集的I/O场景。

## 模块划分
```
├── bin  //源代码目录 
│   ├── config.c            //配置文件模块 
│   ├── config.h
│   ├── epoll.c             //epoll相关模块
│   ├── epoll.h
│   ├── filebuf.c           //文件缓存模块，未启用
│   ├── filebuf.h
│   ├── handle_request.c    //处理http请求和回复模块
│   ├── handle_request.h
│   ├── http.c              //main函数所在模块，主要为启动服务准备 
│   ├── http.h
│   ├── log.c               //日志模块
│   ├── log.h
│   └── makefile            //makefile
├── docroot //资源文件目录
│   ├── 4k.html
│   ├── dir
│   │   └── 4k.html
│   ├── index.html
│   └── test.html
├── etc    //配置文件目录
│  └── httpd.conf
├── log   //日志文件目录
└── README.md
```
## 性能测试

短连接
>ab -n 1000000 -c 10000  127.0.0.1:2017/4k.html
```
Server Hostname:        10.0.0.201
Server Port:            8000

Document Path:          /4k.html
Document Length:        4205 bytes

Concurrency Level:      10000
Time taken for tests:   44.509 seconds
Complete requests:      1000000
Failed requests:        0
Write errors:           0
Total transferred:      4271000000 bytes
HTML transferred:       4205000000 bytes
Requests per second:    22467.14 [#/sec] (mean)
Time per request:       445.094 [ms] (mean)
Time per request:       0.045 [ms] (mean, across all concurrent requests)
Transfer rate:          93708.17 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0  211  25.6    212     313
Processing:    74  231  32.5    229     415
Waiting:        1  168  31.6    164     321
Total:        224  443  16.9    441     539

Percentage of the requests served within a certain time (ms)
  50%    441
  66%    445
  75%    449
  80%    451
  90%    460
  95%    478
  98%    487
  99%    492
 100%    539 (longest request)
```
长连接
>ab -n 1000000 -c 10000  127.0.0.1:2017/4k.html

```
      
Server Hostname:        10.0.0.201
Server Port:            8000

Document Path:          /4k.html
Document Length:        4205 bytes

Concurrency Level:      10000
Time taken for tests:   9.090 seconds
Complete requests:      1000000
Failed requests:        0
Write errors:           0
Keep-Alive requests:    1000000
Total transferred:      4323000000 bytes
HTML transferred:       4205000000 bytes
Requests per second:    110015.47 [#/sec] (mean)
Time per request:       90.896 [ms] (mean)
Time per request:       0.009 [ms] (mean, across all concurrent requests)
Transfer rate:          464450.06 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    1  15.4      0     217
Processing:    20   88  24.5     82     224
Waiting:        0   78  27.6     75     224
Total:         20   90  28.4     83     299

Percentage of the requests served within a certain time (ms)
  50%     83
  66%     91
  75%     98
  80%    104
  90%    121
  95%    148
  98%    170
  99%    192
 100%    299 (longest request)
```

