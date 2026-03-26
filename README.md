High-Performance C++17 Thread Pool
A lightweight, asynchronous task execution framework built with Modern C++17. This project implements a robust thread-safe task queue and provides a seamless interface for concurrent programming.

Key Features
Asynchronous Task Submission: Allow users to retrieve return values from tasks executed in background threads.

RAII-Compliant Resource Management: Ensures strict lifecycle control; all worker threads are safely joined, and resources are reclaimed upon pool destruction.

Efficient Synchronization: Utilizes std::mutex and std::condition_variable to minimize CPU wake-up cycles and optimize task distribution.

Custom Type Erasure: Implements a custom Any class and Result mechanism to handle diverse task return types without compromising type safety.

Architecture
The project follows the Producer-Consumer design pattern:

Producer: The main thread submits tasks via submitTask().

Task Queue: A thread-safe queue stores tasks (wrapped in std::shared_ptr<Task>).

Consumers: A fixed/dynamic set of worker threads continuously pull and execute tasks from the queue.

Quick Start
Prerequisites
C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
CMake 3.10 or higher
Command: 
g++ -std=c++17 main.cpp threadpool.cpp -o threadpool
./threadpool

Basic Usage
See main.cpp
You need to define your own MyTask derived from Task, and overwrite your run() function. Then you can use threadpool to submit your task and use Result object to retrieve the result.

Implementation Details
Type Erasure with Any
To store different return types in a unified Result class, the project implements a Type Erasure pattern. This avoids making the entire ThreadPool a template class, which keeps the header files clean and reduces compilation times.

Synchronization Logic
The pool manages thread contention using a unique_lock with condition_variable. When the task queue is full, the producer waits on notFull_; when the queue is empty, workers wait on notEmpty_.
