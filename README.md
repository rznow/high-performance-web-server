# MiniForum

> 基于 **C++17** 从零实现的轻量级论坛系统，采用 **Main-Reactor + Sub-Reactor + ThreadPool** 高并发网络架构，实现 HTTP Server、用户系统、帖子管理、评论系统、点赞功能以及 MySQL 数据持久化。

---

# 项目预览

## 已实现功能

- 用户注册
- 用户登录（JWT）
- 发帖
- 编辑帖子
- 删除帖子
- 浏览帖子
- 点赞/取消点赞
- 一级评论
- 多级评论（树形结构）
- 无限滚动加载评论
- 评论回复
- 评论数量统计
- 浏览量统计
- MySQL 数据持久化

---

# 项目特点

## 网络层

- Linux Socket 网络编程
- 非阻塞 IO
- Epoll ET（边缘触发）
- Main-Reactor + Sub-Reactor 架构
- Connection 生命周期管理
- Buffer 输入输出缓冲区
- HTTP Keep-Alive
- eventfd 跨线程唤醒

---

## 并发模型

采用经典 Reactor 模型：

```
                 Acceptor
                     │
             Main Reactor
                     │
     ┌───────────────┼───────────────┐
     │               │               │
 Sub Reactor     Sub Reactor     Sub Reactor
     │               │               │
 Connection      Connection      Connection
     │               │               │
     └───────────────┼───────────────┘
                     │
                ThreadPool
                     │
              Business Logic
                     │
                 eventfd
                     │
               Reactor发送响应
```

IO 与业务逻辑完全分离：

- Reactor 线程负责 Socket IO
- ThreadPool 负责业务处理
- eventfd 唤醒 Reactor 回写数据

避免业务线程直接操作 Socket，提高线程安全。

---

## HTTP

实现了完整 HTTP/1.1：

支持：

- GET
- POST
- PUT
- DELETE

请求解析：

```
GET /posts/1 HTTP/1.1
Host: localhost
Connection: keep-alive
```

支持：

- Header 解析
- Body 解析
- QueryString
- JSON 请求
- KeepAlive

---

## Buffer

实现 InputBuffer / OutputBuffer：

支持：

- 自动扩容
- 半包处理
- 粘包处理
- ET 模式循环读取

---

## ThreadPool

自实现线程池：

支持：

- Worker 线程
- Task Queue
- Condition Variable
- std::future
- submit()

业务任务全部异步执行。

---

## MySQL

实现：

- MySQL 封装
- MySQL 连接池
- RAII 自动回收连接
- 动态连接管理

数据库用于：

- 用户信息
- 帖子
- 评论
- 点赞

---

## 用户系统

实现：

- 注册
- 登录
- JWT Token
- Token 校验
- 登录状态保持

---

## 帖子系统

支持：

- 发布帖子
- 编辑帖子
- 删除帖子
- 浏览帖子
- 浏览量统计
- 点赞
- 点赞取消
- 评论数统计

---

## 评论系统

支持：

- 一级评论
- 多级评论（树形结构）
- 回复评论
- 无限滚动加载
- 评论数量统计

评论采用：

```
Comment
 ├── children
 │      ├── children
 │      ├── children
 │      └── ...
```

后端构建评论树：

```
unordered_map<int, Comment*>

comment_id
      │
      ▼
 Comment*

parent_id
      │
      ▼
children.push_back()
```

前端递归渲染评论树。

---

# 技术栈

- C++17
- Linux
- Socket
- TCP/IP
- Epoll
- Reactor
- ThreadPool
- eventfd
- STL
- MySQL
- HTML
- CSS
- JavaScript
- Fetch API
- JWT

---

# 项目目录

```text
MiniForum
├── build
├── include
│   ├── common
│   │   ├── Buffer.h
│   │   ├── Comment.h
│   │   ├── Post.h
│   │   ├── PostCache.h
│   │   └── UserInfo.h
│   │
│   ├── http
│   │   ├── HttpRequest.h
│   │   ├── HttpResponse.h
│   │   └── HttpServer.h
│   │
│   ├── mysql
│   │   ├── MySQL.h
│   │   ├── MySQLPool.h
│   │   └── init.sql
│   │
│   ├── network
│   │   ├── Acceptor.h
│   │   ├── Connection.h
│   │   └── Reactor.h
│   │
│   ├── service
│   │   └── PostService.h
│   │
│   ├── thread
│   │   └── ThreadPool.hpp
│   │
│   └── JWT.h
│
├── src
│   ├── main.cpp
│   ├── Reactor.cpp
│   ├── Connection.cpp
│   ├── Acceptor.cpp
│   ├── HttpRequest.cpp
│   ├── HttpResponse.cpp
│   ├── HttpServer.cpp
│   ├── ThreadPool.cpp
│   ├── MySQL.cpp
│   ├── MySQLPool.cpp
│   ├── PostService.cpp
│   └── PostCache.cpp
│
├── www
│   ├── index.html
│   ├── post.html
│   ├── login.html
│   ├── register.html
│   ├── newPost.html
│   ├── css
│   ├── js
│   └── images
│
├── Makefile
└── README.md
```

---

# HTTP 请求处理流程

```
浏览器

    │

TCP连接建立

    │

Acceptor

    │

Main Reactor

    │

分发 Connection

    │

Sub Reactor

    │

Connection::handleRead()

    │

InputBuffer

    │

HttpRequest 解析

    │

ThreadPool

    │

HttpServer

    │

业务处理

    │

HttpResponse

    │

OutputBuffer

    │

eventfd

    │

Reactor 唤醒

    │

EPOLLOUT

    │

Connection::handleWrite()

    │

浏览器收到响应
```

---

# 编译

```
make
```

---

# 运行

```
make run
```

默认监听：

```
192.168.1.8:8080
```

浏览器访问：

```
http://192.168.1.8:8080
```

---

# 已完成模块

- Reactor
- Epoll ET
- ThreadPool
- eventfd
- Buffer
- HTTP Parser
- HTTP Response
- 静态资源服务器
- 用户注册
- 用户登录
- JWT
- MySQL
- MySQL 连接池
- 帖子系统
- 评论系统
- 多级评论
- 点赞系统
- 浏览统计

---

# 后续计划

计划继续完善：

- Redis 缓存
- Session 管理
- 文件上传
- Markdown 编辑器
- WebSocket 实时通知
- 在线聊天
- 热帖排行榜
- 搜索功能
- Elasticsearch
- 压力测试（wrk）
- HTTPS
- Docker 部署

---

# 学习收获

通过本项目深入理解了：

- Linux 网络编程
- TCP/IP 协议
- HTTP/1.1
- Reactor 网络模型
- Epoll ET 工作机制
- 非阻塞 IO
- Buffer 设计
- ThreadPool 实现
- eventfd 跨线程通信
- MySQL 数据库设计
- MySQL 连接池
- JWT 身份认证
- 多线程高并发服务器设计
- C++17 工程化开发
- 前后端分离开发

---

# License

仅供学习交流使用。