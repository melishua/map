#include "m4_modified_greedy.h"

drop_off_point::drop_off_point(){
    //dropOff = 0;
    has_picked_up = false;
}

std::vector<unsigned> greedy_search(unsigned start, std::vector<unsigned> pick_up_list, std::vector<drop_off_point> drop_off_list, float turn_penalty) {
    std::vector<unsigned> whole_path;
    unsigned start_intersection_id = pick_up_list[start];
    erase_pick_up_point(pick_up_list, drop_off_list, pick_up_list[start]);
    std::vector<unsigned> input_path = construct_path(pick_up_list, drop_off_list);
    std::vector<unsigned> path_segment = find_path_between_intersections_help(start_intersection_id, input_path, turn_penalty);
    if (path_segment.size() == 0) {
        std::vector<unsigned> nothing;
        return nothing;
    }
    whole_path.insert(whole_path.end(), path_segment.begin(), path_segment.end());
    while (drop_off_list.size() != 0) {
        if (erase_pick_up_point(pick_up_list, drop_off_list, destination_intersection_id)) {
            erase_drop_off_point(drop_off_list, destination_intersection_id);
            input_path = construct_path(pick_up_list, drop_off_list);
            path_segment = find_path_between_intersections_help(destination_intersection_id, input_path, turn_penalty);
            if (path_segment.size() == 0) {
                std::vector<unsigned> nothing;
                return nothing;
            }
            whole_path.insert(whole_path.end(), path_segment.begin(), path_segment.end());
        }//do next stuff
        else {
            erase_drop_off_point(drop_off_list, destination_intersection_id);
            input_path = construct_path(pick_up_list, drop_off_list);
            if (input_path.size() == 0) break;
            path_segment = find_path_between_intersections_help(destination_intersection_id, input_path, turn_penalty);
            if (path_segment.size() == 0) {
                std::vector<unsigned> nothing;
                return nothing;
            }
            whole_path.insert(whole_path.end(), path_segment.begin(), path_segment.end());
        }
    }
    
    return whole_path;
}

std::vector<unsigned> construct_path(std::vector<unsigned> &pick_up_list, std::vector<drop_off_point> &drop_off_list) {
    std::vector<unsigned> input_list = pick_up_list;
    for (unsigned i = 0; i < drop_off_list.size(); i++) {
        if (drop_off_list[i].has_picked_up == true) {
            input_list.push_back(drop_off_list[i].dropOff);
        }
    }
    return input_list;
}

bool erase_pick_up_point(std::vector<unsigned> &pick_up_list, std::vector<drop_off_point> &drop_off_list, unsigned checked_element) {
    int count = 0;
    bool is_pick_up_point = false;
    std::vector<unsigned>::iterator end_pos = pick_up_list.end();
    for (std::vector<unsigned>::iterator iter = pick_up_list.begin(); iter != end_pos; ++iter) {
        //std::cout << "to check if it enter for loop again" << std::endl;
        while (drop_off_list[count].has_picked_up == true) count++;

        if (*iter == checked_element) {
            is_pick_up_point = true;
            pick_up_list.erase(iter);
            --iter;
            end_pos = pick_up_list.end();
            //std::cout << "to check which vector access is invalid" << std::endl;
            //std::cout << "the count is reach:  " << count << std::endl;
            drop_off_list[count].has_picked_up = true;
            //std::cout << "the size of pick_up_list is:      " << pick_up_list.size() << std::endl;
            //std::cout << "check if this statement is executed" << std::endl;
        }

        count++;
    }
    //std::cout << "check if it is returned " << std::endl;
    return is_pick_up_point;
}

void erase_drop_off_point(std::vector<drop_off_point> &drop_off_list, unsigned checked_element) {
    std::vector<drop_off_point>::iterator end_pos = drop_off_list.end();
    for (std::vector<drop_off_point>::iterator iter = drop_off_list.begin(); iter != end_pos; ++iter) {
        if (iter->dropOff == checked_element && iter->has_picked_up == true) {
            drop_off_list.erase(iter);
            --iter;
            end_pos = drop_off_list.end();
        }
    }
}

//void debug_print_pick_up_list(std::vector<unsigned> pick_up_list) {
//    for (unsigned i = 0; i < pick_up_list.size(); i++) {
//        std::cout << pick_up_list[i] << " ,  ";
//    }
//    std::cout << "\n";
//}

//void debug_print_drop_off_list(std::vector<drop_off_point> drop_off_list) {
//    for (unsigned i = 0; i < drop_off_list.size(); i++) {
//        std::cout << drop_off_list[i].dropOff << " ,  ";
//    }
//    std::cout << "\n";
//    for (unsigned i = 0; i < drop_off_list.size(); i++) {
//        if (drop_off_list[i].has_picked_up == true)
//            std::cout << "T " << "   ,    ";
//        else
//            std::cout << "F " << "   ,    ";
//    }
//    std::cout << "\n";
//}


std::vector<unsigned> traveling_courier_billy_modified(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots, const float turn_penalty) {
    auto startTime = std::chrono::high_resolution_clock::now();
    bool timeOut = false;
    std::vector<unsigned> pick_up_list;
    std::vector<drop_off_point> drop_off_list;

    for (unsigned i = 0; i < deliveries.size(); i++) {
        //std::cout << deliveries[i].pickUp << "  ,  " << deliveries[i].dropOff << std::endl;
        pick_up_list.push_back(deliveries[i].pickUp);
        drop_off_point temp_point;
        temp_point.dropOff = deliveries[i].dropOff;
        drop_off_list.push_back(temp_point);
    }
    //first begin with one start intersection : pick_up_list[1]
    //try to use multi_thread here
    //first begin with serial logic
    double min_time = std::numeric_limits<double>::infinity();
    std::vector<unsigned> min_path;

    for (unsigned i = 0; i < pick_up_list.size() && !timeOut; i++) {
        //std::cout << "inside greedy search loop" << std::endl; ////////////////////////////////////////////
        std::vector<unsigned> temp_path = greedy_search(i, pick_up_list, drop_off_list, turn_penalty);
        if (temp_path.size() == 0) continue;
        std::vector<unsigned> to_depot = find_path_between_intersections_help(destination_intersection_id, depots, turn_penalty); //very danger here
        if (to_depot.size() == 0) continue;


        double first_path_time = std::numeric_limits<double>::infinity();
        std::vector<unsigned> first_path;
        for (unsigned j = 0; j < depots.size(); j++) {
            std::vector<unsigned> from_depot = find_path_between_intersections(depots[j], pick_up_list[i], turn_penalty);
            if (from_depot.size() == 0) continue;
            double first_path_time_temp = compute_path_travel_time(from_depot, turn_penalty);
            if (first_path_time_temp < first_path_time) {
                first_path_time = first_path_time_temp;
                first_path = from_depot;
            }
        }

        std::vector<unsigned> complete_path(first_path.begin(), first_path.end());
        complete_path.insert(complete_path.end(), temp_path.begin(), temp_path.end());
        complete_path.insert(complete_path.end(), to_depot.begin(), to_depot.end());
        double temp_time = compute_path_travel_time(complete_path, turn_penalty);
        if (temp_time < min_time) {
            min_time = temp_time;
            min_path = complete_path;
        }
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto wallClock = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - startTime);
        if (wallClock.count() > 0.9 * TIME_LIMIT) timeOut = true;
    }
    return min_path;

}






