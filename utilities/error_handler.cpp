#include "error_handler.h"
#include <iostream> // for `cerr`

#define RED  "\x1B[31m" // color of error message
#define YEL  "\x1B[33m" 
#define GRE  "\x1B[32m" 
#define RESET "\x1B[0m"


void ErrorHandler::fatal_error(string message, string file_name, int executed_line) {
    cerr << RED "FATAL ERROR" RESET << file_and_line(file_name, executed_line) << ": " << message << endl;
    exit(EXIT_FAILURE);
}


void ErrorHandler::warning(string message, string file_name, int executed_line) {
    cerr << YEL "WARNING" RESET << file_and_line(file_name, executed_line) << ": " << message << endl;
}


void ErrorHandler::normal_notice(string message, string file_name, int executed_line) {
    cout << "In " << file_name << " (line " << executed_line << ": " << message << endl;
}


void ErrorHandler::success_notice(string message, string file_name, int executed_line) {
    cout << GRE "OPERATION SUCCESS" RESET << file_and_line(file_name, executed_line) << ": " << message << endl;
}


string ErrorHandler::file_and_line(const string &file_name, int executed_line) {
    string final_str;
    if (file_name.length() > 0) {
        final_str += " in " + file_name;
    }
    if (executed_line >= 0) {
        final_str += " (line " + to_string(executed_line) + ')';
    }
    return final_str;
}