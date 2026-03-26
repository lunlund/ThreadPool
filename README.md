High-Performance C++17 Thread Pool
A lightweight, asynchronous task execution framework built with Modern C++17. This project implements a robust thread-safe task queue and provides a seamless interface for concurrent programming, utilizing the Producer-Consumer design pattern.

🚀 Key Features
Asynchronous Task Submission: Seamlessly submit tasks and retrieve return values from background execution using a custom Result mechanism.

RAII-Compliant Resource Management: Ensures strict lifecycle control; all worker threads are safely joined, and resources are reclaimed automatically upon pool destruction.

Efficient Synchronization: Utilizes std::mutex and std::condition_variable to minimize CPU wake-up cycles and optimize task distribution.

Custom Type Erasure: Implements a custom Any class to handle diverse task return types without the overhead of making the entire ThreadPool a template class.

🏗 Architecture
The project follows a classic Producer-Consumer design pattern to manage concurrency:

Producer: The main thread (or any caller) submits tasks via submitTask().

Task Queue: A thread-safe, bounded queue stores tasks wrapped in std::shared_ptr.

Consumers: A set of worker threads that continuously pull and execute tasks from the queue.

🛠 Implementation Details
Type Erasure with Any
To store different return types in a unified Result class, the project implements a Type Erasure pattern. This architectural choice avoids making the ThreadPool a template class, keeping header files clean and significantly reducing compilation times.

Synchronization Logic
The pool manages thread contention using std::unique_lock and std::condition_variable:

Full Queue: The producer waits on the notFull_ condition.

Empty Queue: Worker threads wait on the notEmpty_ condition.

⏱ Quick Start
Prerequisites
C++17 Compatible Compiler: GCC 7+, Clang 5+, or MSVC 2017+

CMake: version 3.10 or higher

Compilation
You can compile the project using the following command:

Bash
g++ -std=c++17 main.cpp threadpool.cpp -o threadpool -lpthread
./threadpool
💻 Basic Usage
To use the thread pool, define your own task by inheriting from the Task base class and overriding the run() method.

C++
#include "threadpool.h"

// 1. Define your custom task
class MyTask : public Task {
public:
    Any run() override {
        // Your logic here...
        return 42; 
    }
};

int main() {
    // 2. Initialize the pool
    ThreadPool pool;
    pool.start(4); // Start with 4 worker threads

    // 3. Submit task and retrieve result
    Result res = pool.submitTask(std::make_shared<MyTask>());
    int val = res.get().cast_<int>();

    return 0;
}
