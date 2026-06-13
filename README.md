# MiniWebServer

基于 C++17 从零实现的高并发 HTTP 服务器，采用 **Main-Reactor + Sub-Reactor + ThreadPool** 架构，基于 **Epoll ET（边缘触发）** 实现事件驱动模型，支持 HTTP 请求解析、静态资源访问、异步业务处理与响应发送。

---

## 项目特点

* 基于 Linux Socket 网络编程
* Main-Reactor + Sub-Reactor 多线程架构
* Epoll ET（边缘触发）模式
* 非阻塞 IO
* HTTP/1.1 请求解析
* HTTP 响应构造
* Buffer 缓冲区管理
* Connection 生命周期管理
* 静态资源服务器
* ThreadPool 业务线程池
* eventfd 跨线程唤醒机制
* 404 页面处理
* 浏览器访问支持
* MySQL 数据库集成
* 用户注册功能
* 用户登录认证


---

## 技术栈

* C++17
* Linux
* Socket
* TCP/IP
* Epoll
* Reactor
* eventfd
* 多线程（std::thread）
* STL
* ThreadPool
* MySQL

---

## 整体架构

```text
                          +----------------+
                          |    Acceptor    |
                          +--------+-------+
                                   |
                                   v
                          +----------------+
                          | Main Reactor   |
                          +--------+-------+
                                   |
                            新连接分发
                                   |
        +--------------------------+--------------------------+
        |                          |                          |
        v                          v                          v
+---------------+        +---------------+        +---------------+
| Sub Reactor 0 |        | Sub Reactor 1 |        | Sub Reactor 2 |
+-------+-------+        +-------+-------+        +-------+-------+
        |                        |                        |
        v                        v                        v
 +--------------+         +--------------+         +--------------+
 | Connection   |         | Connection   |         | Connection   |
 +------+-------+         +------+-------+         +------+-------+
        |                        |                        |
        |                        |                        |
        +------------------------+------------------------+
                                 |
                                 v
                         +---------------+
                         |  Thread Pool  |
                         +-------+-------+
                                 |
                            业务逻辑处理
                                 |
                                 v
                              eventfd
                                 |
                                 v
                            Sub Reactor
```

---

## 并发模型

服务器采用 Main-Reactor + Sub-Reactor + ThreadPool 架构：

### Main Reactor

负责：

* 监听 listenfd
* accept 新连接
* 将连接分发给 Sub Reactor

### Sub Reactor

负责：

* EPOLLIN 事件处理
* EPOLLOUT 事件处理
* Connection 生命周期管理

### Thread Pool

负责：

* 文件读取
* HTTP 响应构造
* 后续业务逻辑扩展

业务线程不直接操作 Socket，而是通过 eventfd 通知对应 Reactor 线程完成发送操作，保证 IO 操作始终在 Reactor 所属线程中执行。

---

## 项目目录

```text
MiniWebServer
├── include
│   ├── Acceptor.h
│   ├── Buffer.h
│   ├── Connection.h
│   ├── HttpRequest.h
│   ├── HttpResponse.h
│   ├── HttpServer.h
│   ├── MySQL.h
│   ├── Reactor.h
│   └── ThreadPool.h
│
├── src
│   ├── Acceptor.cpp
│   ├── Buffer.cpp
│   ├── Connection.cpp
│   ├── HttpRequest.cpp
│   ├── HttpResponse.cpp
│   ├── HttpServer.cpp
│   ├── MySQL.cpp
│   ├── Reactor.cpp
│   ├── ThreadPool.cpp
│   └── main.cpp
│
├── www
│   ├── index.html
│   ├── 404.html
│   ├── login.html
│   ├── register.html
│   ├── css
│   ├── js
│   └── images
│
├── Makefile
└── README.md
```

---

## HTTP 请求处理流程

```text
客户端
    |
    v
TCP连接建立
    |
    v
Acceptor接收连接
    |
    v
Main Reactor
    |
    v
分发至Sub Reactor
    |
    v
Connection::handleRead()
    |
    v
读取请求数据
    |
    v
HttpRequest解析
    |
    v
投递线程池ThreadPool
    |
    v
HttpServer业务逻辑处理
    |
    v
构造HttpResponse
    |
    v
eventfd唤醒Reactor
    |
    v
OutputBuffer
    |
    v
EPOLLOUT触发
    |
    v
Connection::handleWrite()
    |
    v
响应发送完成
```

---

## 已实现功能

### HTTP 请求解析

支持解析：

```http
GET /index.html HTTP/1.1
Host: localhost
Connection: keep-alive
```

解析结果：

```text
Method: GET
Path: /index.html
Version: HTTP/1.1
```

---

### 静态资源访问

支持：

```text
/
/index.html
/css/style.css
/js/app.js
/images/back.png
```

访问流程：

```text
GET /index.html
        ↓
www/index.html
        ↓
读取文件
        ↓
构造响应
```

---

### 404 页面处理

资源不存在时：

```text
GET /notfound.html
```

返回：

```http
HTTP/1.1 404 Not Found
```

并加载：

```text
www/404.html
```

---

### Buffer 缓冲区

实现：

```text
InputBuffer
OutputBuffer
```

支持：

* 数据追加
* 数据读取
* 数据清空
* 半包处理

---

### 异步发送机制

采用：

```text
EPOLLIN
EPOLLOUT
```

双事件驱动模型。

发送流程：

```text
HttpResponse
      ↓
OutputBuffer
      ↓
enableWrite()
      ↓
EPOLLOUT
      ↓
write()
      ↓
发送完成
```

支持处理：

* 半包发送
* 发送缓冲区写满
* ET模式下连续发送

---

## 编译运行

### 编译

```bash
make
```

### 运行

```bash
make run
```

### 浏览器访问

```text
http://127.0.0.1:8080
```

---

## 学习收获

通过本项目深入理解：

* Linux 网络编程
* TCP/IP 协议
* Reactor 模型
* Epoll ET 工作机制
* HTTP 协议
* 非阻塞 IO
* Buffer 设计
* ThreadPool 实现
* eventfd 跨线程通信
* MySQL数据库集成
* 多线程服务器架构设计
* C++ 面向对象设计与工程化开发
