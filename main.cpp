#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include "BoundedQueue.h"
#include "UnboundedQueue.h"

using namespace std;

void print_debug(vector<int> id, vector<int> cap, vector<int> size, int ce);

void for_tests();

void produce(int amount, int size);

char *create_article(int id, const char **cat, int *counters);

vector<BoundedQueue *> Producers;

int main() {
    vector<int> id;  // producer ID
    vector<int> amount;  // amount of products to make
    vector<int> size;  // size fo queue
    int coEditor_size = 0;

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

    cout << "finished main" << endl;
    for_tests();

    return 0;
}

// produce all products for a single producer
void produce(int id, int amount, int size) {
    BoundedQueue *bq = new BoundedQueue(size);
    Producers.push_back(bq);
    const char *categories[3] = {"SPORTS", "NEWS", "WEATHER"};
    int counters[3] = {0, 0, 0};
    for (int i = 0; i < amount; i++)
        (*bq).insert(create_article(id, categories, counters));
    (*bq).insert(create_article(-1, categories, counters));
}

char *create_article(int id, const char **cat, int *counters) {
    int random = rand() % 3;

    string str = "Producer ";
    str.append(to_string(id));
    str.append(" ");
    str.append(cat[random]);
    str.append(" ");
    str.append(to_string(counters[random]));
    counters[random]++;

    cout << str << endl;  // for debug
    return &str[0];
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
    const char *categories[3] = {"SPORTS", "NEWS", "WEATHER"};
    int counters[3] = {0, 0, 0};
    for (int i = 0; i < 18; i++)
        create_article(15, categories, counters);
    create_article(-1, categories, counters);
    */


    BoundedQueue q(20);
    for (int i = 0; i < 15; i++) {
        string str = "Element ";
        str.append(to_string(i));
        cout << str << endl;
        q.insert(&str[0]);
    }

    for (int i = 0; i < 15; i++) {
        cout << q.front() << endl;
        q.remove();
        //char *chs1 = q.remove();
        //cout << chs1 << endl;
    }

    string str = "Hello";
    cout << str << " 1" << endl;
    char *chs = &str[0];
    cout << chs << " 2" << endl;
    cout << &str[0] << " 3" << endl;

}