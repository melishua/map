///*
// * To change this license header, choose License Headers in Project Properties.
// * To change this template file, choose Tools | Templates
// * and open the template in the editor.
// */
//
///* 
// * File:   direction_mode.h
// * Author: panzhi2
// *
// * Created on April 6, 2017, 3:31 PM
// */
//
//#ifndef DIR_MODE_H
//#define DIR_MODE_H
//
//#include "StreetsDatabaseAPI.h"
//
//class dir_mode {
//private:
//// state boolean variables
//bool direction_mode = false;
//bool has_clicked_first_direction_bar = false;
//bool has_clicked_second_direction_bar = false;
//bool direction_search_found = false;
//bool search_poi_flag = false;
//bool start_point_input_from_click = false;
//bool end_point_input_from_click = false;
//// if path should be drawn on map
//bool display_path = false;
//// user input intersection name
//std::string start_point_input;
//std::string destination_input;
//unsigned start_point_id;
//unsigned end_point_id;
//// searched path
//std::vector<unsigned> display_path_list;
//// instruction
//std::vector<std::string> instruction;
//    
//public:
//    dir_mode();
//    ~dir_mode();
//    // determine the direction instruction of the current path
//    std::vector<std::string> direction_instruction(std::vector<unsigned> path); 
//    std::string direction_instruction_helper (const unsigned prv_id, const std::vector<unsigned>& path, const unsigned i, const std::string& street_name);
//    
//    
//    // search for path after user enter start point and destination
//    std::vector<unsigned> handle_search_inters(unsigned start_id, unsigned end_id);
//    std::vector<unsigned> handle_search_poi(unsigned start_id, std::string poi_name);
//    
//    // reset all parameter
//    void reset();
//    
//    // access functions
//    bool get_direction_mode();
//    bool get_display_path();
//    std::string get_start_point_input();
//    std::string get_destination_input();
//    // changing variable function
//    void swap_input();
//    void set_direction_mode(bool mode);
//    void set_display_path(bool set);
//    void set_start_point_input(std::string input);
//    void set_destination_input(std::string input);
//    
//    
//    
//    
//    
//};
//
//
//
//#endif /* DIR_MODE_H */
//
