#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

#include "threadpool.h"
using uLong = unsigned long long;
class MyTask: public Task {
    public:
    MyTask(int begin,int end):begin_(begin),end_(end){}
    Any run() {
        cout<<"tid:"<<this_thread::get_id()<<"begin"<<endl;
        uLong sum=0;
        for(uLong i=begin_;i<=end_;i++)
        {
            sum+=i;
        }
        cout<<"tid:"<<this_thread::get_id()<<"end"<<endl;
        return sum;
    }
    private:
    int begin_;
    int end_;
};
int main() {
    for(int i=1;i<=1000;i++)
    {ThreadPool pool;
    pool.start(5);
    Result res1=pool.submitTask(make_shared<MyTask>(1,10000000));
    Result res2=pool.submitTask(make_shared<MyTask>(10000001,20000000));
    Result res3=pool.submitTask(make_shared<MyTask>(20000001,30000000));
    uLong sum1=res1.get().cast_<uLong>();
    uLong sum2=res2.get().cast_<uLong>();
    uLong sum3=res3.get().cast_<uLong>();
    cout<<(sum1+sum2+sum3)<<endl;}
    //getchar();
}