#include "ErrorHandler.h"

#include <iostream>

bool ErrorHandler::status = false;

void ErrorHandler::error(const int &lineNumber, const int &columnNumber, const std::string &message) {
    status = true;
    std::cout << "[ERROR] " << "line "<< lineNumber  << " column " << columnNumber << ", " << message << std::endl;
    std::exit(1);
}

void ErrorHandler::error(const std::string &message) {
    std::cout << "[ERROR] " << message << std::endl;
    std::exit(1);
}

bool ErrorHandler::getStatus() {
    return status;
}
