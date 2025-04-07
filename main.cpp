#include <iostream>
#include <filesystem>
#include "./src/app/processes/ProcessManagement.hpp"
#include "./src/app/processes/Task.hpp"

// given a rename to std::filesystem->fs
namespace fs = std::filesystem;
using namespace std;
int main(int argc, char* argv[]) {
    string directory;
    string action;
    //Taking input from user 
    cout << "Enter the directory path: ";
    getline(cin, directory);

    cout << "Enter the action (encrypt/decrypt): ";
    getline(cin, action);


    try {
        // is_directory->tell wheather its a file or a directory 
        if (fs::exists(directory) && fs::is_directory(directory)) {
            //creating a class(by default constructor is empty)
            ProcessManagement processManagement;

            for (const auto& entry : fs::recursive_directory_iterator(directory)) {
                // checking if the file is a regular file or not
                if (entry.is_regular_file()) {
                    // fetch the path of the file   
                    string filePath = entry.path().string();
                    IO io(filePath);
                    fstream f_stream = std::move(io.getFileStream());

                    if (f_stream.is_open()) {
                        Action taskAction = (action == "ENCRYPT" || action=="encrypt") ? Action::ENCRYPT : Action::DECRYPT;
                        // Create a unique pointer for the task so that it can be passed ahead in the Queue.
                        auto task = make_unique<Task>(move(f_stream), taskAction, filePath); //constructor of task

                        processManagement.submitToQueue(move(task));
                    } else {
                        cout << "Unable to open file: " << filePath << endl;
                    }
                }
            }
            // All files are read now execute the task
            // processManagement.executeTasks();
        } else {
            cout << "Invalid directory path!" << endl;
        }
    } 
    //error handling
    catch (const fs::filesystem_error& ex) {
        cout << "Filesystem error: " << ex.what() << endl;
    }

    return 0;
}