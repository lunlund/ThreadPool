#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<vector>
#include<queue>
#include<memory>
#include<atomic>
#include<mutex>
#include<condition_variable>
using namespace std;
class Task {
    public:
    virtual void run()=0;
};
enum class PoolMode {
    MODE_FIXED,
    MODE_CHCHED,
};
class Thread {
    public:
    using ThreadFunc = function<void()>;
    //start the thread
    void start();
    Thread(ThreadFunc func);
    ~Thread();
    private:
    ThreadFunc func_;
};
class ThreadPool {
    public:
    ThreadPool();
    ~ThreadPool();

    void setTaskQueMaxThreshHold(int threshhold);
    void submitTask(shared_ptr<Task> sp);//submit one task to the thread pool
    void start(int initThreadSize = 4);//start thread pool
    void setMode(PoolMode mode);
    void setInitThreadSize(int size);
    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool&)=delete;

    private:
    void threadFunc();//define thread function

    private:
    vector<Thread*> threads_;//thread list
    size_t initThreadSize_;

    queue<shared_ptr<Task>> taskQue_;
    atomic_int taskSize_;
    int taskQueMaxThreshHold_; 
    mutex taskQueMtx_;//ensuring taskQueue thread security

    condition_variable notFull_;//taskQueue is not full
    condition_variable notEmpty_;//taskQueue is not empty

    PoolMode poolMode_;//the current working mode of the thread pool

};

#endif