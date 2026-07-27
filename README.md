# MiniForum

> 基于 **C++17** 从零实现的高性能论坛系统，采用 **Main-Reactor +
> Sub-Reactor + ThreadPool** 网络模型，集成 **Epoll
> ET、MySQL、Redis、JWT**，实现用户、帖子、评论等完整业务。

------------------------------------------------------------------------

# ✨ Features

-   用户注册 / 登录（JWT）
-   发帖、编辑、删除帖子
-   点赞 / 取消点赞
-   一级评论、多级评论
-   浏览量统计
-   评论数量统计
-   MySQL 持久化
-   Redis 缓存（Cache Aside）
-   MySQL / Redis 连接池

------------------------------------------------------------------------

# 🏗 系统架构

``` text
                Browser
                   │
                 HTTP
                   │
            Main Reactor
                   │
               Acceptor
                   │
        ┌──────────┴──────────┐
        │                     │
   Sub Reactor           Sub Reactor
        │                     │
    Connection           Connection
        └──────────┬──────────┘
                   │
              ThreadPool
                   │
             Business Logic
                   │
             ┌──────────────┐
             │              │
           Redis          MySQL
```

------------------------------------------------------------------------

# 核心技术

-   C++17
-   Linux Socket
-   Epoll ET
-   Reactor
-   ThreadPool
-   eventfd
-   HTTP/1.1
-   MySQL
-   Redis
-   JWT
-   STL

------------------------------------------------------------------------

# 核心模块

## 网络层

-   Main-Reactor + Sub-Reactor
-   非阻塞 Socket
-   ET 模式
-   Connection 生命周期管理
-   Buffer
-   HTTP KeepAlive
-   eventfd 跨线程唤醒

## 数据层

-   MySQL 封装
-   Prepared Statement
-   MySQL 连接池
-   Redis 封装
-   Redis 连接池
-   Cache Aside

## 业务层

-   用户系统
-   帖子系统
-   评论系统
-   点赞系统

------------------------------------------------------------------------

# Redis 缓存设计

``` text
Browser
   │
Redis
   │ miss
MySQL
   │
Redis
```

缓存内容：

-   Post(Hash)
-   Comment(Hash)
-   评论列表(List)
-   点赞数
-   浏览量

------------------------------------------------------------------------

# HTTP 请求流程

``` text
Browser
   │
Connection
   │
InputBuffer
   │
HttpRequest
   │
ThreadPool
   │
HttpServer
   │
Redis / MySQL
   │
HttpResponse
   │
OutputBuffer
   │
EPOLLOUT
```

------------------------------------------------------------------------

# 项目目录

``` text
MiniForum
├── include
├── src
├── www
├── build
├── Makefile
```

------------------------------------------------------------------------

# 编译

``` bash
make
```

# 运行

``` bash
make run
```

默认：

``` text
http://192.168.1.8:8080
```

------------------------------------------------------------------------

# RoadMap

-   [x] Reactor
-   [x] ThreadPool
-   [x] HTTP Server
-   [x] JWT
-   [x] MySQL
-   [x] Redis
-   [ ] WebSocket
-   [ ] Docker
-   [ ] Elasticsearch
-   [ ] wrk 压力测试
-   [ ] HTTPS

------------------------------------------------------------------------

# Learning

通过本项目深入理解：

-   Reactor 网络模型
-   Epoll ET
-   非阻塞 IO
-   ThreadPool
-   Buffer
-   HTTP/1.1
-   MySQL Prepared Statement
-   Redis Cache Aside
-   RAII
-   C++17 工程化开发

------------------------------------------------------------------------

# License

仅供学习交流使用。
