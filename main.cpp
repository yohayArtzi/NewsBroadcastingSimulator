#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <string.h>
#include <unistd.h>
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

void checkInputScanning(vector<int> id, vector<int> cap, vector<int> size, int ce);

void testsZone();

void produce(BoundedQueue *bq, int id, int amount);

string create_article(int id, const char **cat, int *counters);

void addToDispatcher(string article);

void dispatch(vector<BoundedQueue *> PRODUCERS,
              vector<UnboundedQueue *> DISPATCHER, int num_of_producers);

void coeditor(vector<UnboundedQueue *> DISPATCHER, BoundedQueue *COEDITORS, int i);

void display(BoundedQueue *COEDITORS, int num_of_producers, vector<BoundedQueue *> PRODUCERS,
             vector<UnboundedQueue *> DISPATCHER);

void deleteAllocatedMem(int num_of_producers, vector<BoundedQueue *> PRODUCERS,
                        vector<UnboundedQueue *> DISPATCHER, BoundedQueue *COEDITORS);


//int arr[3];


int main() {
    vector<BoundedQueue *> PRODUCERS;
    vector<UnboundedQueue *> DISPATCHER;
    BoundedQueue *COEDITORS;
    int num_of_producers;

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

    // initialize producers queues
    for (int i = 0; i < num_of_producers; i++) {
        BoundedQueue *bq = new BoundedQueue(size[i]);
        PRODUCERS.push_back(bq);
    }
    // create producers threads
    vector<thread> producersThreads;
    for (int i = 0; i < num_of_producers; i++) {
        thread producer(produce, PRODUCERS[i], id[i], amount[i]);
        producersThreads.push_back(move(producer));
    }
    // initialize dispatcher's queues
    UnboundedQueue *sports = new UnboundedQueue;
    UnboundedQueue *news = new UnboundedQueue;
    UnboundedQueue *weather = new UnboundedQueue;
    DISPATCHER.push_back(sports);
    DISPATCHER.push_back(news);
    DISPATCHER.push_back(weather);
    thread dispatcher(dispatch, PRODUCERS, DISPATCHER, num_of_producers);

    // initialize co-editors queue
    COEDITORS = new BoundedQueue(coEditor_size);
    thread coeditor1(coeditor, DISPATCHER, COEDITORS, 0);
    thread coeditor2(coeditor, DISPATCHER, COEDITORS, 1);
    thread coeditor3(coeditor, DISPATCHER, COEDITORS, 2);
    thread screenManager(display, COEDITORS, num_of_producers, PRODUCERS, DISPATCHER);

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

void deleteAllocatedMem(int num_of_producers, vector<BoundedQueue *> PRODUCERS,
                        vector<UnboundedQueue *> DISPATCHER, BoundedQueue *COEDITORS){
    for(int i=0;i<num_of_producers;i++){
        delete PRODUCERS[i];
    }
    for(int i=0; i<3;i++){
        delete DISPATCHER[i];
    }
    delete COEDITORS;
}

// display articles to the screen
void display(BoundedQueue *COEDITORS, int num_of_producers, vector<BoundedQueue *> PRODUCERS,
             vector<UnboundedQueue *> DISPATCHER) {
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
    deleteAllocatedMem(num_of_producers, PRODUCERS, DISPATCHER, COEDITORS);

    //cout << "DONES: " << doneCounter << endl;
    //cout << "in display: finished" << endl;
    return;
}

// move articles from dispatcher's queues to co-editors queue
void coeditor(vector<UnboundedQueue *> DISPATCHER, BoundedQueue *COEDITORS, int i) {
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
void addToDispatcher(vector<UnboundedQueue *> DISPATCHER, string article) {
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

void dispatch(vector<BoundedQueue *> PRODUCERS, vector<UnboundedQueue *> DISPATCHER, int num_of_producers) {
    //cout<<"**** inside dispatch ****"<<endl;
    // create queue for every category
    //UnboundedQueue *sports = new UnboundedQueue;
    //UnboundedQueue *news = new UnboundedQueue;
    //UnboundedQueue *weather = new UnboundedQueue;
    //DISPATCHER.push_back(sports);
    //DISPATCHER.push_back(news);
    //DISPATCHER.push_back(weather);

    int not_done = 1;
    while (not_done) {
        not_done = 0;
        //while(PRODUCERS.size() < num_of_producers);
        //int n = PRODUCERS.size();
        for (int i = 0; i < PRODUCERS.size(); i++) {
            string article = PRODUCERS[i]->remove();
            if (article.compare("DONE") != 0) {
                not_done = 1;
                addToDispatcher(DISPATCHER, article);
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
void produce(BoundedQueue *bq, int id, int amount) {
    //BoundedQueue *bq = new BoundedQueue(size);
    //PRODUCERS.push_back(bq);
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