#include <unittest++/UnitTest++.h>
#include <iostream>
#include <algorithm>
#include <set>
#include <random>
#include <vector>
#include "m1.h"
#include "StreetsDatabaseAPI.h"

using namespace std;

struct MapFixture {

    MapFixture() {
        //Load the map
        load_map("/cad2/ece297s/public/maps/toronto_canada.streets.bin");

        //Initialize random number generators
        rng = std::minstd_rand(3);
        rand_intersection = std::uniform_int_distribution<unsigned>(0, getNumberOfIntersections() - 1);
        rand_street = std::uniform_int_distribution<unsigned>(1, getNumberOfStreets() - 1);
        rand_segment = std::uniform_int_distribution<unsigned>(0, getNumberOfStreetSegments() - 1);
        rand_poi = std::uniform_int_distribution<unsigned>(0, getNumberOfPointsOfInterest() - 1);
        rand_lat = std::uniform_real_distribution<double>(43.48, 43.91998);
        rand_lon = std::uniform_real_distribution<double>(-79.78998, -79.00001);
    }

    ~MapFixture() {
        //Clean-up
        close_map();
    }

    
    std::minstd_rand rng;
    std::uniform_int_distribution<unsigned> rand_intersection;
    std::uniform_int_distribution<unsigned> rand_street;
    std::uniform_int_distribution<unsigned> rand_segment;
    std::uniform_int_distribution<unsigned> rand_poi;
    std::uniform_real_distribution<double> rand_lat;
    std::uniform_real_distribution<double> rand_lon;
};

TEST_FIXTURE(MapFixture, find_street_id_from_names_function_test) {
    // prompt to aquire street name
    std::string street_name;
    std::cout << "Please insert street name: ";
    std::cin >> street_name;

    // result from API
    std::vector<unsigned> API_result;
    for (unsigned i = 0; i < getNumberOfStreets(); i++) {
        if (street_name == getStreetName(i)) {
            API_result.push_back(i);
        }
    }

    // result from m1 function
    std::vector <unsigned> m1_result = find_street_ids_from_name(street_name);

    // sort result and compare
    std::sort(m1_result.begin(), m1_result.end());
    std::sort(API_result.begin(), API_result.end());

    // check for same number of result
    CHECK_EQUAL(m1_result.size(), API_result.size());

    
    // check each output
    for (unsigned i = 0; i < API_result.size(); i++) {
        CHECK_EQUAL(m1_result[i], API_result[i]);
    }

} //intersection_ids_from_street_names function test

TEST_FIXTURE(MapFixture, find_street_id_from_names_corner_test) {
    std::vector<unsigned> expected;
    std::vector<unsigned> actual;

    // street name that does not exist
    expected = {};
    actual = find_street_ids_from_name("ECE297 please let us pass");
    std::sort(actual.begin(), actual.end());
    for (unsigned i = 0; i < expected.size(); i++) {
        CHECK_EQUAL(expected[i], actual[i]);
    }

    // duplicate name
    expected = {0};
    actual = find_street_ids_from_name("<unknown>");
    std::sort(actual.begin(), actual.end());
    for (unsigned i = 0; i < expected.size(); i++) {
        CHECK_EQUAL(expected[i], actual[i]);
    }

    // special character 
    expected = {};
    actual = find_street_ids_from_name("*&^%$$#@ /*-+^^^");
    std::sort(actual.begin(), actual.end());
    for (unsigned i = 0; i < expected.size(); i++) {
        CHECK_EQUAL(expected[i], actual[i]);
    }

    // name with special character
    expected = {52};
    actual = find_street_ids_from_name("Ramp to Highway 404 & Don Valley Parkway");
    std::sort(actual.begin(), actual.end());
    for (unsigned i = 0; i < expected.size(); i++) {
        CHECK_EQUAL(expected[i], actual[i]);
    }
}

TEST_FIXTURE(MapFixture, find_street_id_from_names_time_test) {
    // Store all street name
    unsigned total_num_street = getNumberOfStreets();
    std::vector <string> all_street_name;
    for (unsigned i = 0; i < total_num_street; i++) {
        all_street_name.push_back(getStreetName(i));
    }
    
    // Timed Test   
    std::vector<unsigned> result;
    for (unsigned i = 0; i < 10000; i++) {
        std::string street_name = all_street_name[i];
        UNITTEST_TIME_CONSTRAINT(0.00025);
        result = find_street_ids_from_name(street_name);
    }
}
