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

TEST_FIXTURE(MapFixture, intersection_ids_from_street_function_unit_test) {
    // Compare Result from APIDataBase for function test   
    // error: 3 seg: 3, 85, 84, 146, 173, 184, 192 ; 4 seg: 163, 165, ; 
    unsigned from, to;
    cout << "Please insert test range (0 to 108859): ";
    cin >> from >> to;
    for (unsigned intersec_id = from; intersec_id < to; intersec_id++) {
        std::cout << std::endl << std::endl << std::endl << "Start to test for street intersection: " << intersec_id << std::endl;
        
        // aquire connecting street segment of current intersection
        vector<unsigned> street_seg_id_of_intersec;
        unsigned num_seg_of_intersec = getIntersectionStreetSegmentCount(intersec_id);
        
        // continue test case if number of street segment is greater or equal to two
        if (num_seg_of_intersec < 2) {
            break;
        }
        
        // store connecting street segment
        for (unsigned iter = 0; iter < num_seg_of_intersec; iter++) {
            street_seg_id_of_intersec.push_back(getIntersectionStreetSegment(intersec_id, iter));
        }        
        
        // extract street name from street id in street segment with API functions
        std::string test_street_name_1 = getStreetName(getStreetSegmentInfo(street_seg_id_of_intersec[0]).streetID);
        std::string test_street_name_2 = getStreetName(getStreetSegmentInfo(street_seg_id_of_intersec[1]).streetID);
        
        cout << "Testing for: " << test_street_name_1 << " &and& " << test_street_name_2 << endl;
        
        // call test function (find_intersection_ids_from_street_names) from m1.cpp
        std::vector<unsigned> found_inter_id_from_name = find_intersection_ids_from_street_names(test_street_name_1, test_street_name_2);
        
        for (unsigned i = 0; i < found_inter_id_from_name.size(); i++) {
            //cout << found_inter_id_from_name[i] << endl;
        }
        
        
        // check if the current intersection id exist in the return vector of test function
        bool include_flag;
            if (std::find(found_inter_id_from_name.begin(), found_inter_id_from_name.end(), intersec_id) == found_inter_id_from_name.end()) {
                include_flag = false;
                std::cout << "include_flag: false!" << std::endl;
            } else {
                include_flag = true;
                std::cout << "include_flag: true" << std::endl;
            }
            //std::cout << "include_flag: " << include_flag << std::endl;
            CHECK_EQUAL(true, include_flag);
    }


} //intersection_ids_from_street_names function test

/*
TEST_FIXTURE(MapFixture, intersection_ids_from_street_corner_unit_test) {
    std::vector<unsigned> expected;
    std::vector<unsigned> actual;
    // same name 
    expected = {};
    actual = find_intersection_ids_from_street_names("Bloor Street", "Bloor Street");
    std::sort(actual.begin(), actual.end());
    CHECK_EQUAL(expected.size(), actual.size());
    // no name 
    expected = {};
    actual = find_intersection_ids_from_street_names("", "");
    std::sort(actual.begin(), actual.end());
    CHECK_EQUAL(expected.size(), actual.size());

    // special character 
    expected = {};
    actual = find_intersection_ids_from_street_names("*&^%$$#@", "/*-+^^^");
    std::sort(actual.begin(), actual.end());
    CHECK_EQUAL(expected.size(), actual.size());

    // streets that does not intersect
    expected = {};
    actual = find_intersection_ids_from_street_names("Yonge Street", "Bay Street");
    std::sort(actual.begin(), actual.end());
    CHECK_EQUAL(expected.size(), actual.size());

    // duplicate street name 
    expected = {}; // don't know yet
    actual = find_intersection_ids_from_street_names("John Street", "Victoria Street");
    std::sort(actual.begin(), actual.end());
    //CHECK_EQUAL(expected, actual);

    // street that does not exist
    expected = {};
    actual = find_intersection_ids_from_street_names("ECE297", "Perfect Please");
    std::sort(actual.begin(), actual.end());
    CHECK_EQUAL(expected.size(), actual.size());

    // street that goes back to itself 
    expected = {}; // don't know yet
    actual = find_intersection_ids_from_street_names("Spadina Crescent", "Spadina Avenue");
    std::sort(actual.begin(), actual.end());
    //CHECK_EQUAL(expected, actual);

    expected = {}; // don't know yet
    actual = find_intersection_ids_from_street_names("Spadina Crescent", "Spadina Crescent");
    std::sort(actual.begin(), actual.end());
    //CHECK_EQUAL(expected, actual);

    // street that changes name at intersection
    expected = {}; // don't know yet
    actual = find_intersection_ids_from_street_names("Finch Avenue West", "Finch Avenue East");
    std::sort(actual.begin(), actual.end());
    //CHECK_EQUAL(expected, actual);
}

//
//TEST_FIXTURE(MapFixture, intersection_ids_from_street_performance_unit_test) {
//        //Timed Test
//        UNITTEST_TIME_CONSTRAINT(250);//in ms
//        std::vector<unsigned> result;
//        for (size_t k = 0; k < 1000000; k++) {
//            result = find_intersection_ids_from_street_names(street_names_1[k], street_names_2[k]);
//        }
//        UnitTest::TimeHelpers::SleepMs(1000);
//}
