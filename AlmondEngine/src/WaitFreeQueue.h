#pragma once

#include <atomic>
#include <vector>
#include <optional>
#include <iostream>

namespace almond {

template<typename T>
class WaitFreeQueue {
public:
    explicit WaitFreeQueue(size_t capacity);

    bool enqueue(const T& item); // Add an item to the queue
    bool dequeue(T& item);       // Remove an item from the queue
    bool isEmpty() const;        // Check if the queue is empty

private:
    std::vector<std::optional<T>> buffer;  // Buffer for queue items
    alignas(64) std::atomic<size_t> tail;  // Tail index
    alignas(64) std::atomic<size_t> head;  // Head index
};

template<typename T>
WaitFreeQueue<T>::WaitFreeQueue(size_t capacity)
    : buffer(capacity), head(0), tail(0) {
    if (capacity == 0) {
        throw std::invalid_argument("Capacity must be greater than zero.");
    }
}

template<typename T>
bool WaitFreeQueue<T>::enqueue(const T& item) {
    size_t currentTail = tail.load(std::memory_order_acquire);  // Ensure proper synchronization on load
    size_t nextTail = (currentTail + 1) % buffer.size();        // Calculate next tail position

    // Check if the queue is full
    if (nextTail == head.load(std::memory_order_acquire)) {
        std::cerr << "Queue is full!" << std::endl;
        return false; // Queue full
    }

    buffer[currentTail] = item;                                 // Enqueue the item
    tail.store(nextTail, std::memory_order_release);            // Update tail with memory barrier
    return true;
}

template<typename T>
bool WaitFreeQueue<T>::dequeue(T& item) {
    size_t currentHead = head.load(std::memory_order_relaxed); // Load head index

    // Check if the queue is empty
    if (currentHead == tail.load(std::memory_order_acquire)) {
        //std::cerr << "Queue is empty!" << std::endl;
        return false; // Queue empty
    }

    // Check if there's a value to dequeue
    if (buffer[currentHead].has_value()) {
        // Move the item from the buffer
        item = std::move(*buffer[currentHead]);

        // Clear the value in the buffer
        buffer[currentHead].reset();

        // Update head and ensure proper memory visibility
        head.store((currentHead + 1) % buffer.size(), std::memory_order_release);
        return true; // Successfully dequeued
    }

    std::cerr << "No valid item to dequeue at current head!" << std::endl;
    return false; // No valid item to dequeue
}

template<typename T>
bool WaitFreeQueue<T>::isEmpty() const {
    return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
}
} // namespace almond
