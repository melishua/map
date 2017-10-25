#include "m4_helper.h"

unsigned find_path_between_intersections_return_single_destination(const unsigned intersect_id_start, vector<unsigned> valid_end_intersections, const double turn_penalty) {
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
                return (currNODE.intersection_id); 
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
    return intersect_id_start;
}

