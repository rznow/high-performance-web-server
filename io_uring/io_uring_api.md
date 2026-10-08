# io_uring 常用接口速查

本文介绍 `liburing` 中常用的 `io_uring` 接口，按照一次 I/O
操作的生命周期组织：

``` text
io_uring_queue_init()
        ↓
io_uring_get_sqe()
        ↓
io_uring_prep_xxx()
        ↓
io_uring_submit()
        ↓
io_uring_wait_cqe() / io_uring_peek_cqe()
        ↓
io_uring_cqe_seen()
        ↓
io_uring_queue_exit()
```

------------------------------------------------------------------------

## 1. `io_uring_queue_init()`

### 函数原型

``` cpp
int io_uring_queue_init(
    unsigned entries,
    struct io_uring *ring,
    unsigned flags
);
```

### 参数

  ----------------------------------------------------------------------------
  参数                    类型                    作用
  ----------------------- ----------------------- ----------------------------
  `entries`               `unsigned`              Ring
                                                  中期望支持的队列深度，即
                                                  SQ/CQ 的容量规模

  `ring`                  `struct io_uring*`      指向需要初始化的 `io_uring`
                                                  对象

  `flags`                 `unsigned`              初始化标志，普通使用通常传
                                                  `0`
  ----------------------------------------------------------------------------

### 返回值

-   `0`：成功
-   `< 0`：失败，返回负的错误码

### 作用

创建并初始化一个 `io_uring` 实例。

初始化后，`ring` 会关联：

``` text
Submission Queue (SQ)
        +
Completion Queue (CQ)
```

### 示例

``` cpp
io_uring ring;

int ret = io_uring_queue_init(64, &ring, 0);

if (ret < 0) {
    std::cerr << "queue init failed\n";
    return 1;
}
```

### 注意

`io_uring_queue_init()` 应该在使用其他 `io_uring` API 前调用。

使用结束后必须调用：

``` cpp
io_uring_queue_exit(&ring);
```

------------------------------------------------------------------------

# 2. `io_uring_get_sqe()`

### 函数原型

``` cpp
struct io_uring_sqe* io_uring_get_sqe(
    struct io_uring *ring
);
```

### 参数

  参数     类型                 作用
  -------- -------------------- -----------------------
  `ring`   `struct io_uring*`   已初始化的 `io_uring`

### 返回值

返回一个可用的：

``` cpp
struct io_uring_sqe*
```

即：

> Submission Queue Entry，提交队列项。

如果当前没有可用 SQE，可能返回：

``` cpp
nullptr
```

### 作用

从 SQ 中获取一个空闲的 SQE，用来描述一次 I/O 操作。

例如：

``` text
SQE

┌────────────────────┐
│ opcode = READ      │
│ fd = 10             │
│ buffer = buf        │
│ length = 4096       │
│ offset = 0          │
└────────────────────┘
```

### 示例

``` cpp
io_uring_sqe* sqe = io_uring_get_sqe(&ring);

if (!sqe) {
    std::cerr << "no SQE available\n";
    return 1;
}
```

------------------------------------------------------------------------

# 3. `io_uring_prep_read()`

### 函数原型

``` cpp
void io_uring_prep_read(
    struct io_uring_sqe *sqe,
    int fd,
    void *buf,
    unsigned nbytes,
    __u64 offset
);
```

### 参数

  参数       类型                     作用
  ---------- ------------------------ ------------------
  `sqe`      `struct io_uring_sqe*`   要填写的 SQE
  `fd`       `int`                    文件描述符
  `buf`      `void*`                  接收数据的缓冲区
  `nbytes`   `unsigned`               最多读取多少字节
  `offset`   `__u64`                  文件读取偏移

### 作用

把一个 `read` 操作填写到 SQE 中。

注意：

> `io_uring_prep_read()` 本身不会立即执行读取。

它只是：

``` text
填写 SQE
```

真正让内核处理，需要后续：

``` cpp
io_uring_submit(&ring);
```

### 示例

``` cpp
char buf[4096];

io_uring_sqe* sqe = io_uring_get_sqe(&ring);

io_uring_prep_read(
    sqe,
    fd,
    buf,
    sizeof(buf),
    0
);
```

