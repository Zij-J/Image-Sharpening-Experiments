// super basic unit test function (workable only when `!=` operator is defined) & profiling timer
#ifndef CODE_TESTER_H
#define CODE_TESTER_H

#include <iostream>
#include <string.h>     // for stings
#include <functional>   // for `std:function`
#include <sstream>  // for `stringstream`
// #include <source_location> // for getting information of caller of unit test (only available after C++20, so must pass to function...)
using namespace std; // for `cout`...
#define RED  "\x1B[31m" // why not have colors?
#define YEL  "\x1B[33m" 
#define GRE  "\x1B[32m" 
#define RESET "\x1B[0m"


#define DEBUG_COUT(n) cout << #n << ": " << n << endl;

// ps. 
// `typename` and `class` can both be used in template. Originally using `class` to lesser
// the keywords. Later they found out it's confusing for some cases and beginners, so they added `typename`  
// source: https://web.archive.org/web/20060619131004/http://blogs.msdn.com/slippman/archive/2004/08/11/212768.aspx


// if not generate the correct value, termainte the program
template <typename Type>
void unit_test(Type expect_value, Type function_return_value, string file_name, int executed_line) {
    // not pass
    if (expect_value != function_return_value) {
        cout << RED "FAILED unit test: " RESET << file_name << " (line " << executed_line <<  ")" << endl;
        exit(EXIT_FAILURE);
    }
    // pass!
    cout << GRE "SUCCESS unit test: " RESET << file_name << " (line " << executed_line <<  ")" << endl;
} 


// if not getting the correct string (need to do input indirection to `stringstream` from `<sstream>`), termainte the program
void cout_unit_test(const char *expect_output, stringstream &got_output, string file_name, int executed_line);

// will only profil the function once + will retrun time get, have greater flexiblity
clock_t just_profil_timing(function<void (void)> function);

// print out average time of the `function` runing `test_time` times (pre_function is just an empty function by default)
void profil_timer(function<void (void)> profiling_function, int test_time, bool is_show_each_calculation, string file_name, int executed_line, function<void (void)> pre_function = [](void) -> void{});


#endif