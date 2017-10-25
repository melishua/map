#include "m4.h"
#include "m3.h"
#include "global.h"
#include <limits>
#include <chrono>
#define TIME_LIMIT 30 

std::vector<unsigned> legal;
std::vector<bool> deliveries_has_picked_up;
std::vector<bool> deliveries_has_dropped_off;

void doing_pickups_dropoffs(const std::vector<DeliveryInfo>& deliveries, const unsigned intersection_id, unsigned& count);
std::vector<unsigned> find_path_from_intersection_ids(const std::vector<unsigned>& intersection_ids, const float turn_penalty);
std::vector<unsigned> traveling_courier_greedy(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const float turn_penalty) ;
//std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, 
//                                        const std::vector<unsigned>& depots, 
//                                        const float turn_penalty){
//    
//    double shortest_time = std::numeric_limits<double>::max();
//    std::vector<unsigned> shortest_path;
//    
//    std::vector<unsigned> pick_ups;
//    for (unsigned i = 0; i < deliveries.size(); i++) {       
//        pick_ups.push_back(deliveries[i].pickUp);       
//    }
//
//    for (unsigned current_depot = 0; current_depot < depots.size(); current_depot++) {
//            
//            unsigned count = deliveries.size();
//            deliveries_has_picked_up.clear();
//            deliveries_has_picked_up.resize(count, false);
//            deliveries_has_dropped_off.clear();
//            deliveries_has_dropped_off.resize(count, false);
//            legal_intersections = pick_ups;
//            //unsigned current_intersection_id;
//
//        //Go from any depot to nearest pickup
//        std::vector<unsigned> temp_path = find_path_between_intersections_help(depots[current_depot], legal_intersections, turn_penalty);
//        if (temp_path.size() == 0) continue;
//        doing_pickups_dropoffs(deliveries, destination_intersection_id, count);
//
//        while (count > 0) {
//            //go to nearest pickUp or legal dropOff
//            //current_intersection_id = destination_intersection_id;
//            std::vector<unsigned> new_path = find_path_between_intersections_help(destination_intersection_id, legal_intersections, turn_penalty);
//            
//            if (new_path.size() == 0) break; //no legal path
//            
//            temp_path.insert(temp_path.end(), new_path.begin(), new_path.end());
//            doing_pickups_dropoffs(deliveries, destination_intersection_id, count);
//        }
//
//        if (count != 0) continue; // corrsp to break in while loop
//
//        std::vector<unsigned> new_path = find_path_between_intersections_help(destination_intersection_id, depots, turn_penalty);
//        if (new_path.size() == 0) continue;
//
//        temp_path.insert(temp_path.end(), new_path.begin(), new_path.end()); //Go to nearest depot
//
//        double temp_time = compute_path_travel_time(temp_path, turn_penalty);
//        if (temp_time < shortest_time) {
//            shortest_time = temp_time;
//            shortest_path = temp_path;
//        }
//    }
//    return shortest_path;
//}

