#include "m3.h"
#include "m1.h"
#include "Map.h"
#include "global.h"
#include "NODE.h"
#include "MinHeap.h"

/*******************************************************************************
 *                          Global Variable
 *******************************************************************************/
unsigned destination_intersection_id;

/*******************************************************************************
 *                          Function Declaration
 *******************************************************************************/
//std::vector<unsigned> find_path_between_intersections_help(const unsigned intersect_id_start, std::vector<unsigned> valid_end_intersections, const double turn_penalty);

/*******************************************************************************
 *                          Function Implementation
 *******************************************************************************/

double compute_path_travel_time(const std::vector<unsigned>& path, const double turn_penalty) {
    double path_travel_time = 0.0;
    if (path.size() == 0) {
        return path_travel_time;
    } else {
        for (unsigned i = 0; i < path.size(); i++) {
            path_travel_time += mymap.seg_travel_time_list[path[i]];
            if (i != path.size() - 1) {
                if (IsTurned(path[i], path[i + 1]))
                    path_travel_time += turn_penalty;
            }
        }
        return path_travel_time;
    }
}

std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, const unsigned intersect_id_end, const double turn_penalty) {
    vector<unsigned> valid_end_intersections (1,intersect_id_end);
    return find_path_between_intersections_help(intersect_id_start,valid_end_intersections,turn_penalty);
}  
std::vector<unsigned> find_path_between_intersections_help(const unsigned intersect_id_start, std::vector<unsigned> valid_end_intersections, const double turn_penalty) {
    std::vector<NODE> store_created_node;
    store_created_node.resize(mymap.total_number_intersection);
    MinHeap wavefront;
    NODE source(0.0, intersect_id_start, -1); //want to use long int
    wavefront.Insert(source);
    store_created_node.at(intersect_id_start) = source;

    while (wavefront.size() != 0) {
        NODE currNODE = wavefront.GetMin();
        wavefront.DeleteMin();
        if (store_created_node[currNODE.intersection_id].visited_flag == false) {
            currNODE.visited_flag = true;
            store_created_node.at(currNODE.intersection_id) = currNODE;

            if (std::find(valid_end_intersections.begin(),valid_end_intersections.end(),currNODE.intersection_id) != valid_end_intersections.end()) {
                destination_intersection_id = currNODE.intersection_id;
                return (TrackBack(store_created_node, currNODE));
                
            }

            vector<unsigned> outEdge = find_intersection_street_segments(currNODE.intersection_id);
            for (size_t i = 0; i < outEdge.size(); i++) {
                if (mymap.street_segments_list[outEdge[i]].from == currNODE.intersection_id) {
                    if (store_created_node[mymap.street_segments_list[outEdge[i]].to].visited_flag != true) {
                        double temp_weight = currNODE.weight + mymap.seg_travel_time_list[outEdge[i]];
                        if (currNODE.reaching_edge != -1 && mymap.street_segments_list[currNODE.reaching_edge].street_index != mymap.street_segments_list[outEdge[i]].street_index) {
                            temp_weight = temp_weight + turn_penalty;
                        }

                        NODE toNODE(temp_weight, mymap.street_segments_list[outEdge[i]].to, outEdge[i]);
                        wavefront.Insert(toNODE);
                        store_created_node.at(mymap.street_segments_list[outEdge[i]].to) = toNODE;
                    }
                } else if (mymap.street_segments_list[outEdge[i]].oneWay == false) {
                    if (store_created_node[mymap.street_segments_list[outEdge[i]].from].visited_flag != true) {
                        double temp_weight = currNODE.weight + mymap.seg_travel_time_list[outEdge[i]];
                        if ((currNODE.reaching_edge != -1) && (mymap.street_segments_list[currNODE.reaching_edge].street_index != mymap.street_segments_list[outEdge[i]].street_index)) {
                            temp_weight = temp_weight + turn_penalty;
                        }
                        NODE toNODE(temp_weight, mymap.street_segments_list[outEdge[i]].from, outEdge[i]);
                        wavefront.Insert(toNODE);
                        store_created_node.at(mymap.street_segments_list[outEdge[i]].from) = toNODE;
                    }
                }
            }
        }
    }
    std::vector<unsigned> nothing;
    return nothing;
}


// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// The path will begin at the specified intersection, and end on the 
// intersection that is closest (in Euclidean distance) to the point of 
// interest.
// If no such path exists, returns an empty (size == 0) vector.
std::vector<unsigned> find_path_to_point_of_interest(const unsigned intersect_id_start, const std::string point_of_interest_name, const double turn_penalty) {
    
    std::vector<unsigned> specific_poi_list = mymap.poi_name_poi_ids[point_of_interest_name];
    // return empty vector if poi not found
    if (specific_poi_list.size() == 0) return specific_poi_list;
    
    // aquire closest intersection then find path
    std::vector<unsigned> end_intersections_list;
      for (unsigned i = 0; i < specific_poi_list.size(); i++) {
        unsigned intersect_id_end = find_closest_intersection(mymap.poi_list[specific_poi_list[i]].position);
        end_intersections_list.push_back(intersect_id_end);
      }
    return find_path_between_intersections_help(intersect_id_start,end_intersections_list,turn_penalty);
}