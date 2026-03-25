#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<vector>
#include<queue>
#include<memory>
#include<atomic>
#include<mutex>
#include<condition_variable>
using namespace std;
class Semaphore
{
    public:
    Semaphore(int limit=0):reslimit_(limit) {}
    ~Semaphore()=default;
    void wait() {
        unique_lock<mutex> lock(mtx_);
        cond_.wait(lock,[&](){return reslimit_>0;});
        reslimit_--;
    }
    void post() {
        unique_lock<mutex> lock(mtx_);
        reslimit_++;
        cond_.notify_all();
    }
    private:
    mutex mtx_;
    condition_variable cond_;
    int reslimit_;
};

class Any {
    public:
    Any()=default;
    ~Any()=default;
    Any(const Any&)=delete;
    Any& operator=(const Any&)=delete;
    Any(Any&&)=default;
    Any& operator=(Any&&) = default;

    template<typename T>
    Any(T data):base_(make_unique<Derive<T>>(data)){}

    template<typename T>
    T cast_()
    {
        Derive<T> *pd=dynamic_cast<Derive<T>*>(base_.get());
        if(pd==NULL)
        {
            throw "type is incompatible";
        }
        else
        {
            return pd->data_;
        }
    }
    private:
    class Base {
    public:
        virtual ~Base()=default;
    };
    //derive
    template<typename T>
    class Derive: public Base {
        public:
        Derive(T data):data_(data){}
        T data_;
    };
    private:
    unique_ptr<Base> base_;
};
class Task;
class Result {
    public:
    Result(shared_ptr<Task> task,bool isValid=true);
    ~Result()=default;
    Any get();
    void setVal(Any any);

    private:
    Any any_;
    Semaphore sem_;
    shared_ptr<Task> task_;
    atomic_bool isValid_;
};
class Task {
    public:
    Task();
    ~Task()=default;
    void exec();
    virtual Any run()=0;
    void setResult(Result *res);
    private:
    Result* result_;
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
    Result submitTask(shared_ptr<Task> sp);//submit one task to the thread pool
    void start(int initThreadSize = 4);//start thread pool
    void setMode(PoolMode mode);
    void setInitThreadSize(int size);
    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool&)=delete;

    private:
    void threadFunc();//define thread function

    private:
    vector<unique_ptr<Thread>> threads_;//thread list
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