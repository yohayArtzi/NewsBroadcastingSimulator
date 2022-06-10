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
    void insert (char *s){
        sem_wait(&this->empty);
        mtx.lock();
        this->q.push(s);
        mtx.unlock();
        sem_post(&this->full);
    }

    // atomic remove
    char* remove () {
        sem_wait(&this->full);
        mtx.lock();
        string str = q.front();
        this->q.pop();
        mtx.unlock();
        sem_post(&this->empty);

        // return after converting string to char*


        return &str[0];
    }

    string front (){
        mtx.lock();
        string str = q.front();
        mtx.unlock();

        return str;
    }
};

#endif //EX3_BOUNDEDQUEUE_H
