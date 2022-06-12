#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include "BoundedQueue.h"
#include "UnboundedQueue.h"
#include <string.h>
#include <unistd.h>

using namespace std;

void checkInputScanning(vector<int> id, vector<int> cap, vector<int> size, int ce);

void testsZone();

void produce(int id, int amount, int size);

string create_article(int id, const char **cat, int *counters);

void addToDispatcher(string article);

void dispatch();

void coeditor(int i);

void display();

vector<BoundedQueue *> PRODUCERS;
vector<UnboundedQueue *> DISPATCHER;
BoundedQueue *COEDITORS;
int num_of_producers;
//int arr[3];


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
    configStream.close();

    //checkInputScanning(id, amount, size, coEditor_size);

    num_of_producers = id.size();

    // create producers threads
    vector<thread> producersThreads;
    for (int i = 0; i < num_of_producers; i++) {
        thread producer(produce, id[i], amount[i], size[i]);
        producersThreads.push_back(move(producer));
    }

    thread dispatcher(dispatch);

    COEDITORS = new BoundedQueue(coEditor_size);
    thread coeditor1(coeditor, 0);
    thread coeditor2(coeditor, 1);
    thread coeditor3(coeditor, 2);
    thread screenManager(display);

    // wait to other threads to complete
    for (int i = 0; i < producersThreads.size(); i++)
        producersThreads[i].join();
    dispatcher.join();
    coeditor1.join();
    coeditor2.join();
    coeditor3.join();
    screenManager.join();

    // check balance produced/delivered
/*    cout << "sports article produced " << arr[0] << ", delivered " << s << endl;
    cout << "news produced " << arr[1] << ", delivered " << n << endl;
    cout << "weather produced " << arr[2] << ", delivered " << w << endl;

    cout << "----------FINISHED MAIN----------" << endl;*/
    //testsZone();

    return 0;
}

void deleteGlobalMem(){
    for(int i=0;i<num_of_producers;i++){
        delete PRODUCERS[i];
    }
    for(int i=0; i<3;i++){
        delete DISPATCHER[i];
    }
    delete COEDITORS;
}

// display articles to the screen
void display() {
    //cout<<"**** inside display ****"<<endl;
    int doneCounter = 0;
    string article = COEDITORS->remove();
    while (true) {
        while (article.compare("DONE") != 0) {
            cout << article << endl;
            article = COEDITORS->remove();
        }
        doneCounter++;
        if(doneCounter == 3)
            break;
        article = COEDITORS->remove();
    }
    cout<<"DONE"<<endl;
    //void deleteGlobalMem();

    //cout << "DONES: " << doneCounter << endl;
    //cout << "in display: finished" << endl;
    return;
}

// move articles from dispatcher's queues to co-editors queue
void coeditor(int i) {
    //cout<<"**** inside co-editor ****"<<endl;
    while (DISPATCHER.size() < 3);
    //cout<<"after waiting to all dispatchers"<<endl;
    string article = DISPATCHER[i]->remove();
    //cout<<"COEDITOR "<<i<<" first article"<<endl;
    while (article.compare("DONE") != 0) {
        sleep(0.1);  // simulating editing process
        //cout<<"ARTICLE "<<article<< " moved to"<<endl;
        COEDITORS->insert(article);
        article = DISPATCHER[i]->remove();
    }
    COEDITORS->insert("DONE");
    //cout << "in coeditor " << i << ": finished" << endl;
    return;
}

// add article to appropriate dispatcher's queue
void addToDispatcher(string article) {
    while(DISPATCHER.size() < 3);

    // check if article belongs to SPORTS
    size_t found = article.find("SPORTS");
    if (found != string::npos) {
        DISPATCHER[0]->insert(article);
        //cout << "Inserted to the \"S dispatcher queue\"" << endl;
        return;
    }
    // check if article belongs to NEWS
    found = article.find("NEWS");
    if (found != string::npos) {
        DISPATCHER[1]->insert(article);
        //cout << "Inserted to the \"N dispatcher queue\"" << endl;
        return;
    }
    // check if article belongs to WEATHER
    found = article.find("WEATHER");
    if (found != string::npos) {
        DISPATCHER[2]->insert(article);
        //cout << "Inserted to the \"W dispatcher queue\"" << endl;
    }
    return;
}

void dispatch() {
    //cout<<"**** inside dispatch ****"<<endl;
    // create queue for every category
    UnboundedQueue *sports = new UnboundedQueue;
    UnboundedQueue *news = new UnboundedQueue;
    UnboundedQueue *weather = new UnboundedQueue;
    DISPATCHER.push_back(sports);
    DISPATCHER.push_back(news);
    DISPATCHER.push_back(weather);

    int not_done = 1;
    while (not_done) {
        not_done = 0;
        while(PRODUCERS.size() < num_of_producers);
        for (int i = 0; i < num_of_producers; i++) {
            string article = PRODUCERS[i]->remove();
            if (article.compare("DONE") != 0) {
                not_done = 1;
                addToDispatcher(article);
            } else
                PRODUCERS[i]->insert(article);
        }
    }
    // add an "DONE" at the end of every dispatcher queue
    for (int i = 0; i < 3; i++) {
        DISPATCHER[i]->insert("DONE");
    }
    //cout << "in dispatch: finished" << endl;
    return;
}

// produce all products for a single producer
void produce(int id, int amount, int size) {
    BoundedQueue *bq = new BoundedQueue(size);
    PRODUCERS.push_back(bq);
    const char *categories[3] = {"SPORTS", "NEWS", "WEATHER"};
    int counters[3] = {0, 0, 0};
    for (int i = 0; i < amount; i++) {
        (*bq).insert(create_article(id, categories, counters));
/*        if (i == 0)
            cout << "producer " << id << " produced his first product" << endl;*/
    }
    (*bq).insert("DONE");

/*    if (num_of_producers - 1 == id)
        cout << "in produce: finished" << endl;*/
}

// create single article randomly
string create_article(int id, const char **cat, int *counters) {
    int random = rand() % 3;
    //arr[random]++;

    string str = "Producer ";
    str.append(to_string(id));
    str.append(" ");
    str.append(cat[random]);
    str.append(" ");
    str.append(to_string(counters[random]));
    counters[random]++;

    //cout << str << endl;  // for debug
    return str;
}

// check scanning of input
void checkInputScanning(vector<int> id, vector<int> cap, vector<int> size, int ce) {
    if (id.size() != cap.size() && id.size() != size.size())
        cout << "size problem" << endl;

    for (int i = 0; i < id.size(); i++) {
        cout << "PRODUCER " << id[i] << endl;
        cout << "num of products: " << cap[i] << endl;
        cout << "queue size: " << size[i] << endl;
        cout << endl;
    }
    cout << "co editor queue size: " << ce << endl;

    cout << "----- finished checkInputScanning -----" << endl;
}

void testsZone() {
    cout << "----- TESTS ZONE -----" << endl;
    return;
}