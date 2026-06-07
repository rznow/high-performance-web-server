# MiniWebServer

基于 C++17 从零实现的高并发 HTTP 服务器，采用 **主从 Reactor + Epoll ET（边缘触发）** 事件驱动模型，实现了非阻塞网络通信、HTTP 协议解析、静态资源访问以及异步发送机制。

---

## 项目特点

* 基于 Linux Socket 网络编程
* 主从 Reactor 多线程架构
* Epoll 边缘触发（ET）模式
* 非阻塞 IO
* HTTP/1.1 请求解析
* HTTP 响应构造
* Buffer 缓冲区管理
* Connection 生命周期管理
* 静态文件服务器
* 404 页面处理
* 浏览器访问支持

---

## 技术栈

* C++17
* Linux
* Socket
* TCP/IP
* Epoll
* Reactor
* 多线程（std::thread）
* STL

---

## 项目架构

```text
                     +----------------+
                     |    Acceptor    |
                     +--------+-------+
                              |
                              v
                     +----------------+
                     | Master Reactor |
                     +--------+-------+
                              |
                      新连接分发
                              |
        +---------------------+---------------------+
        |                     |                     |
        v                     v                     v
+---------------+   +---------------+   +---------------+
| Sub Reactor 0 |   | Sub Reactor 1 |   | Sub Reactor 2 |
+-------+-------+   +-------+-------+   +-------+-------+
        |                   |                   |
        v                   v                   v
 +--------------+   +--------------+   +--------------+
 | Connection   |   | Connection   |   | Connection   |
 +--------------+   +--------------+   +--------------+
```

---

## 目录结构

```text
http
├── include
│   ├── Acceptor.h
│   ├── Buffer.h
│   ├── Connection.h
│   ├── HttpRequest.h
│   ├── HttpResponse.h
│   └── Reactor.h
│
├── src
│   ├── Acceptor.cpp
│   ├── Buffer.cpp
│   ├── Connection.cpp
│   ├── HttpRequest.cpp
│   ├── HttpResponse.cpp
│   ├── Reactor.cpp
│   └── main.cpp
│
├── www
│   ├── index.html
│   ├── 404.html
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
Master Reactor
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
查找静态资源
    |
    v
HttpResponse构造
    |
    v
写入OutputBuffer
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
/images/logo.png
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

### 404 页面

当资源不存在时：

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

### 异步发送机制

采用：

```text
EPOLLIN
EPOLLOUT
```

双事件驱动模型。

发送流程：

```text
Response
    ↓
OutputBuffer
    ↓
EPOLLOUT
    ↓
write()
    ↓
发送完成
```

支持处理：

* 半包发送
* 内核发送缓冲区写满
* 边缘触发模式

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

## 学习收获

通过本项目深入理解：

* Linux 网络编程
* Reactor 模型
* Epoll ET 工作机制
* HTTP 协议
* TCP 半包与粘包问题
* 非阻塞 IO
* 多线程服务器架构设计
* C++ 面向对象设计与工程化开发

---

## License

MIT License

