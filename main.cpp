#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "./src/app/processes/ProcessManagement.hpp"
#include "./src/app/processes/Task.hpp"
#include "./src/chunk-encryption/utils/progress_utils.h"
#include <atomic>
#include <chrono>

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

    cout << "yes for multithreading? (y/n): ";
    char threadChoice;
    cin >> threadChoice;
    cin.ignore();
    bool useMultithreading = (threadChoice == 'y' || threadChoice == 'Y');

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

        // Count total files to process
        size_t total_files = 0;
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().filename() != passwordFileName) {
                ++total_files;
            }
        }
        cout << "Total files to process: " << total_files << endl;
        std::atomic<size_t> files_processed(0);
        auto start_time = std::chrono::steady_clock::now();

        // Proceed to encryption/decryption
        ProcessManagement processManagement(useMultithreading);

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
                // Update and print progress
                size_t processed = files_processed.fetch_add(1) + 1;
                print_progress(processed, total_files, start_time,false);
            }
        }
        print_progress(total_files, total_files, start_time,false);
        cout << endl;

        // Uncomment to run tasks
        // processManagement.executeTasks();

    } catch (const fs::filesystem_error& ex) {
        cout << "❌ Filesystem error: " << ex.what() << endl;
    }
    return 0;
}