------------------------------------------------------------------------

# 4. `io_uring_prep_write()`

### 函数原型

``` cpp
void io_uring_prep_write(
    struct io_uring_sqe *sqe,
    int fd,
    const void *buf,
    unsigned nbytes,
    __u64 offset
);
```

### 参数

  参数       类型                     作用
  ---------- ------------------------ --------------------
  `sqe`      `struct io_uring_sqe*`   要填写的 SQE
  `fd`       `int`                    文件描述符
  `buf`      `const void*`            待写入数据的缓冲区
  `nbytes`   `unsigned`               写入字节数
  `offset`   `__u64`                  文件写入偏移

### 作用

准备一个文件 `write` 操作。

同样：

``` cpp
io_uring_prep_write();
```

只是准备请求。

真正提交：

``` cpp
io_uring_submit();
```

### 示例

``` cpp
const char* msg = "hello io_uring\n";

io_uring_sqe* sqe = io_uring_get_sqe(&ring);

io_uring_prep_write(
    sqe,
    fd,
    msg,
    strlen(msg),
    0
);
```

------------------------------------------------------------------------

# 5. `io_uring_prep_recv()`

### 函数原型

``` cpp
void io_uring_prep_recv(
    struct io_uring_sqe *sqe,
    int sockfd,
    void *buf,
    unsigned len,
    unsigned flags
);
```

### 参数

  参数       类型                     作用
  ---------- ------------------------ ------------------------------------------
  `sqe`      `struct io_uring_sqe*`   要填写的 SQE
  `sockfd`   `int`                    Socket 文件描述符
  `buf`      `void*`                  接收数据的缓冲区
  `len`      `unsigned`               最大接收长度
  `flags`    `unsigned`               `recv()` 的 flags，例如 `MSG_WAITALL` 等

### 作用

准备一个 Socket 接收操作，相当于：

``` cpp
recv(sockfd, buf, len, flags);
```

### 示例

``` cpp
char buf[4096];

io_uring_sqe* sqe = io_uring_get_sqe(&ring);

io_uring_prep_recv(
    sqe,
    client_fd,
    buf,
    sizeof(buf),
    0
);
```

### CQE 中的结果

完成之后：

``` cpp
cqe->res
```

表示实际接收到的字节数。

例如：

``` text
cqe->res = 128
```

表示本次接收到 128 字节。

------------------------------------------------------------------------

# 6. `io_uring_prep_send()`

### 函数原型

``` cpp
void io_uring_prep_send(
    struct io_uring_sqe *sqe,
    int sockfd,
    const void *buf,
    unsigned len,
    unsigned flags
);
```

### 参数

  参数       类型                     作用
  ---------- ------------------------ -------------------
  `sqe`      `struct io_uring_sqe*`   要填写的 SQE
  `sockfd`   `int`                    Socket 文件描述符
  `buf`      `const void*`            待发送数据
  `len`      `unsigned`               最大发送长度
  `flags`    `unsigned`               `send()` 的 flags

### 作用

准备一个 Socket 发送操作。

对应传统：

``` cpp
send(sockfd, buf, len, flags);
```

### 示例

``` cpp
const char* msg = "hello client";

io_uring_sqe* sqe = io_uring_get_sqe(&ring);

io_uring_prep_send(
    sqe,
    client_fd,
    msg,
    strlen(msg),
    0
);
```

完成后：

``` cpp
cqe->res
```

表示实际发送的字节数。

------------------------------------------------------------------------

# 7. `io_uring_prep_accept()`

### 函数原型

``` cpp
void io_uring_prep_accept(
    struct io_uring_sqe *sqe,
    int fd,
    struct sockaddr *addr,
    socklen_t *addrlen,
    int flags
);
```

### 参数

  ----------------------------------------------------------------------------------
  参数                    类型                     作用
  ----------------------- ------------------------ ---------------------------------
  `sqe`                   `struct io_uring_sqe*`   要填写的 SQE

  `fd`                    `int`                    监听 Socket

  `addr`                  `struct sockaddr*`       用于保存客户端地址，可为
                                                   `nullptr`

  `addrlen`               `socklen_t*`             地址结构长度，可为 `nullptr`

  `flags`                 `int`                    `accept4()` 的 flags，例如
                                                   `SOCK_NONBLOCK`、`SOCK_CLOEXEC`
  ----------------------------------------------------------------------------------

