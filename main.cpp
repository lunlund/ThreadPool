#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

#include "threadpool.h"
int main() {
    ThreadPool pool;
    pool.start(6);
    this_thread::sleep_for(chrono::seconds(5));
}