///*
// * To change this license header, choose License Headers in Project Properties.
// * To change this template file, choose Tools | Templates
// * and open the template in the editor.
// */
//
///* 
// * File:   interface.h
// * Author: panzhi2
// *
// * Created on April 6, 2017, 4:09 PM
// */
//
//#ifndef INTERFACE_H
//#define INTERFACE_H
//
//
//#include "m1.h"
//#include "m2.h"
//#include "m3.h"
//#include "dir_mode.h"
//#include "StreetsDatabaseAPI.h"
//#include "graphics.h"
//#include "math.h"
//#include <algorithm>
//#include "global.h"
//#include "string.h"
//#include <chrono>
//
//
///*******************************************************************************
// *                          Function Declaration
// *******************************************************************************/
//// drawing functions: 
//// graphic initialization
//void initialize_map_coordinate();
//void draw_screen();
//void change_map();
//void act_on_input_map_button_func(void (*drawscreen_ptr) (void));
//void act_on_help_button_func(void (*drawscreen_ptr) (void));
//// basic map element
//void draw_intersections();
//void draw_level_street(std::vector<StreetSegmentIndex> level, int name_size, bool direction_path_mode);
//void draw_features();
//void draw_streets();
//// map labels
//void draw_street_name(t_point start, t_point end, std::string name, int name_size);
//void draw_direction(t_point start, t_point end, unsigned current_segment, bool direction_path_mode);
//void determine_direction(unsigned intersection_start, unsigned intersection_end, std::vector<unsigned> &street_segments);
//void determine_direction_help(unsigned intersection_id_from, unsigned intersection_id_to, unsigned cross_street_segment);
//void draw_marker(LatLon position);
//void draw_point_of_interest_name();
//std::vector<value> nearby_search_POI_from_POI_types(std::string user_input_type);
//void draw_point_of_interest_icon(t_point centre, float off_set_x, float off_set_y, unsigned current_poi);
//POI_type hashit(std::string const& inString); // list of POI icon
//void draw_star(LatLon position);
//void draw_start_point_input();
//void draw_destination_input();
//// search bar and info bar with search result
//void draw_help_information();
//void draw_search_bar_background();
//void draw_info_bar_background();
//void draw_search_button();
//void draw_info_bar_intersection_name(unsigned id);
//void draw_text();
//void draw_info_bar_five_near_POI(unsigned intersection_id);
//void draw_name_on_map(std::string name, LatLon ll);
//void to_specific_intersection(unsigned id, float x, float y); //zoom to specifc intersection and display on map
//void search_bar_display_result_not_found();
//void search_bar_display_request_user_input();
//void search_bar_display_click_on_search_bar();
//std::vector<unsigned> search_intersection_from_street_names(std::string inter_name);
//void draw_direction_button();
//void draw_input_line();
//void draw_cross_button();
//void draw_direction_bar_background();
//void draw_first_direction_bar_input_background();
//void draw_second_direction_bar_input_background();
//void act_on_mouse_click_on_direction_search(int x_scrn, int y_scrn);
//// graphic set up of draw street
//void draw_screen_park_path(float per_zoom);
//void draw_screen_search_level(float per_zoom);
//void draw_screen_street_level(float per_zoom);
//void draw_screen_block_level(float per_zoom);
//void draw_screen_start_level(float per_zoom);
////user interface interaction 
//void act_on_mouse_click(float x, float y, t_event_buttonPressed event);
//void act_on_mouse_click_on_map(float x, float y);
//void act_on_mouse_click_on_search_bar(int x_scrn, int y_scrn);
//void act_on_key_press(char key_pressed, int keysym);
//void direction_search();
//void place_search();
//// calculation functions
//float cartesian_to_longitude(float cartesian_lon);
//float cartesian_to_latitude(float cartesian_lat);
//float percentage_zoom();
//void search_input_intersections();
//// m3 related draw functions
//void draw_path();
//// path_bound find_path_bound (const std::vector<unsigned>& path);
//void enter_pressed_direction_mode();
//bool search_direction_setup();
//void handle_search_poi(unsigned start_id, std::string poi_name);
//
//void auto_complete();
//void draw_info_bar_auto_complete();
//void act_on_mouse_click_auto_complete(int yscrn);
//void set_custom_screen(const std::vector<unsigned>& path);
//void list_multi_point(std::vector<unsigned> id_list, std::string type);
//void multi_point_message();
//void result_not_found_message();
//void same_message();
//void draw_instruction ();
//void path_zoom(std::vector<unsigned> dir_path);
//void draw_start_pin(LatLon position);
//
//void draw_screen_direction_mode();
//void draw_screen_search_mode();
//
///*******************************************************************************
// *                          Global Variable Declaration
// *******************************************************************************/
//
//// stores the calculated coordinated system in cartesian form
//t_bound_box coord_cs;
//
//// for mouse click
//bool has_marked_point = false;
//LatLon marked_latlon;
//unsigned marked_intersection;
//static int num_help_button_clicks = 0;
//
//// for key press
//// user input from graphic interface
//std::string input;
//
//bool draw_screen_from_act_on_button_pressed = false;
//bool has_clicked_button_changed_map = false;
//bool has_clicked_help_button = false;
//bool has_clicked_search_bar = false;
//
////for auto-complete
//std::vector<std::string> matches;
//int selected_auto_complete = -1;
//
//
//
//// class variable
//dir_mode direction_bar;
//
//#endif /* INTERFACE_H */
//