### 作用

准备一个异步 `accept` 操作。

传统模型：

``` cpp
int client_fd = accept(
    listen_fd,
    ...
);
```

io_uring：

``` text
SQE
 │
 └── ACCEPT(listen_fd)
          ↓
       内核处理
          ↓
        CQE
          ↓
      cqe->res
          ↓
     client_fd
```

### 示例

``` cpp
io_uring_sqe* sqe = io_uring_get_sqe(&ring);

io_uring_prep_accept(
    sqe,
    listen_fd,
    nullptr,
    nullptr,
    SOCK_NONBLOCK
);
```

完成之后：

``` cpp
int client_fd = cqe->res;
```

如果：

``` text
client_fd >= 0
```

表示 accept 成功。

如果：

``` text
client_fd < 0
```

表示发生错误，此时它通常是负的 `-errno`。

------------------------------------------------------------------------

# 8. `io_uring_submit()`

### 函数原型

``` cpp
int io_uring_submit(
    struct io_uring *ring
);
```

### 参数

  参数     类型                 作用
  -------- -------------------- ---------------
  `ring`   `struct io_uring*`   io_uring 实例

### 返回值

成功时：

``` text
>= 0
```

表示提交给内核的 SQE 数量。

失败时：

``` text
< 0
```

表示负的错误码。

### 作用

把之前准备好的 SQE 提交给内核。

例如：

``` cpp
io_uring_get_sqe(&ring);
io_uring_prep_read(...);

io_uring_get_sqe(&ring);
io_uring_prep_read(...);

io_uring_get_sqe(&ring);
io_uring_prep_write(...);

io_uring_submit(&ring);
```

一次提交多个请求：

``` text
SQ

READ
READ
WRITE
  │
  │ submit
  ▼
Kernel
```

### 关键理解

``` cpp
io_uring_prep_read()
```

是：

> 准备请求

而：

``` cpp
io_uring_submit()
```

是：

> 把请求交给内核执行

------------------------------------------------------------------------

# 9. `io_uring_wait_cqe()`

### 函数原型

``` cpp
int io_uring_wait_cqe(
    struct io_uring *ring,
    struct io_uring_cqe **cqe_ptr
);
```

### 参数

  参数        类型                      作用
  ----------- ------------------------- ----------------
  `ring`      `struct io_uring*`        io_uring 实例
  `cqe_ptr`   `struct io_uring_cqe**`   输出完成的 CQE

### 返回值

-   `0`：成功获得 CQE
-   `< 0`：错误

### 作用

等待至少一个 I/O 操作完成。

例如：

``` cpp
io_uring_cqe* cqe;

int ret = io_uring_wait_cqe(
    &ring,
    &cqe
);

if (ret < 0) {
    // error
}
```

完成之后：

``` cpp
cqe->res
```

是 I/O 操作结果。

------------------------------------------------------------------------

# 10. `io_uring_peek_cqe()`

### 函数原型

``` cpp
int io_uring_peek_cqe(
    struct io_uring *ring,
    struct io_uring_cqe **cqe_ptr
);
```

### 参数

与 `io_uring_wait_cqe()` 基本一致。

### 区别

这是非常重要的区别：

``` text
wait_cqe
```

> 没有完成事件时可以等待。

而：

``` text
peek_cqe
```

> 只检查现在有没有已经完成的 CQE，不主动等待。

可以理解为：

``` text
wait_cqe()

没有 CQE
   ↓
等待
   ↓
有 CQE
   ↓
返回
```

而：

``` text
peek_cqe()

没有 CQE
   ↓
立即返回

有 CQE
   ↓
立即返回
```

### 示例

``` cpp
io_uring_cqe* cqe;

int ret = io_uring_peek_cqe(
    &ring,
    &cqe
);

if (ret == 0) {
    // 有完成事件
    handle(cqe);
}
```

