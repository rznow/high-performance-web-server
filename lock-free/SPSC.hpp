#pragma once

#include <atomic>
#include <cstddef>
#include <utility>
#include <array>

/*
 *  memory_order_relaxed
 *  ─────────────────────
 *  保证：
 *    ✔ 当前操作的原子性（不会读到半写状态）
 *    ✔ 同一原子变量的修改顺序一致性（所有线程看到的顺序相同）
 *
 *  不保证：
 *    ✘ 编译器 / CPU 不对其前后的操作重排
 *    ✘ 与其他变量之间的顺序关系
 *    ✘ 任何跨线程的 happens-before 同步
 *
 *  适用：独立计数器、统计量等"只关心自己值、不关心顺序"的场景。
 */

/*
 *  memory_order_acquire   (用于 load / RMW)
 *  ─────────────────────────────────────────
 *  保证：
 *    ✔ 此操作【之后】的所有读写，不允许重排到此操作【之前】
 *    ✔ 与某个 release store 配对时，建立 happens-before：
 *      看到 release 写的值 ⟹ 也看到 release 之前的所有写
 *
 *  不保证：
 *    ✘ 不约束【前面】的操作（那是 release 的活）
 *    ✘ 单独一个 acquire 不构成完整同步，必须配对 release
 *
 *  适用：读"发布信号"的一方。
 *        例：消费者读 tail_，看到新值就保证能读到生产者写的 buffer。
 */

/*
 *  memory_order_release   (用于 store / RMW)
 *  ─────────────────────────────────────────
 *  保证：
 *    ✔ 此操作【之前】的所有读写，不允许重排到此操作【之后】
 *    ✔ 发布：把"此操作之前的所有写"对配对 acquire 的一方可见
 *
 *  不保证：
 *    ✘ 不约束【后面】的操作（那是 acquire 的活）
 *    ✘ 单独一个 release 不构成完整同步，必须配对 acquire
 *
 *  适用：写"发布信号"的一方。
 *        例：生产者写完 buffer_ 后 release 写 tail_，消费者 acquire 读 tail_。
 */


template <class T, size_t Capacity>
class SPSCQueue
{
        static_assert(Capacity > 0);
    public:
        SPSCQueue() : head_(0), tail_(0){}
        
        bool Enqueue(T value) {  
            size_t tail = tail_.load(std::memory_order_relaxed);  
            size_t next_tail = (tail + 1) % Capacity;  
            if (next_tail == head_.load(std::memory_order_acquire)) { // 队满
                return false;  
            }  
            buffer_[tail] = std::move(value);  
            tail_.store(next_tail, std::memory_order_release); // 发布新tail  
            return true;  
        }  

        bool Dequeue(T& value) {  
            size_t head = head_.load(std::memory_order_relaxed);  
            if (head == tail_.load(std::memory_order_acquire)) { // 队空  
                return false;  
            }  
            value = std::move(buffer_[head]);  
            head_.store((head + 1) % Capacity, std::memory_order_release);  
            return true;  
        }  

    private:
        std::array<T, Capacity> buffer_;
        alignas(64) std::atomic<size_t> head_;
        alignas(64) std::atomic<size_t> tail_;
};