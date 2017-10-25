/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include "TSP.h"
#include "m4.h"
#include "m3.h"

std::vector<unsigned> compute_path_for_list_of_inter (const std::vector<unsigned>& inter_list, const float turn_penalty);
std::vector<unsigned> fastest_path (const float turn_penalty);
std::vector<unsigned> find_closest_depot_route(const std::vector<unsigned>& depots, unsigned inter_id, bool start,const float turn_penalty);

std::vector<std::vector<unsigned>> geo_closest_path;


std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    // use two opt to find geo-graphic closested distance path
    TSP tsp(deliveries);        // initialization       
    tsp.two_opt();
    geo_closest_path = tsp.get_geo_list();
    // check for path to find time fasted path
    std::vector<unsigned> best_path = fastest_path(turn_penalty);
    // find closest depots
    std::vector<unsigned> start_depot = find_closest_depot_route(depots, best_path[0], true, turn_penalty);
    std::vector<unsigned> end_depot = find_closest_depot_route(depots, best_path[best_path.size() - 1], false, turn_penalty);
    // combine results
    best_path.insert(best_path.begin(), start_depot.begin(), start_depot.end());
    best_path.insert(best_path.end(), end_depot.begin(), end_depot.end());
    return best_path;
}


std::vector<unsigned> find_closest_depot_route(const std::vector<unsigned>& depots, unsigned inter_id, bool start,const float turn_penalty) {
    bool route_valid_found = false;
    // geo best time and route
    std::vector<unsigned> best_route;   
    double best_time;
    // top
    unsigned i = 0;
    while(i < depots.size()) {
        std::vector<unsigned> comb_route;
        if (start){
            comb_route = find_path_between_intersections(depots[i], inter_id, turn_penalty);
        }
        else {
            comb_route = find_path_between_intersections(inter_id, depots[i], turn_penalty);
        }
        
        // next if route not available
        if (comb_route.size() != 0) {
            // initialize best if found done so
            if (!route_valid_found) {
                route_valid_found = true;
                best_route = comb_route;
                best_time = compute_path_travel_time(best_route, turn_penalty);
            } else {
                // compare with best route
                double comb_time = compute_path_travel_time(comb_route, turn_penalty);
                // update information if current route has a shorter time
                if (comb_time < best_time){
                    best_route = comb_route;
                    best_time = comb_time;                   
                }
            }
        }
        i++;
    }
    return best_route;
}



std::vector<unsigned> fastest_path (const float turn_penalty) {
    bool route_valid_found = false;
    // geo best time and route
    std::vector<unsigned> best_route;   
    double best_time;
    
    // top 30 possible result
    unsigned i = 0;
    unsigned improve = 0;
    while(i < geo_closest_path.size() && improve < 30) {        
        std::vector<unsigned> comb_route = compute_path_for_list_of_inter(geo_closest_path[i], turn_penalty);
        // next if route not available
        if (comb_route.size() != 0) {
            // initialize best if found done so
            if (!route_valid_found) {
                route_valid_found = true;
                best_route = comb_route;
                best_time = compute_path_travel_time(best_route, turn_penalty);
            } else {
                // compare with best route
                double comb_time = compute_path_travel_time(comb_route, turn_penalty);
                // update information if current route has a shorter time
                if (comb_time < best_time){
                    best_route = comb_route;
                    best_time = comb_time;                   
                }
            }
        }
        i++;
        improve++;
    }
    return best_route;
}

// run through all route path
std::vector<unsigned> compute_path_for_list_of_inter (const std::vector<unsigned>& inter_list, const float turn_penalty) {
    std::vector<unsigned> comb_route {};
    // loop through path list to find route
    for (unsigned i = 0; i < inter_list.size() - 1; i++) {
        std::vector<unsigned> route_two_inter = find_path_between_intersections(inter_list[i], inter_list[i+1], turn_penalty);
        // path not available
        if (route_two_inter.size() == 0) {
            // return if any intersection pair is not available
            return route_two_inter;
        }
        comb_route.insert(comb_route.end(), route_two_inter.begin(), route_two_inter.end());
    }
    return comb_route;
}