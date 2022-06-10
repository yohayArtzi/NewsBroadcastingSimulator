#ifndef EX3_UNBOUNDEDQUEUE_H
#define EX3_UNBOUNDEDQUEUE_H


#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>

using namespace std;

class UnboundedQueue{
    sem_t full;
    mutex mtx;
    queue<string> q;

public:
    UnboundedQueue(){
        sem_init(&this->full, 0, 0);
    }

    // atomic insert
    void insert (string str){
        mtx.lock();
        this->q.push(str);
        mtx.unlock();
        sem_post(&this->full);
    }

    // atomic remove
    string remove () {
        sem_wait(&this->full);
        mtx.lock();
        string str = this->q.front();
        this->q.pop();
        mtx.unlock();

        return str;
    }

    string front (){
        mtx.lock();
        string str = this->q.front();
        mtx.unlock();

        return str;
    }
};


#endif //EX3_UNBOUNDEDQUEUE_H
