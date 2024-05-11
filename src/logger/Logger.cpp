#include "Logger.h"

#include <iostream>

void Logger::error(const std::string &message) {
    std::string outputString = "Error: " + message;
    std::cout << outputString << std::endl;
}
