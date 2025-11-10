#include <iostream>
#include <fstream>
#include <stdexcept>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx;

void writeFile(int threadId){
    ofstream file("log.txt", ios::app);
    if(!file){
        lock_guard<mutex> lock(mtx);
        throw runtime_error("File not found");
    }

    int times = 5;
    for(int i = 0; i < times; i++){
        file << "Thread " << threadId << " wrote log " << i + 1 << endl;
    }

    file.close();
}

void processFile(){
    ifstream file("log.txt");
    if(!file){
        throw runtime_error("Cannot open file");
    }

    string line;
    while(getline(file, line)){
        if(line == "error") throw runtime_error("Bad data found!");
        cout << line << endl;
    }

    file.close();
}

int main(){
    try {
        thread t1(writeFile, 1);
        thread t2(writeFile, 2);
        thread t3(writeFile, 3);

        t1.join();
        t2.join();
        t3.join();

        cout << "All threads finished.\n";
        processFile();

    } catch(const std::exception& e) {
        cerr << "Caught exception: " << e.what() << '\n';
    }

    return 0;
}
