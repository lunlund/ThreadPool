#include "threadpool.h"
#include<functional>
#include<thread>
#include<iostream>
const int TASK_MAX_THRESHHOLD = 4;
ThreadPool::ThreadPool():initThreadSize_(4),taskSize_(0),taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD),poolMode_(PoolMode::MODE_FIXED),isPoolRunning(0),killedThreadSize_(0) {

}
ThreadPool::~ThreadPool() {
    isPoolRunning=0;
    unique_lock<mutex> lock(taskQueMtx_);
    notEmpty_.notify_all();
    exitCond_.wait(lock,[&](){ return killedThreadSize_==initThreadSize_;});
}

void ThreadPool::setTaskQueMaxThreshHold(int threshhold){
    taskQueMaxThreshHold_=threshhold;
}
Result ThreadPool::submitTask(shared_ptr<Task> sp){
    //get lock
    unique_lock<mutex> lock(taskQueMtx_);
    //communication among threads, wait until the task queue is availbale
    //the blocked time can not exceed 1s, otherwise return "submit failure error" to user
    if(!notFull_.wait_for(lock,chrono::seconds(1),[&](){return taskQue_.size()<taskQueMaxThreshHold_;}))
    {
        //the condition is not met
        cerr<<"task queue is full, submitting task fails"<<endl;
        return Result(sp,false);
    }
    //if available, push back this task
    taskQue_.emplace(sp);
    taskSize_++;
    //if pushed successfully, notify all threads waiting for notEmpty_
    notEmpty_.notify_all();
    return Result(sp);
}
void ThreadPool::start(int initThreadSize){
    isPoolRunning=1;
    initThreadSize_=initThreadSize;
        //create thread object
    for(int i=0;i<initThreadSize_;i++) {
        auto ptr = make_unique<Thread>(bind(&ThreadPool::threadFunc,this));
        threads_.emplace_back(std::move(ptr));//when creating thread object, we need pass a thread function to thread object
    }
    //start all threads
    for(int i=0;i<initThreadSize_;i++) {
        threads_[i]->start();//need a thread function
    }
}
void ThreadPool::setMode(PoolMode mode) {
    poolMode_=mode;
}
void ThreadPool::setInitThreadSize(int size){
    //record the number of threads 
    initThreadSize_=size;
}
//get task from task queue, and assign this one thread to one task
void ThreadPool::threadFunc() {
    while(isPoolRunning) {
        shared_ptr<Task> task;
        {
            //get lock first
            unique_lock<mutex> lock(taskQueMtx_);
            //wait for not empty condition
            notEmpty_.wait(lock,[&](){return taskQue_.size()>0||!isPoolRunning;});
            if(!isPoolRunning) break;
            //pop one task from task queue
            task=taskQue_.front();
            taskQue_.pop();
            taskSize_--;
            if(taskQue_.size()>0) {
                notEmpty_.notify_all();
            }
            notFull_.notify_all();
        }
        //current thread to perform this task
        if(task!=NULL) {
            //task->run();//move the return value to Result
            task->exec();
        }
    }
    unique_lock<mutex> lock(taskQueMtx_);
    killedThreadSize_++;
    exitCond_.notify_all();
    notEmpty_.notify_all();
}
/////////thread implementation
Thread::Thread(ThreadFunc func):func_(func) {

}
Thread::~Thread(){

}
//start thread
void Thread::start() {
    thread t(func_);
    t.detach();
}
//////Task implementation
Task::Task():result_(NULL){}

void Task::exec(){
    if(result_!=NULL) 
    {
        result_->setVal(run());
    }
}
void Task::setResult(Result* res)
{
    result_=res;
}


/////Result implementation
Result::Result(shared_ptr<Task> task,bool isValid):task_(task),isValid_(isValid) {
    task_->setResult(this);
}
Any Result::get()//user call this func
{
    if(!isValid_)
    {
        return "";
    }
    else
    {
        sem_.wait();//if the task is not finished, it will be blocked here
        return std::move(any_);
    }
}
//
void Result::setVal(Any any) {
    //store the return value of a task
    this->any_=std::move(any);
    sem_.post();
}