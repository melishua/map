#include <iostream>
#include <unittest++/UnitTest++.h>

/*
 * This is the main that drives running
 * unit tests.
 */

using namespace std;

int main(int argc, char** argv) {
    // Output message
    cout << "This is unit tests for M1.cpp" << endl;
    
    // Run all defined unit tests
    int num_failures = UnitTest::RunAllTests();

    return num_failures;
}