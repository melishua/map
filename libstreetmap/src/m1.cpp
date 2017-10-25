#include "m1.h"
#include "global.h"
#include <math.h>

//declare functions
Map mymap;
std::string layer1_map_name;

// load map data and build data structure to speed up API function
bool load_map(std::string map_name) {
    layer1_map_name = map_name;
    return (mymap.load_layer2_map(map_name));
}

//Clean-up map related data structures
void close_map() {   
    closeStreetDatabase();
    //closeOSMDatabase();
}

// Returns street id(s) for the given street name --- BILLY
// If no street with this name exists, returns a 0-length vector
std::vector<unsigned> find_street_ids_from_name(std::string street_name) {
    return mymap.street_name_street_id[street_name];
}


// Returns the street segments for the given intersection --- BILLY
std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {
    return mymap.intersection_street_segments[intersection_id];
}


// Returns the street names at the given intersection --- BILLY
// (includes duplicate street names in returned vector)
std::vector<std::string> find_intersection_street_names(unsigned intersection_id) {
    std::vector<std::string> street_names;
    std::vector<unsigned> streetSegment = mymap.intersection_street_segments[intersection_id];
    
    for (std::vector<unsigned>::iterator iter = streetSegment.begin(); iter != streetSegment.end(); iter++) {
        //street_seg base(street based.(0)).street seg info
        street_names.push_back(mymap.street_segments_list[mymap.street_street_segments[mymap.street_segments_list[*iter].street_index][0]].street_name); 
    }
    
    return street_names;
}


// Returns true if you can get from intersection1 to intersection2 using a single street segment
// (hint:check for 1-way streets too) --- BILLY
// corner case: an intersection is considered to be connected to itself
bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) {
    std::vector<unsigned> steetSegment_id1 = find_intersection_street_segments(intersection_id1);
    std::vector<unsigned> steetSegment_id2 = find_intersection_street_segments(intersection_id2);
    
    for (std::vector<unsigned>::iterator iter1 = steetSegment_id1.begin(); iter1 != steetSegment_id1.end(); iter1++) {
        for (std::vector<unsigned>::iterator iter2 = steetSegment_id2.begin(); iter2 != steetSegment_id2.end(); iter2++) {
            if (*iter1 == *iter2) {
                if (mymap.street_segments_list[*iter1].oneWay != true) {
                    return true;
                } else {
                    if (mymap.street_segments_list[*iter1].from == intersection_id1)
                        return true;
                }
            }
        }
    }
    
    return false;
}


// Returns all intersections reachable by traveling down one street segment from
// given intersection (hint: you can't travel the wrong way on a 1-way street)
// the returned vector should NOT contain duplicate intersections --- MELISSA
std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id) {
    std::vector<IntersectionIndex> adj_intsec_id;

    // aquire number of street segment that is connect with given intersection
    unsigned num_street_seg = mymap.intersection_street_segments[intersection_id].size();

    // aquire connected street segment index of given intersection
    // then aquire endpoint intersection from streetSegmentInfo    
    for (StreetSegmentIndex current_seg = 0; current_seg < num_street_seg; current_seg++) {
        // aquire ith_street_segment 
        street_seg_info this_street_seg_comb = mymap.street_segments_list[mymap.intersection_street_segments[intersection_id][current_seg]];
        // determine if intersection is reachable by looking into oneWay and fromOrTo variable
        // if the street segment is oneway and out of intersection
        // if the street segment is not (oneWay and also into direction) and does not go back to itself then the to intersection is reachable
        if (this_street_seg_comb.to != intersection_id) {
            adj_intsec_id.push_back(this_street_seg_comb.to);
        } else {
            if (!this_street_seg_comb.oneWay) {
                adj_intsec_id.push_back(this_street_seg_comb.from);
            }
        }
    }

    std::set<unsigned> adj_intsec_id_unique(adj_intsec_id.begin(), adj_intsec_id.end());
    adj_intsec_id.assign(adj_intsec_id_unique.begin(), adj_intsec_id_unique.end());

    return adj_intsec_id;
}

