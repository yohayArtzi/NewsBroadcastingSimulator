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
    void insert (char *s){
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

        // return after converting string to char*
        return &str[0];
    }
};


#endif //EX3_UNBOUNDEDQUEUE_H
