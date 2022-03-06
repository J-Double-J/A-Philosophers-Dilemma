//
//  main.cpp
//  PhilosopherDilemma
//
//  Created by Joshua Jacobs on 11/10/21.
//

//There's some things I could do different here: CONSTANTS, and segment the code in a neater method, there is a fair amount repeated.

#include <iostream>
#include <mutex>
#include <getopt.h>
#include <thread>
#include <vector>

using namespace std;

void HandleOptions(uint32_t, char**, vector<mutex*>&);
void Philosopher(vector<mutex*>&);
void PhilosopherPlay(vector<mutex*>&, uint16_t&);
void TryMethod(vector<mutex*>&);
void TryPlay(vector<mutex*>&, uint16_t&);
void OtherMethod(vector<mutex*>&);
void OtherPlay(vector<mutex*>&, uint16_t&);

mutex out;

int main(int argc, char ** argv) {
    
    mutex mt0, mt1, mt2, mt3, mt4;
    vector<mutex*> fluggelhorns;
    fluggelhorns.push_back(&mt0);
    fluggelhorns.push_back(&mt1);
    fluggelhorns.push_back(&mt2);
    fluggelhorns.push_back(&mt3);
    fluggelhorns.push_back(&mt4);
    
    HandleOptions(argc, argv, fluggelhorns);
    while (1);
    return 0;
}

void HandleOptions(uint32_t argc, char** argv, vector<mutex*>& fluggelhorns) {
    int c;
    while ((c = getopt(argc, argv, "gtoh")) != -1) {
        switch (c) {
            //Philosopher's
            case 'g':
                Philosopher(fluggelhorns); //replace these with gen case
                break;
            //try_lock
            case 't':
                TryMethod(fluggelhorns); //replace with gen case
                break;
            //other
            case 'o':
                OtherMethod(fluggelhorns);
                break;
            case 'h':
            default:
                cerr << "Usage:\n";
                cerr << "-g     Using Philosopher's Algorithm, will lock." << endl;
                cerr << "-t     Using a try_lock based solution, will not lock." << endl;
                cerr << "-o     Using a different solution, will not lock." << endl;
                exit(1);
        }
    }
}

//Starts the threads to use the Philosopher method
void Philosopher(vector<mutex*>& fluggelhorns) {
    vector<thread> threads;
    
    for (uint16_t i = 0; i < 5; i++) {
        threads.push_back(thread(std::bind(&PhilosopherPlay, fluggelhorns, i)));
    }
    
    for (uint16_t j = 0; j < 5; j++) {
        threads.at(j).join();
    }
}


void PhilosopherPlay(vector<mutex*>& fluggelhorns, uint16_t& id) {
    while(true) {
        fluggelhorns.at(id)->lock();
        fluggelhorns.at((id+1)%5)->lock();
        out.lock();
        cout << "Thread [" << id << "] plays." << endl;
        out.unlock();
        fluggelhorns.at((id+1)%5)->unlock();
        fluggelhorns.at(id)->unlock();
        sched_yield();
    }
}

//Starts the threads for the Try_Lock method
void TryMethod(vector<mutex*>& fluggelhorns) {
    vector<thread> threads;
    for (uint16_t i = 0; i < 5; i++) {
        threads.push_back(thread(std::bind(&TryPlay, fluggelhorns, i)));
    }
    
    for (uint16_t i = 0; i < 5; i++) {
        threads.at(i).join();
    }
}

void TryPlay(vector<mutex*>& fluggelhorns, uint16_t& id) {
    while(true) {
        if (fluggelhorns.at(id)->try_lock()) {
            if (fluggelhorns.at((id+1)%5)->try_lock()) {
                out.lock();
                cout << "Thread [" << id << "] plays." << endl;
                out.unlock();
                fluggelhorns.at((id+1)%5)->unlock();
                fluggelhorns.at(id)->unlock();
            } else {
                fluggelhorns.at(id)->unlock();
            }
        }
        sched_yield();
    }
}

//Starts the other method
void OtherMethod(vector<mutex*>& fluggelhorns) {
    vector<thread> threads;
    for (uint16_t i = 0; i < 5; i++) {
        threads.push_back(thread(std::bind(&OtherPlay, fluggelhorns, i)));
    }
    
    for (uint16_t i = 0; i < 5; i++) {
        threads.at(i).join();
    }
}

//The other method has one thread go "backwards"
void OtherPlay(vector<mutex*>& fluggelhorns, uint16_t& id) {
    while(true) {
        if (id == 4) {
            fluggelhorns.at(0)->lock();
            fluggelhorns.at(id)->lock();
            out.lock();
            cout << "Thread [" << id << "] plays." << endl;
            out.unlock();
            fluggelhorns.at(id)->unlock();
            fluggelhorns.at(0)->unlock();
        } else{
            fluggelhorns.at(id)->lock();
            fluggelhorns.at((id+1)%5)->lock();
            out.lock();
            cout << "Thread [" << id << "] plays." << endl;
            out.unlock();
            fluggelhorns.at((id+1)%5)->unlock();
            fluggelhorns.at(id)->unlock();
        }
        sched_yield();
    }
    
}
