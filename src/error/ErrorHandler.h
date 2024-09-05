#pragma once

#include <string>
#include <vector>

class ErrorHandler {
private:
    static bool status;

public:
    static void error(const int &lineNumber, const int &columnNumber, const std::string& message);
    static void error(const std::string& message);
    static bool getStatus();
};