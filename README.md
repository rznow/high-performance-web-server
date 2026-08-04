# MiniForum

> 基于 **C++17 从零实现的高性能论坛系统**  
> 采用 **Main-Reactor + Sub-Reactor + ThreadPool** 网络模型，基于 Linux 
> Epoll ET 实现高并发 HTTP 服务，集成 MySQL、Redis、JWT，实现用户、
> 帖子、评论、点赞、缓存等完整业务。

---

# 📌 项目简介

MiniForum 是一个从零设计并实现的高性能论坛系统。

项目没有使用现成 Web 框架，而是基于 Linux Socket、
Epoll、Reactor 模型自主实现 HTTP Server。

系统采用：

- Main-Reactor 负责连接建立
- Sub-Reactor 负责 IO 事件处理
- ThreadPool 处理业务逻辑
- Redis 提供高性能缓存
- MySQL 负责数据持久化

通过该项目深入实践：

- C++ 网络编程
- 高并发服务器设计
- Linux IO 多路复用
- 数据库优化
- 缓存系统设计
- 后端工程化开发


---

# ✨ Features

## 用户系统

- 用户注册
- 用户登录
- JWT Token 身份认证
- 用户信息查询
- 用户头像上传
- 用户资料展示


## 帖子系统

- 发布帖子
- 修改帖子
- 删除帖子
- 帖子分页查询
- 浏览量统计
- 作者信息展示


## 评论系统

- 一级评论
- 多级回复
- 评论分页
- 评论数量统计
- 评论缓存


## 点赞系统

- 点赞帖子
- 取消点赞
- 点赞数量统计
- Redis 高速计数


## 数据系统

- MySQL 数据持久化
- Redis Cache Aside 缓存
- MySQL 连接池
- Redis 连接池
- Redis 后台同步


---

# 🏗 系统架构

```
                         Browser
                            |
                           HTTP
                            |
                    +---------------+
                    | Main Reactor |
                    +---------------+
                            |
                        Acceptor
                            |
          +-----------------+----------------+
          |                                  |
   +--------------+                  +--------------+
   | Sub Reactor  |                  | Sub Reactor |
   +--------------+                  +--------------+
          |                                  |
      Connection                         Connection
          |                                  |
          +---------------+------------------+
                          |
                      ThreadPool
                          |
                    Business Layer
                          |
          +---------------+---------------+
          |                               |
        Redis                           MySQL
        Cache                         Persistence

```


---

# 🚀 核心技术


## 网络

- C++17
- Linux Socket
- TCP/IP
- Epoll ET
- Reactor 模型
- 非阻塞 IO
- eventfd
- HTTP/1.1
- Keep-Alive


## 并发

- Main-Reactor
- Sub-Reactor
- ThreadPool
- Mutex
- Condition Variable
- RAII


## 数据库

- MySQL
- Prepared Statement
- MySQL Connection Pool


## 缓存

- Redis
- Redis Connection Pool
- Cache Aside Pattern
- Hash
- ZSet
- List


## 认证

- JWT
- Token 校验
- 用户权限控制


---

# ⚙️ 网络模型


## Main Reactor

负责：

- listen socket
- accept 新连接
- 分发 Connection


## Sub Reactor

负责：

- epoll_wait
- socket read/write
- Connection 生命周期


## ThreadPool

负责：

- HTTP 请求处理
- MySQL 查询
- Redis 操作
- 业务逻辑


流程：

```
Client

 ↓

Socket

 ↓

Epoll

 ↓

Connection

 ↓

HttpRequest

 ↓

ThreadPool

 ↓

HttpServer

 ↓

Redis/MySQL

 ↓

HttpResponse

 ↓

EPOLLOUT

 ↓

Client
```


---

# 📦 HTTP Server


支持：

- HTTP/1.1
- Keep-Alive
- GET
- POST
- multipart/form-data
- JSON Request


请求流程：

```
recv()

 ↓

InputBuffer

 ↓

HttpRequest Parser

 ↓

Business Handler

 ↓

HttpResponse

 ↓

OutputBuffer

 ↓

send()
```


---

# 🗄 Redis 缓存设计


采用：

## Cache Aside Pattern

读取帖子：
```
                 Request
                    |
                    |
              Redis Index
                    |
        +-----------+------------+
        |                        |
       Hit                      Miss
        |                        |
        |                        |
   Get Post Hash             MySQL Page
        |                        |
   +----+----+                   |
   |         |                   |
cache/redis Miss                 |
   |         |                   |
 All Hit  MySQL                  |
   |         |                   |
 Return   Merge                  |
   |         |                   |
   +----+----+-------------------+
        |
        |
   Update Redis
        |
        |
     Return
```


