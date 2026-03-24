#include "threadpool.h"
#include<functional>
#include<thread>
#include<iostream>
const int TASK_MAX_THRESHHOLD = 1024;
ThreadPool::ThreadPool():initThreadSize_(4),taskSize_(0),taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD),poolMode_(PoolMode::MODE_FIXED) {

}
ThreadPool::~ThreadPool() {

}

void ThreadPool::setTaskQueMaxThreshHold(int threshhold){
    taskQueMaxThreshHold_=threshhold;
}
void ThreadPool::submitTask(shared_ptr<Task> sp){

}
void ThreadPool::start(int initThreadSize){
    initThreadSize_=initThreadSize;
        //create thread object
    for(int i=0;i<initThreadSize_;i++) {
        threads_.emplace_back(new Thread(bind(&ThreadPool::threadFunc,this)));//when creating thread object, we need pass a thread function to thread object
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
void ThreadPool::threadFunc() {
    cout<<"begin threadFunc id"<<this_thread::get_id()<<endl;
    cout<<"end threadFunc id"<<this_thread::get_id()<<endl;
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