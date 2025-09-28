// super basic unit test function (workable only when `!=` operator is defined) & profiling timer
#include "code_tester.h"
#include <time.h>       // for `time`
#include <iomanip>      // for `setw` `setfill`

// ps. 
// Seems like only `clock` or `clock_gettime(CLOCK_PROCESS_CPUTIME_ID)` can get CPU time, after testing, only `clock` works(different from normal timing)
// clock can get the same precision(10^-6 seconds) with `getrusage` (Linux specified) and portable, correct measure CPU time(clock_gettime is wrong...)! 
// Clock: https://man7.org/linux/man-pages/man3/clock.3.html


// will only profil the function once + will retrun time get, have greater flexiblity
clock_t just_profil_timing(function<void (void)> function) {
    // prepare timer
    clock_t total_time;
    
    // real test is here!
    total_time = clock();
    function();
    total_time = clock() - total_time;

    // retrun result
    return total_time;
}


// print out average time of the `function` runing `test_time` times (pre_function is just an empty function by default, is given in code_tester.h)
void profil_timer(function<void (void)> profiling_function, int test_time, bool is_show_each_calculation, string file_name, int executed_line, function<void (void)> pre_function) {
    // prepare
    cout << YEL "Now testing Calculation time at: " RESET << file_name << " (line " << executed_line <<  ")" << endl;
    clock_t average = 0; // ps. clock_t is essential long
    // calculate precision_digit 
    int precision_digit = 0, clocks_per_second = CLOCKS_PER_SEC;
    while ((clocks_per_second /= 10) != 0)
        ++precision_digit;

    // tests CPU time (time as independent to other program as possible)!
    for (int i = 0; i < test_time; ++i) {
        // do prerequest
        pre_function();

        // real test is here!
        clock_t total_time = just_profil_timing(profiling_function);

        // print and calculate result (result is precision is dependent to `CLOCKS_PER_SEC`, in Linux will be 10^6, but in Windows may only be 10^3)
        if (is_show_each_calculation) {
            cout << setfill('0') << setw(2) << i+1 << "-th Calculation time: " << total_time / CLOCKS_PER_SEC  << "." << setfill('0') << setw(precision_digit) << total_time % CLOCKS_PER_SEC << " seconds" << endl;
        }
        average += total_time;
    }

    // calculate average and print them
    cout << "Total consumed time: " << average / CLOCKS_PER_SEC  << "." << setfill('0') << setw(precision_digit) << average % CLOCKS_PER_SEC <<  " seconds" << endl;
    average /= test_time;
    cout << YEL "Average Calculation time: " RESET << average / CLOCKS_PER_SEC  << "." << setfill('0') << setw(precision_digit) << average % CLOCKS_PER_SEC <<  " seconds" << endl;
}


// if not getting the correct string (need to do input indirection to `stringstream` from `<sstream>`), termainte the program
void cout_unit_test(const char *expect_output, stringstream &got_output, string file_name, int executed_line) {
    // first ensure everthing is in buffer
    got_output.flush(); 
    // not pass (return 0 if equal)
    if (got_output.str().compare(expect_output)) {
        cout << RED "FAILED unit test: " RESET << file_name << " (line " << executed_line <<  ")" << endl;
        exit(EXIT_FAILURE);
    }
    // pass!
    cout << GRE "SUCCESS unit test: " RESET << file_name << " (line " << executed_line <<  ")" << endl;
    got_output.str(string()); // clear the `got_output` buffer (set as empty `string`, `""` need convert to `string` to pass(slow)) for other input
} 