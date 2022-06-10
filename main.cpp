#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include "BoundedQueue.h"
#include "UnboundedQueue.h"
#include <string.h>

using namespace std;

void print_debug(vector<int> id, vector<int> cap, vector<int> size, int ce);

void for_tests();

void produce(int id, int amount, int size);

string create_article(int id, const char **cat, int *counters);

void test_producer_queue(BoundedQueue *q, int size) {
    for (int i = 0; i < size; i++) {
        cout << (*q).remove() << endl;
    }
}

void foo(int a, int b) {
    cout << "What's up " << a << " or maybe " << b << "?" << endl;
}

vector<BoundedQueue *> PRODUCERS;
vector<UnboundedQueue *> DISPATCHER;

int main() {
    vector<int> id;  // producer ID
    vector<int> amount;  // amount of products to make
    vector<int> size;  // size fo producer's queue
    int coEditor_size = 0; // size of c-editor's queue

    // read data from config file
    fstream configStream;
    configStream.open("config.txt", ios::in);
    if (configStream.is_open()) {
        string line;
        // arrange data in vectors
        for (int i = 0; getline(configStream, line); i++) {
            // id
            if (0 == i % 4) {
                id.push_back(stoi(line));
                coEditor_size = stoi(line);
            }
            // num of products to produce
            if (1 == i % 4)
                amount.push_back(stoi(line));
            // size of queue
            if (2 == i % 4)
                size.push_back(stoi(line));
        }
        id.pop_back();

    }
    configStream.close(); //close the file object.

    print_debug(id, amount, size, coEditor_size);

    /*
    for (int i = 0; i < id.size(); i++)
        thread t(produce, amount[i], size[i]);
*/
    thread producer(produce, id[0], amount[0], size[0]);
    //thread dispatcher(dispatch);

    producer.join();



    //cout << "amount is: " << amount[0] << endl;
    //cout << "size is: " << size[0] << endl;
    //thread try1(foo, amount[0], size[0]);
    //try1.join();

    cout << "finished main" << endl;
    for_tests();

    return 0;
}

void dispatch(int size) {
    // create queue for every category
    UnboundedQueue *sports = new UnboundedQueue;
    UnboundedQueue *news = new UnboundedQueue;
    UnboundedQueue *weather = new UnboundedQueue;
    DISPATCHER.push_back(sports);
    DISPATCHER.push_back(news);
    DISPATCHER.push_back(weather);

    int not_done=1;
    while(not_done){
        for(int i=0; i<size;i++){
            string article = PRODUCERS[i]->remove();
        }
    }
}

// produce all products for a single producer
void produce(int id, int amount, int size) {
    BoundedQueue *bq = new BoundedQueue(size);
    PRODUCERS.push_back(bq);
    const char *categories[3] = {"SPORTS", "NEWS", "WEATHER"};
    int counters[3] = {0, 0, 0};
    for (int i = 0; i < amount; i++)
        (*bq).insert(create_article(id, categories, counters));
    (*bq).insert(create_article(-1, categories, counters));
    //test_producer_queue(bq, size);
}

// create single article randomly
string create_article(int id, const char **cat, int *counters) {
    int random = rand() % 3;

    string str = "Producer ";
    str.append(to_string(id));
    str.append(" ");
    str.append(cat[random]);
    str.append(" ");
    str.append(to_string(counters[random]));
    counters[random]++;

    cout << str << endl;  // for debug
    return str;
}

void print_debug(vector<int> id, vector<int> cap, vector<int> size, int ce) {
    if (id.size() != cap.size() && id.size() != size.size())
        cout << "size problem" << endl;

    for (int i = 0; i < id.size(); i++) {
        cout << "PRODUCER " << id[i] << endl;
        cout << "num of products: " << cap[i] << endl;
        cout << "queue size: " << size[i] << endl;
        cout << endl;
    }
    cout << "co editor queue size: " << ce << endl;

    cout << "----- finished print_debug -----" << endl;
}

void for_tests() {
    cout << "----- TESTS ZONE -----" << endl;

/*
    // article creation tests
    const char *categories[3] = {"SPORTS", "NEWS", "WEATHER"};
    int counters[3] = {0, 0, 0};
    for (int i = 0; i < 18; i++)
        create_article(15, categories, counters);
    create_article(-1, categories, counters);

    // queue's test
    BoundedQueue q(15);
    for (int i = 0; i < 15; i++) {
        string str = "Element ";
        str.append(to_string(i));
        cout << str << endl;
        q.insert(str);
    }

    for (int i = 0; i < 15; i++) {
        cout << q.remove() << endl;

*/

}