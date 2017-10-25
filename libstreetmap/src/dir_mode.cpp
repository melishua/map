///*
// * To change this license header, choose License Headers in Project Properties.
// * To change this template file, choose Tools | Templates
// * and open the template in the editor.
// */
//
//#include "dir_mode.h"
//
///******************************* class set up *********************************/
//
//// constructor does nothing
//
//dir_mode::dir_mode() {
//
//}
//
//
//// destructor resets all parameter
//
//dir_mode::~dir_mode() {
//    reset();
//}
//
//// resets all parameter
//
//void dir_mode::reset() {
//    // state boolean variables
//    direction_mode = false;
//    has_clicked_first_direction_bar = false;
//    has_clicked_second_direction_bar = false;
//
//    direction_search_found = false;
//    start_point_id = 0;
//    end_point_id = 0;
//    search_poi_flag = false;
//    start_point_input_from_click = false;
//    end_point_input_from_click = false;
//    // need to implement
//    display_path = false;
//
//    // input names:
//    start_point_input.clear();
//    destination_input.clear();
//
//    display_path_list.clear();
//    // instruction
//    instruction.clear();
//}
//
///**************************** function implementation *************************/
//
//std::vector<std::string> dir_mode::direction_instruction(std::vector<unsigned> path) {
//    // return back if size of path is zero
//    if (path.size() == 0) return path;
//
//    // clear the original instruction
//    instruction.clear();
//
//    // initialize and declare variable
//    // save the start point instruction
//    unsigned i = 0;
//    std::string prv_name = mymap.street_segments_list[path[i]].street_name;
//    unsigned prv_id = path[i];
//    std::string first_ins = "Head straight on " + prv_name + ".";
//    std::cout << std::endl << "Navigation Instruction:" << std::endl << first_ins << std::endl;
//    instruction.push_back(first_ins);
//
//    // loop through path to determine route instruction
//    while (i < path.size()) {
//        std::string street_name = mymap.street_segments_list[path[i]].street_name;
//        if (street_name != prv_name) {
//            instruction.push_back(direction_instruction_helper(prv_id, path, i, street_name));
//        }
//        // save new previous street
//        prv_name = street_name;
//        prv_id = path[i];
//        i++;
//    }
//    std::string end_ins = "Destination: " + destination_input;
//    if (search_poi_flag) {
//        end_ins = end_ins + " locate at: " + mymap.intersection_list[destination_intersection_id].name;
//    }
//    //instruction.push_back(end_ins);
//    instruction =
//            std::cout << end_ins << std::endl << std::endl << std::endl;
//}
//
//// this is the helper function for direction instruction which returns the corresponding
//// instruction for a path segment
//
//std::string dir_mode::direction_instruction_helper(const unsigned prv_id, const std::vector<unsigned>& path, const unsigned i, const std::string& street_name) {
//    // determine turn angle
//    // find intersecting point
//    std::string instr;
//    unsigned prv_from = mymap.street_segments_list[prv_id].from;
//    unsigned prv_to = mymap.street_segments_list[prv_id].to;
//    unsigned cur_from = mymap.street_segments_list[path[i]].from;
//    unsigned cur_to = mymap.street_segments_list[path[i]].to;
//    unsigned rel_pt;
//    unsigned srt_pt;
//    unsigned end_pt;
//    LatLon rel_ll;
//    LatLon srt_ll;
//    LatLon end_ll;
//    if (prv_from == cur_from) {
//        rel_pt = prv_from;
//        srt_pt = prv_to;
//        end_pt = cur_to;
//    } else if (prv_from == cur_to) {
//        rel_pt = prv_from;
//        srt_pt = prv_to;
//        end_pt = cur_from;
//    } else if (prv_to == cur_from) {
//        rel_pt = prv_to;
//        srt_pt = prv_from;
//        end_pt = cur_to;
//    } else if (prv_to == cur_to) {
//        rel_pt = prv_to;
//        srt_pt = prv_from;
//        end_pt = cur_from;
//    }
//    rel_ll = mymap.intersection_list[rel_pt].position;
//    srt_ll = mymap.intersection_list[srt_pt].position;
//    end_ll = mymap.intersection_list[end_pt].position;
//
//    // find slope, y = mx + b, b = y - mx
//    float m = (end_ll.lat() - srt_ll.lat()) / (end_ll.lon() - srt_ll.lon());
//    float b = end_ll.lat() - m * end_ll.lon();
//    // find g(rel_lon) = expected lat
//    float g_rel_lon = m * rel_ll.lon() + b;
//    // determine turn direction
//    if (m >= 0) {
//        if (rel_ll.lat() > g_rel_lon) {
//            // right
//            instr = "turn right onto " + street_name;
//        } else if (rel_ll.lat() < g_rel_lon) {
//            // left
//            instr = "turn left onto " + street_name;
//        } else {
//            // straight
//            instr = "head straight onto " + street_name;
//        }
//    } else {
//        if (rel_ll.lat() > g_rel_lon) {
//            // left
//            instr = "turn left onto " + street_name;
//        } else if (rel_ll.lat() < g_rel_lon) {
//            // right
//            instr = "turn right onto " + street_name;
//        } else {
//            // straight
//            instr = "head straight onto " + street_name;           
//        }
//    }
//    
//    std::cout << instr << std::endl;
//    return instr;
//}
//
//// response to search two intersection
//std::vector<unsigned> handle_search_inters(unsigned start_id, unsigned end_id) {
//    // find path
//    std::vector<unsigned> dir_path = find_path_between_intersections(start_id, end_id, 15);
//    display_path_list = dir_path;
//    if (dir_path.size() == 0) {
//        result_not_found_message();      
//    }
//    
//  return dir_path;     
//}
//
//std::vector<unsigned> handle_search_poi(unsigned start_id, std::string poi_name) {
//    std::vector<unsigned> dir_path = find_path_to_point_of_interest(start_id, poi_name, 15);
//    display_path_list = dir_path;
//    if (dir_path.size() == 0) {
//        result_not_found_message();
//    }
//    return dir_path;  
//}
//
///******************************** accessible function **************************/
//bool dir_mode::get_direction_mode() {
//    return direction_mode;
//}
//
//bool dir_mode::get_display_path() {
//    return display_path;
//}
//
//std::string dir_mode::get_start_point_input() {
//    return start_point_input;
//}
//std::string dir_mode::get_destination_input() {
//    return destination_input;
//}
//
///************************* changing parameter function *************************/
//void dir_mode::swap_input() {
//    // swap saved name
//    std::string temp = start_point_input;
//    start_point_input = destination_input;
//    destination_input = temp;
//    // swap saved id
//    unsigned tem = start_point_id;
//    start_point_id = end_point_id;
//    end_point_id = tem;
//    // reset parameters
//    direction_search_found = false;  
//    end_point_input_from_click = true;
//    start_point_input_from_click = true;
//    has_clicked_first_direction_bar = false;
//    has_clicked_second_direction_bar = false;
//    instruction.clear();
//    display_path_list.clear(); 
//}
//
//void dir_mode::set_direction_mode(bool mode) {
//    direction_mode = mode;
//}
//
//void dir_mode::set_display_path(bool set) {
//    display_path = set;
//}
//
//void dir_mode::set_start_point_input(std::string input) {
//    start_point_input = input;
//}
//
//void dir_mode::set_destination_input(std::string input) {
//    destination_input = input;
//}