std::vector<unsigned> traveling_courier_greedy(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const float turn_penalty) {

    auto startTime = std::chrono::high_resolution_clock::now();
    //bool timeOut = false;
    
    double shortest_time = std::numeric_limits<double>::max();
    std::vector<unsigned> shortest_path;
    std::vector<unsigned> shortest_path_intersections;

    std::vector<unsigned> pick_ups;
    for (unsigned i = 0; i < deliveries.size(); i++) {
        pick_ups.push_back(deliveries[i].pickUp);
    }

    for (unsigned current_pickup = 0; current_pickup < pick_ups.size(); current_pickup++) {
                       
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto wallClock = std::chrono::duration_cast<std::chrono::duration<double>> (currentTime - startTime);
        if (wallClock.count() > 0.95 * TIME_LIMIT) break;//timeOut = true;
         
        unsigned count = deliveries.size();
        deliveries_has_picked_up.clear();
        deliveries_has_picked_up.resize(count, false);
        deliveries_has_dropped_off.clear();
        deliveries_has_dropped_off.resize(count, false);
        legal = pick_ups;
        
        double current_time = 0;
        std::vector<unsigned> temp_path;
        std::vector<unsigned> temp_path_intersections;
        
        doing_pickups_dropoffs(deliveries, pick_ups[current_pickup], count);              
        destination_intersection_id = pick_ups[current_pickup];
        temp_path_intersections.push_back(destination_intersection_id);
        
        while (count > 0) {
            //go to nearest pickUp or legal dropOff
            std::vector<unsigned> new_path = find_path_between_intersections_help(destination_intersection_id, legal, turn_penalty);
            if (new_path.size() == 0) break; //no legal path

            current_time += compute_path_travel_time(new_path, turn_penalty);
            if (current_time >= shortest_time) break;
            
            temp_path.insert(temp_path.end(), new_path.begin(), new_path.end());
            temp_path_intersections.push_back(destination_intersection_id);
            doing_pickups_dropoffs(deliveries, destination_intersection_id, count);
        }

        if (count != 0) continue; // corrsp to break in while loop

        std::vector<unsigned> to_depot = find_path_between_intersections_help(destination_intersection_id, depots, turn_penalty);
        if (to_depot.size() == 0) continue;

        current_time += compute_path_travel_time(to_depot, turn_penalty);
        if (current_time >= shortest_time) continue;

        std::vector<unsigned> from_depot;
        double from_depot_shortest_time = std::numeric_limits<double>::max();

        //find nearest depot to the pickup
        for (unsigned current_depot = 0; current_depot < depots.size(); current_depot++) {

            std::vector<unsigned> temp_from_depot = find_path_between_intersections(depots[current_depot], pick_ups[current_pickup], turn_penalty);
            if (temp_from_depot.size() == 0) continue;
            double temp_from_depot_time = compute_path_travel_time(temp_from_depot, turn_penalty);

            if (temp_from_depot_time < from_depot_shortest_time) {
                from_depot_shortest_time = temp_from_depot_time;
                from_depot = temp_from_depot;
            }
        }
        if (from_depot.size() == 0) continue; 
        
        current_time += from_depot_shortest_time;
        if (current_time >= shortest_time) continue;       
            
        temp_path.insert(temp_path.begin(), from_depot.begin(), from_depot.end());
        temp_path.insert(temp_path.end(), to_depot.begin(), to_depot.end());

        shortest_time = current_time;
        shortest_path = temp_path;
        shortest_path_intersections = temp_path_intersections;
    }//end for
    
    return shortest_path_intersections;
}

void doing_pickups_dropoffs(const std::vector<DeliveryInfo>& deliveries, const unsigned intersection_id, unsigned& count) {
    for (unsigned i = 0; i < deliveries.size(); i++) {
        if (deliveries[i].pickUp == intersection_id) {
            deliveries_has_picked_up[i] = true;
            legal.push_back(deliveries[i].dropOff);
        }
        if ((deliveries[i].dropOff == intersection_id) && (deliveries_has_picked_up[i]) && (!deliveries_has_dropped_off[i])) {
            deliveries_has_dropped_off[i] = true;
            count--;
        }
    }

    unsigned j = 0;
    while (j < legal.size()) {
        if (legal[j] == intersection_id) {
            legal.erase(legal.begin() + j);
        } else j++;
    }
}

//void annealing() {
//    S = InitialSolution;
//    C = Cost(S); // E.g. travel time
//    T = high temperature; // big number
//    
//    while (solution changing) {
//        Snew = perturb(S);
//        double Cnew = Cost(S);
//        double change = Cnew - C
//        if (Cnew < C) {
//            S = Snew; // Update solution
//            C = Cnew;
//        }
//        T = reduceTemp(T);
//    }
//}

std::vector<unsigned> find_path_from_intersection_ids(const std::vector<unsigned>& intersection_ids, const float turn_penalty) {
    std::vector<unsigned> path = find_path_between_intersections(intersection_ids[0], intersection_ids[1], turn_penalty);
    
    for (unsigned i=1 ; i < intersection_ids.size()-1; i++){
        std::vector<unsigned> temp_path = find_path_between_intersections(intersection_ids[i], intersection_ids[i+1], turn_penalty);
        path.insert(path.end(), temp_path.begin(), temp_path.end());        
    }
    return path;
}