// Returns all street segments for the given street --- MELISSA
std::vector<unsigned> find_street_street_segments(unsigned street_id) {
    return mymap.street_street_segments[street_id];
}

// Returns all intersections along the a given street --- MELISSA
std::vector<unsigned> find_all_street_intersections(unsigned street_id) {
    return mymap.street_intersections[street_id];
}

// Return all intersection ids for two intersecting streets --- MELISSA
// This function will typically return one intersection id.
// However street names are not guaranteed to be unique, so more than 1 intersection id may exist
std::vector<unsigned> find_intersection_ids_from_street_names(std::string street_name1, std::string street_name2) {
    // found_street_id_1: all street ids that corresponds to street_name1
    // found_all_intersect: all intersections associated with specific street id

    // loop through all interception to find name
    std::vector<unsigned> street1_inter, street2_inter, street1_id, street2_id, intersec_id_street_name;
    std::vector<unsigned> attemption;
    // find all street id that has name string name 1
    street1_id = find_street_ids_from_name(street_name1);
    street2_id = find_street_ids_from_name(street_name2);

    for (auto iter1 = street1_id.begin(); iter1 != street1_id.end(); iter1++) {
        street1_inter = find_all_street_intersections(*iter1);
        for (auto iter2 = street2_id.begin(); iter2 != street2_id.end(); iter2++) {
            street2_inter = find_all_street_intersections(*iter2);
            for (auto iter3 = street1_inter.begin(); iter3 != street1_inter.end(); iter3++) {
                for (auto iter4 = street2_inter.begin(); iter4 != street2_inter.end(); iter4++) {
                    if (*iter3 == *iter4)
                        intersec_id_street_name.push_back(*iter3);
                }
            }
        }
    }
    
    return intersec_id_street_name;
}

// Returns the distance between two coordinates in meters --- IVY
double find_distance_between_two_points(LatLon point1, LatLon point2) {
    return mymap.find_distance_between_two_points(point1 , point2);
}

// Returns the length of the given street segment in meters --- IVY
double find_street_segment_length(unsigned street_segment_id) {
    return mymap.street_segments_list[street_segment_id].length;
}

// Returns the length of the specified street in meters --- IVY
double find_street_length(unsigned street_id) {
    //sum all the street segments length to get the street length
    double length = 0.0;
    std::vector<unsigned> StreetSegList = find_street_street_segments(street_id);
    for (unsigned current_seg = 0; current_seg < StreetSegList.size(); current_seg++) {
        length += find_street_segment_length(StreetSegList[current_seg]);
    }
    return length;
}

// Returns the travel time to drive a street segment in seconds (time=distance/speed_limit) --- IVY
double find_street_segment_travel_time(unsigned street_segment_id) {
    return mymap.seg_travel_time_list[street_segment_id];
}

// Returns the nearest point of interest to the given position --- IVY
unsigned find_closest_point_of_interest(LatLon my_position) {
    std::vector<value> result;
    mymap.rtree_poi.query(bgi::nearest(point(mymap.longitude_to_cartesian(my_position.lon()),
            mymap.latitude_to_cartesian(my_position.lat())), 1), std::back_inserter(result));
    
    return result[0].second;
}

// Returns the the nearest intersection to the given position --- IVY
unsigned find_closest_intersection(LatLon my_position) {
    std::vector<value> result;
    mymap.rtree_intersection.query(bgi::nearest(point(mymap.longitude_to_cartesian(my_position.lon()),
            mymap.latitude_to_cartesian(my_position.lat())), 10), std::back_inserter(result));
    
    //test the 10 points again with find_distance_between_two_points() to find the actual closest
    double shortest_distance = find_distance_between_two_points(my_position, getIntersectionPosition(result[0].second));
    double temp_distance;
    unsigned closest_int = result[0].second;

    for (unsigned i = 1; i < result.size(); i++) {
        temp_distance = find_distance_between_two_points(my_position, getIntersectionPosition(result[i].second));
        if (temp_distance < shortest_distance) {
            shortest_distance = temp_distance;
            closest_int = result[i].second;
        }
    }
    
    return closest_int; 
}
