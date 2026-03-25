# ThreadPool
HOW TO USE IT
ThreadPool pool;
pool.start(4);
class MyTask: public Task {
    public:
    void run(){
        //your code here
    }
}
pool.submitTask(make_shared<MyTask>());