读取：

```
Request

 ↓

Redis

 ↓ hit

Return


 ↓ miss


MySQL

 ↓

Update Redis

 ↓

Return
```


缓存内容：


## Post

Redis Hash:

```
post:{id}

title
content
author
view_count
like_count
comment_count
```


## 评论

```
comment:{id}

content
user_id
parent_id
```


评论索引：

```
post:{id}:comments
```

使用：

- ZSet 保存评论顺序
- Hash 保存评论内容


## 用户

```
user:{id}

id
username
avatar
```


---

# 🔥 数据一致性设计


## Cache Aside


更新流程：

```
Update MySQL

      |

Delete Redis

      |

Next Request Reload Cache
```


避免：

- 数据长期不一致
- 缓存污染


---

# 📂 项目结构


```
MiniForum

├── include
│
│   ├── common
│   │
│   ├── http
│   │
│   ├── mysql
│   │
│   ├── network
│   │
│   ├── redis
│   │
│   ├── service
│   │
│   ├── thread
│   │
│   └── util
│
├── src
│
├── www
│
│   ├── css
│   ├── images
│   ├── html
│   ├── upload
│   └── js
│
├── server.conf
│
├── build
│
├── workbench
│
└── Makefile

```


---

# ⚡ 性能测试


使用：

```
wrk
```


测试：

```bash
wrk -t4 -c200 -d30s http://127.0.0.1:8080/
或
make bench THREADS=? CONNS=? DURATION=? PATH_URL=? SCRIPT=?
```


测试指标：

- QPS
- 延迟
- 并发连接
- CPU 使用率


---

# 🛠 编译


环境：

```
Linux

g++ >= 11

C++17

Redis

MySQL
```


编译：

```bash
make
```


运行：

```bash
make run
```


测试：

```bash
make test
```


访问：

```
http://127.0.0.1:8080
```


---

# 📝 配置文件


示例：

```ini
# server
port=8080
thread_num=8
reactor_num=4
max_events=1000

# mysql
mysql_host=127.0.0.1
mysql_user=webserver
mysql_password=123456
mysql_database=miniforum
mysql_port=3306
mysql_pool_size=10

# redis
redis_host=127.0.0.1
redis_port=6379
redis_pool_size=10

```


---

# 🧩 已完成


- [x] TCP Server
- [x] Reactor 模型
- [x] Main/Sub Reactor
- [x] Epoll ET
- [x] 非阻塞 Socket
- [x] eventfd
- [x] ThreadPool
- [x] HTTP Parser
- [x] HTTP Response
- [x] Keep Alive
- [x] JWT认证
- [x] 用户系统
- [x] 帖子系统
- [x] 评论系统
- [x] 点赞系统
- [x] 文件上传
- [x] MySQL
- [x] Redis
- [x] Redis缓存
- [x] Redis后台同步
- [x] wrk压力测试


---

# 🚧 RoadMap


## 工程化

- [ ] Logger 异步日志系统
- [ ] Config 配置系统
- [ ] Docker 部署
- [ ] Nginx 反向代理
- [ ] HTTPS


## 性能优化

- [ ] Redis Pipeline
- [ ] MySQL批量操作优化
- [ ] 热点数据缓存
- [ ] Prometheus监控


## 高级功能

- [ ] WebSocket实时通信
- [ ] Elasticsearch全文搜索
- [ ] 消息队列


## AI方向

- [ ] AI帖子摘要
- [ ] AI内容审核
- [ ] RAG智能搜索
- [ ] Agent助手


---

# 📚 技术收获


通过本项目深入理解：

- Reactor 网络模型
- Epoll ET
- Linux IO
- TCP连接管理
- HTTP协议
- C++ RAII
- 多线程并发
- Redis缓存设计
- MySQL优化
- 后端系统架构


---

# ⭐ 项目亮点


相比普通 Web 项目：

✅ 自主实现 HTTP Server  
✅ 自主设计 Reactor 网络模型  
✅ 高并发 IO 模型  
✅ Redis + MySQL 双层存储  
✅ Cache Aside 缓存架构  
✅ 连接池设计  
✅ 完整业务闭环  


---

# License

MIT License

仅用于学习交流。