适合自己控制事件循环，而不是一直阻塞等待。

------------------------------------------------------------------------

# 11. `io_uring_cqe_seen()`

### 函数原型

``` cpp
void io_uring_cqe_seen(
    struct io_uring *ring,
    struct io_uring_cqe *cqe
);
```

### 参数

  参数     类型                     作用
  -------- ------------------------ --------------------
  `ring`   `struct io_uring*`       io_uring 实例
  `cqe`    `struct io_uring_cqe*`   已经处理完成的 CQE

### 作用

告诉 io_uring：

> 这个 CQE 我已经处理完了，可以从 CQ 中回收。

典型流程：

``` cpp
io_uring_wait_cqe(&ring, &cqe);

handle(cqe);

io_uring_cqe_seen(&ring, cqe);
```

### 为什么必须调用？

CQ 是一个有限容量的 Ring Buffer。

如果一直不消费：

``` text
CQ

CQE
CQE
CQE
CQE
...
```

最终可能：

``` text
CQ 满
```

因此：

``` cpp
io_uring_cqe_seen()
```

就是消费 CQE。

------------------------------------------------------------------------

# 12. `io_uring_queue_exit()`

### 函数原型

``` cpp
void io_uring_queue_exit(
    struct io_uring *ring
);
```

### 参数

  参数     类型                 作用
  -------- -------------------- ------------------------
  `ring`   `struct io_uring*`   要销毁的 io_uring 实例

### 作用

释放 `io_uring_queue_init()` 创建的相关资源。

### 示例

``` cpp
io_uring_queue_exit(&ring);
```

通常在程序结束或者服务器关闭时调用。

------------------------------------------------------------------------

# 13. 一次完整 I/O

以 `read` 为例：

``` cpp
io_uring ring;

io_uring_queue_init(64, &ring, 0);

char buf[4096];

io_uring_sqe* sqe =
    io_uring_get_sqe(&ring);

io_uring_prep_read(
    sqe,
    fd,
    buf,
    sizeof(buf),
    0
);

io_uring_submit(&ring);

io_uring_cqe* cqe;

io_uring_wait_cqe(
    &ring,
    &cqe
);

if (cqe->res >= 0) {
    std::cout
        << "read bytes = "
        << cqe->res
        << '\n';
}

io_uring_cqe_seen(
    &ring,
    cqe
);

io_uring_queue_exit(&ring);
```

整个过程：

``` text
┌─────────────────────────────┐
│ io_uring_queue_init()       │
│ 初始化 SQ / CQ              │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│ io_uring_get_sqe()          │
│ 获取一个 SQE                │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│ io_uring_prep_read()        │
│ 填写 READ 请求              │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│ io_uring_submit()           │
│ 提交给内核                  │
└──────────────┬──────────────┘
               ↓
          Linux Kernel
               │
               │ 执行 read
               ↓
┌─────────────────────────────┐
│ Completion Queue             │
│             ↓               │
│          CQE                  │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│ io_uring_wait_cqe()         │
│ 获取完成结果                │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│ cqe->res                    │
│ 获取 read 结果              │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│ io_uring_cqe_seen()         │
│ 消费 CQE                    │
└──────────────┬──────────────┘
               ↓
┌─────────────────────────────┐
│ io_uring_queue_exit()       │
│ 销毁 io_uring               │
└─────────────────────────────┘
```

------------------------------------------------------------------------

# 14. `SQE` 和 `CQE`

理解 io_uring 最重要的两个结构：

## SQE

``` cpp
struct io_uring_sqe;
```

表示：

> 我要执行什么操作？

例如：

``` text
READ
WRITE
RECV
SEND
ACCEPT
```

------------------------------------------------------------------------

## CQE

``` cpp
struct io_uring_cqe;
```

表示：

> 这个操作执行结果是什么？

常用字段：

``` cpp
cqe->res
```

和：

``` cpp
cqe->user_data
```

其中：

### `res`

I/O 操作结果。

例如：

``` text
READ  → 实际读取字节数
WRITE → 实际写入字节数
ACCEPT → 新 client fd
错误 → -errno
```

