#include "Cryption.hpp"
#include "../processes/Task.hpp"
#include "../fileHandling/ReadEnv.cpp"
#include <ctime>
#include <iomanip>

using namespace std;
int executeCryption(const string& taskData) {
    Task task = Task::fromString(taskData); //static method class dependent
    ReadEnv env; //object to read
    string envKey = env.getenv(); //"12345" in .env
    int key = stoi(envKey);// now //12345
    if (task.action == Action::ENCRYPT) {
        char ch;
        while (task.f_stream.get(ch)) { //char by char

            //hashing: divide by 256--> rem between 0 to 255
            //A= 68 key=12345, so (68+12345)%256
            ch = (ch + key) % 256;

            //seek pointer reads 0 index and advances to 1st index, so used -1 to get it at 0
            //hello world
            //seek advanced to e, so -1, now at h
            //so, cur position is of h's hashing
            //eg: hello--> 8ello
            task.f_stream.seekp(-1, ios::cur);
            task.f_stream.put(ch);
        }
        task.f_stream.close();
    }
    else 
    {
        char ch;
        while (task.f_stream.get(ch)) {
            ch = (ch - key + 256) % 256;
            task.f_stream.seekp(-1, ios::cur);
            task.f_stream.put(ch);
        }
        task.f_stream.close();
    }

    time_t t = std::time(nullptr);
    tm* now = std::localtime(&t);
    cout << "Exiting the encryption/decryption at: " << put_time(now, "%Y-%m-%d %H:%M:%S") << endl;
   
    return 0;
}