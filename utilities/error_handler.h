#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
using namespace std;


#define ErrorHandler_FATAL_IF(condition, file_name, executed_line)  if (condition) ErrorHandler::fatal_error(#condition, file_name, executed_line);


class ErrorHandler {
public:
    static void fatal_error(string message, string file_name = "", int executed_line = -1);
    static void warning(string message, string file_name = "", int executed_line = -1);
    static void normal_notice(string message, string file_name, int executed_line);
    static void success_notice(string message, string file_name= "", int executed_line = -1);

private:
    static string file_and_line(const string &file_name, int executed_line);
};


#endif