### `user_data`

用于把完成事件与用户自己的对象关联起来。

例如：

``` cpp
sqe->user_data =
    reinterpret_cast<uint64_t>(conn);
```

完成后：

``` cpp
Connection* conn =
    reinterpret_cast<Connection*>(
        cqe->user_data
    );
```

服务器中非常重要。

------------------------------------------------------------------------

# 15. 这些 API 可以分成四类

## ① 生命周期

``` text
io_uring_queue_init()
io_uring_queue_exit()
```

负责：

``` text
创建
销毁
```

------------------------------------------------------------------------

## ② SQ 操作

``` text
io_uring_get_sqe()
io_uring_prep_read()
io_uring_prep_write()
io_uring_prep_recv()
io_uring_prep_send()
io_uring_prep_accept()
io_uring_submit()
```

负责：

``` text
获取 SQE
   ↓
填写请求
   ↓
提交请求
```

------------------------------------------------------------------------

## ③ CQ 操作

``` text
io_uring_wait_cqe()
io_uring_peek_cqe()
io_uring_cqe_seen()
```

负责：

``` text
等待/检查完成事件
   ↓
获取 CQE
   ↓
处理结果
   ↓
消费 CQE
```

------------------------------------------------------------------------

## ④ 请求类型

``` text
prep_read()
prep_write()
prep_recv()
prep_send()
prep_accept()
```

本质上是在告诉 SQE：

``` text
opcode = 什么？
参数 = 什么？
```

------------------------------------------------------------------------

# 16. 最容易混淆的三个接口

### `prep_xxx`

``` cpp
io_uring_prep_read()
```

只是：

``` text
准备请求
```

不会真正开始执行。

------------------------------------------------------------------------

### `submit`

``` cpp
io_uring_submit()
```

是：

``` text
把 SQ 中准备好的请求提交给内核
```

------------------------------------------------------------------------

### `wait_cqe`

``` cpp
io_uring_wait_cqe()
```

是：

``` text
等待内核完成请求
```

因此：

``` text
prep
 ↓
submit
 ↓
kernel execute
 ↓
CQE
 ↓
wait/peek
```

------------------------------------------------------------------------

# 17. 一个服务器中的典型模式

服务器通常不是只处理一个请求，而是：

``` text
                    ┌── READ A
                    │
                    ├── READ B
Application ── SQ ──┼── WRITE C
                    │
                    ├── ACCEPT D
                    │
                    └── RECV E
                         │
                         ▼
                       Kernel
                         │
                         ▼
                    ┌── CQE A
                    │
                    ├── CQE B
                    ├── CQE C
Application ── CQ ──┼── CQE D
                    │
                    └── CQE E
```

所以 io_uring 的核心就是：

``` text
            SQ
             │
             │ 请求
             ▼
        Linux Kernel
             │
             │ 完成
             ▼
            CQ
```

而：

``` text
SQE = 请求描述
CQE = 完成结果
```

这是学习 io_uring 时最重要的一组概念。

------------------------------------------------------------------------

# 18. 推荐学习顺序

对于已经掌握 Epoll/Reactor 的 C++ 开发者，可以按照：

``` text
① queue_init / queue_exit
            ↓
② get_sqe
            ↓
③ prep_read / prep_write
            ↓
④ submit
            ↓
⑤ wait_cqe
            ↓
⑥ cqe->res
            ↓
⑦ cqe_seen
            ↓
⑧ user_data
            ↓
⑨ recv / send / accept
            ↓
⑩ 批量 SQE / CQE
            ↓
⑪ SQPOLL
            ↓
⑫ registered buffers
            ↓
⑬ fixed files
            ↓
⑭ multishot accept / recv
            ↓
⑮ io_uring 内核源码
```

如果目标是深入服务器架构，尤其应该重点理解：

``` text
SQ/CQ Ring Buffer
        ↓
SQE/CQE
        ↓
user_data
        ↓
batch submit
        ↓
completion
        ↓
SQPOLL
        ↓
io-wq
        ↓
VFS / Socket
```

这些比单纯记住 `io_uring_prep_read()` 的函数参数更重要。
