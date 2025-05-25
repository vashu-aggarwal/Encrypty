#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "./src/app/processes/ProcessManagement.hpp"
#include "./src/app/processes/Task.hpp"

namespace fs = std::filesystem;
using namespace std;

int main(int argc, char* argv[]) {
    string directory, action, password;
    const string passwordFileName = ".password.txt";

    cout << "Enter the directory path: ";
    getline(cin, directory);

    cout << "Enter the action (encrypt/decrypt): ";
    getline(cin, action);

    cout << "Enter password: ";
    getline(cin, password);

    try {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            cout << "❌ Invalid directory path!" << endl;
            return 1;
        }

        string passwordFilePath = directory + "/" + passwordFileName;

        if (action == "encrypt" || action == "ENCRYPT") {
            // Save password in file
            ofstream outFile(passwordFilePath);
            if (!outFile) {
                cout << "❌ Failed to create password file!" << endl;
                return 1;
            }
            outFile << password;
            outFile.close();
        } else if (action == "decrypt" || action == "DECRYPT") {
            // Read and compare password
            ifstream inFile(passwordFilePath);
            if (!inFile) {
                cout << "❌ Password file not found. Cannot decrypt!" << endl;
                return 1;
            }
            string storedPassword;
            getline(inFile, storedPassword);
            inFile.close();

            if (storedPassword != password) {
                cout << "❌ Incorrect password. Access denied!" << endl;
                return 1;
            }
        } else {
            cout << "❌ Unknown action. Please use 'encrypt' or 'decrypt'." << endl;
            return 1;
        }

        // Proceed to encryption/decryption
        ProcessManagement processManagement;

        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                if (entry.path().filename() == passwordFileName) continue;

                string filePath = entry.path().string();
                IO io(filePath);
                fstream f_stream = std::move(io.getFileStream());

                if (f_stream.is_open()) {
                    Action taskAction = (action == "encrypt" || action == "ENCRYPT")
                                            ? Action::ENCRYPT
                                            : Action::DECRYPT;

                    auto task = make_unique<Task>(move(f_stream), taskAction, filePath);
                    processManagement.submitToQueue(move(task));
                } else {
                    cout << "❌ Unable to open file: " << filePath << endl;
                }
            }
        }

        // Uncomment to run tasks
        // processManagement.executeTasks();

    } catch (const fs::filesystem_error& ex) {
        cout << "❌ Filesystem error: " << ex.what() << endl;
    }
    return 0;
}
