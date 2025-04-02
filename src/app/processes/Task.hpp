#ifndef TASK_HPP
#define TASK_HPP


#include "../fileHandling/IO.hpp"
#include <fstream>
#include <string>
#include <sstream>

enum class Action {
    ENCRYPT,
    DECRYPT
};

struct Task {
    std::string filePath;
    std::fstream f_stream;
    Action action;
    Task(std::fstream&& stream, Action act, std::string filePath) : f_stream(std::move(stream)), action(act), filePath(filePath) {}
}



































#endif