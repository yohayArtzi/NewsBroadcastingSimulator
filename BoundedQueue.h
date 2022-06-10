#ifndef EX3_BOUNDEDQUEUE_H
#define EX3_BOUNDEDQUEUE_H


#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>

using namespace std;

class BoundedQueue{
    sem_t full;
    sem_t empty;
    mutex mtx;
    queue<string> q;

public:
    BoundedQueue(int size){
        sem_init(&this->full, 0, 0);
        sem_init(&this->empty, 0, size);
    }

    // atomic insert
    void insert (string str){
        sem_wait(&this->empty);
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
        sem_post(&this->empty);

        return str;
    }

    string front (){
        mtx.lock();
        string str = this->q.front();
        mtx.unlock();

        return str;
    }
};

#endif //EX3_BOUNDEDQUEUE_H
