#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "StreetsDatabaseAPI.h"
#include "graphics.h"
#include "math.h"
#include <algorithm>
#include "global.h"
#include "string.h"
#include <chrono>

/*******************************************************************************
 *                          Function Declaration
 *******************************************************************************/
// drawing functions: 
// graphic initialization
void initialize_map_coordinate();
void draw_screen();
void change_map();
void act_on_input_map_button_func(void (*drawscreen_ptr) (void));
void act_on_help_button_func(void (*drawscreen_ptr) (void));
// basic map element
void draw_intersections();
void draw_level_street(std::vector<StreetSegmentIndex> level, int name_size, bool direction_path_mode);
void draw_features();
void draw_streets();
// map labels
void draw_street_name(t_point start, t_point end, std::string name, int name_size);
void draw_direction(t_point start, t_point end, unsigned current_segment, bool direction_path_mode);
void determine_direction(unsigned intersection_start, unsigned intersection_end, std::vector<unsigned> &street_segments);
void determine_direction_help(unsigned intersection_id_from, unsigned intersection_id_to, unsigned cross_street_segment);
void draw_marker(LatLon position);
void draw_point_of_interest_name();
std::vector<value> nearby_search_POI_from_POI_types(std::string user_input_type);
void draw_point_of_interest_icon(t_point centre, float off_set_x, float off_set_y, unsigned current_poi);
POI_type hashit(std::string const& inString); // list of POI icon
void draw_star(LatLon position);
void draw_start_point_input();
void draw_destination_input();
// search bar and info bar with search result
void draw_help_information();
void draw_search_bar_background();
void draw_info_bar_background();
void draw_search_button();
void draw_info_bar_intersection_name(unsigned id);
void draw_text();
void draw_info_bar_five_near_POI(unsigned intersection_id);
void draw_name_on_map(std::string name, LatLon ll);
void to_specific_intersection(unsigned id, float x, float y); //zoom to specifc intersection and display on map
void search_bar_display_result_not_found();
void search_bar_display_request_user_input();
void search_bar_display_click_on_search_bar();
std::vector<unsigned> search_intersection_from_street_names(std::string inter_name);
void draw_direction_button();
void draw_input_line();
void draw_cross_button();
void draw_direction_bar_background();
void draw_first_direction_bar_input_background();
void draw_second_direction_bar_input_background();
void act_on_mouse_click_on_direction_search(int x_scrn, int y_scrn);
// graphic set up of draw street
void draw_screen_park_path(float per_zoom);
void draw_screen_search_level(float per_zoom);
void draw_screen_street_level(float per_zoom);
void draw_screen_block_level(float per_zoom);
void draw_screen_start_level(float per_zoom);
//user interface interaction 
void act_on_mouse_click(float x, float y, t_event_buttonPressed event);
void act_on_mouse_click_on_map(float x, float y);
void act_on_mouse_click_on_search_bar(int x_scrn, int y_scrn);
void act_on_key_press(char key_pressed, int keysym);
void direction_search();
void place_search();
// calculation functions
float cartesian_to_longitude(float cartesian_lon);
float cartesian_to_latitude(float cartesian_lat);
float percentage_zoom();
void search_input_intersections();
// m3 related draw functions
void draw_path();
// path_bound find_path_bound (const std::vector<unsigned>& path);
void enter_pressed_direction_mode();
bool search_direction_setup();
void handle_search_poi(unsigned start_id, std::string poi_name);

void auto_complete();
void draw_info_bar_auto_complete();
void act_on_mouse_click_auto_complete(int yscrn);
void set_custom_screen(const std::vector<unsigned>& path);
void handle_multi_search_point(unsigned y, std::vector<unsigned> id_list, std::string type);
void list_multi_point(std::vector<unsigned> id_list, std::string type);
void handle_search_inters(unsigned start_id, unsigned end_id);
void multi_point_message();
void result_not_found_message();
void same_message();
void direction_instruction (std::vector<unsigned> path);
void draw_instruction ();
void path_zoom(std::vector<unsigned> dir_path);
void draw_start_pin(LatLon position);

/*******************************************************************************
 *                          Global Variable Declaration
 *******************************************************************************/

// stores the calculated coordinated system in cartesian form
t_bound_box coord_cs;

// for mouse click
bool has_marked_point = false;
LatLon marked_latlon;
unsigned marked_intersection;
static int num_help_button_clicks = 0;

// for key press
// user input from graphic interface
std::string input;
std::string start_point_input;
std::string destination_input;

bool draw_screen_from_act_on_button_pressed = false;
bool has_clicked_button_changed_map = false;
bool has_clicked_help_button = false;
bool has_clicked_search_bar = false;

//for direction search
bool direction_mode = false;
bool has_clicked_first_direction_bar = false;
bool has_clicked_second_direction_bar = false;
bool direction_search_found = false;
unsigned start_point_id;
unsigned end_point_id;
bool search_poi_flag = false;
bool start_point_input_from_click = false;
bool end_point_input_from_click = false;
// need to implement
bool display_path = false;
std::vector<unsigned> display_path_list;
// instruction
std::vector<std::string> instruction;

//for auto-complete
std::vector<std::string> matches;
int selected_auto_complete = -1;

/*******************************************************************************
 *                   Function Implementation: Draw onto screen
 *******************************************************************************/
// Draws the map.

void draw_map() {
    mymap.load_layer1_map(layer1_map_name);
    // initialize background colour and map name
    t_color background_color(234, 234, 234);
    init_graphics(layer1_map_name, background_color);
    create_button("Window", "Change Map", act_on_input_map_button_func);
    create_button("Change Map", "Help", act_on_help_button_func);
    // aquire max/min lat/lon to set up the screen
    initialize_map_coordinate();

    // set visible world according to the calculated latlon in cartesian form
    set_visible_world(coord_cs);
    set_keypress_input(true);
    event_loop(act_on_mouse_click, nullptr, act_on_key_press, draw_screen);
    close_graphics();
}

// draw elements of the map

void draw_screen() {
    set_drawing_buffer(OFF_SCREEN);

    // reset screen
    clearscreen();
    draw_features();
    draw_streets();

    // double buffering
    copy_off_screen_buffer_to_screen();
    set_drawing_buffer(ON_SCREEN);

    if (!direction_mode) {
        // top left search bar
        draw_search_bar_background();
        if (input.length() == 0) {
            setcolor(DARKGREY);
            setfontsize(20);
            drawtext(100, 35, "Search Here");
        } else {
            draw_text();
        }
        draw_direction_button();
        draw_search_button();

        if (has_marked_point && !draw_screen_from_act_on_button_pressed) {
            draw_marker(marked_latlon);
            draw_info_bar_background();
            draw_info_bar_intersection_name(marked_intersection);
        }

    } else {
        draw_direction_bar_background();
        draw_cross_button();

        if (display_path) {
            draw_path();
        } else {
            draw_marker(marked_latlon);
        }
              
        if (start_point_input.length() == 0) {
            set_coordinate_system(GL_SCREEN);
            setcolor(183, 213, 234);
            setfontsize(12);
            t_bound_box start_box(30, 20, 200, 60);
            drawtext_in(start_box, "Choose Start Point");
            set_coordinate_system(GL_WORLD);
            draw_marker(marked_latlon);
        } else {
            draw_start_point_input();
        }

        if (destination_input.length() == 0) {
            set_coordinate_system(GL_SCREEN);
            setcolor(183, 213, 234);
            setfontsize(12);
            t_bound_box destination_box(30, 70, 200, 110);
            drawtext_in(destination_box, "Choose Destination");
            set_coordinate_system(GL_WORLD);
            draw_marker(marked_latlon);
        } else {
            draw_destination_input();
        }
    }
    if (has_clicked_help_button) {
        draw_help_information();
    }
}

void change_map() {
    has_clicked_button_changed_map = false;
    clearscreen();
    destroy_button("Change Map");
    close_map();
    mymap.clear();
    std::string map_path = "/cad2/ece297s/public/maps/" + user_input_map_name + ".streets.bin";
    load_map(map_path);
    draw_map();
    return;
}

void draw_start_point_input() {
    set_coordinate_system(GL_SCREEN);
    setcolor(WHITE);
    setfontsize(14);
    t_bound_box text_box(30, 10, 300, 60);
    int count = 0;
    while (!drawtext_in(text_box, start_point_input.substr(count))) {
        count++;
    }
    set_coordinate_system(GL_WORLD);
}

void draw_destination_input() {
    set_coordinate_system(GL_SCREEN);
    setcolor(WHITE);
    setfontsize(14);
    t_bound_box text_box(30, 60, 300, 110);
    int count = 0;
    while (!drawtext_in(text_box, destination_input.substr(count))) {
        count++;
    }
    set_coordinate_system(GL_WORLD);
}

/*******************************************************************************
 *                   Function Implementation: Screen Interaction
 *******************************************************************************/
void act_on_input_map_button_func(void (*drawscreen_ptr) (void)) {
    has_clicked_button_changed_map = true;
    update_message("please enter new map name here: ");
    drawscreen_ptr();
    return;
}

void act_on_help_button_func(void (*drawscreen_ptr) (void)) {
    if (num_help_button_clicks % 2 == 0)
        has_clicked_help_button = true;
    else
        has_clicked_help_button = false;

    num_help_button_clicks++;
    drawscreen_ptr();
    return;
}

void act_on_mouse_click(float x, float y, t_event_buttonPressed event) {
    has_clicked_search_bar = false;
    set_coordinate_system(GL_SCREEN);
    //react if user clicked onto the info bar
    int x_scrn = non_static_xworld_to_scrn(x);
    int y_scrn = non_static_yworld_to_scrn(y);
    std::cout << "x screen: " << x_scrn << " y screen: " << y_scrn << std::endl;
    if (has_clicked_help_button) return;
    display_path = false;
    if ((x_scrn > 10 && x_scrn < 400) && (y_scrn > 10 && y_scrn < 350)) {
        if (direction_mode) {            
            act_on_mouse_click_on_direction_search(x_scrn, y_scrn);
        } else if (y_scrn > 60 && input.size() != 0) {
            act_on_mouse_click_auto_complete(y_scrn);
        } else {
            act_on_mouse_click_on_search_bar(x_scrn, y_scrn);
        }
    } else {
        act_on_mouse_click_on_map(x, y);
    }
}

void act_on_mouse_click_auto_complete(int y_scrn) {
    if (y_scrn < 118) {
        input = matches[0];
    } else if (y_scrn < 176) {
        input = matches[1];
    } else if (y_scrn < 234) {
        input = matches[2];
    } else if (y_scrn < 292) {
        input = matches[3];
    } else {
        input = matches[4];
    }
    place_search();
    selected_auto_complete = -1;
    matches.clear();
}

void act_on_mouse_click_on_map(float x, float y) {
    // transform current click point to latlon
    auto const lon = cartesian_to_longitude(x);
    auto const lat = cartesian_to_latitude(y);
    LatLon position(lat, lon);
    // terminal out put
    std::cout << "User clicked a mouse button at coordinates ("
            << x << "," << y << ")" << std::endl;

    marked_intersection = find_closest_intersection(position);
    std::cout << "Closest intersection: " << mymap.intersection_list[marked_intersection].name << "\n";
    float per_zoom = percentage_zoom();
    if (direction_mode) {
        if (has_clicked_first_direction_bar) {
            draw_first_direction_bar_input_background();
            start_point_input = mymap.intersection_list[marked_intersection].name;
            draw_start_point_input();
            start_point_id = marked_intersection;
            start_point_input_from_click = true;
        } else if (has_clicked_second_direction_bar) {
            draw_second_direction_bar_input_background();
            destination_input = mymap.intersection_list[marked_intersection].name;
            draw_destination_input();
            end_point_id = marked_intersection;
            end_point_input_from_click = true;
        }
        has_marked_point = true;
        marked_latlon = mymap.intersection_list[marked_intersection].position;
        to_specific_intersection(marked_intersection, x, y);

    } else {
        // zoom to the found position
        if (per_zoom < 0.4) {
            marked_latlon = position;
            has_marked_point = true;
            to_specific_intersection(marked_intersection, x, y);
            draw_info_bar_background();
            draw_info_bar_intersection_name(marked_intersection);
            draw_info_bar_five_near_POI(marked_intersection);
        } else {
            draw_screen();
        }
    }

}

void act_on_mouse_click_on_search_bar(int x_scrn, int y_scrn) {
    // press on search input box
    if ((x_scrn > 10 && x_scrn < 300) && (y_scrn > 10 && y_scrn < 60)) {
        has_clicked_search_bar = true;
        if (input.length() == 0) {
            draw_search_bar_background();
        }
    }// press on search button
    else if ((x_scrn > 300 && x_scrn < 350) && (y_scrn > 10 && y_scrn < 60)) {
        act_on_key_press(13, 0);
    }// press on direction button
    else if ((x_scrn > 350 && x_scrn < 400) && (y_scrn > 10 && y_scrn < 60)) {
        direction_mode = true;       
        draw_screen();
    }
}

void act_on_mouse_click_on_direction_search(int x_scrn, int y_scrn) {
    if ((x_scrn > 10 && x_scrn < 300) && (y_scrn > 10 && y_scrn < 60)) { // start input box
        has_clicked_first_direction_bar = true;
        has_clicked_second_direction_bar = false;
        if (start_point_input.length() == 0) {
            draw_first_direction_bar_input_background();
        }
        display_path = false;
    } else if ((x_scrn > 10 && x_scrn < 300) && (y_scrn > 60 && y_scrn < 110)) { // destination input box
        has_clicked_second_direction_bar = true;
        has_clicked_first_direction_bar = false;
        if (destination_input.length() == 0) {
            draw_second_direction_bar_input_background();
        }
        display_path = false;
    } else if ((x_scrn > 370 && x_scrn < 400) && (y_scrn > 10 && y_scrn < 40)) { // back to search mode
        direction_mode = false;
        has_clicked_first_direction_bar = false;
        has_clicked_second_direction_bar = false;
        end_point_input_from_click = false;
        start_point_input_from_click = false;
        direction_search_found = false;
        display_path = false;
        start_point_id = 0;
        end_point_id = 0;
        display_path_list.clear();
        start_point_input.clear();
        destination_input.clear();
        instruction.clear();
        draw_screen();
    } else if ((x_scrn > 350 && x_scrn < 380) && (y_scrn > 50 && y_scrn < 80)) { // swap input
        std::string temp = start_point_input;
        start_point_input = destination_input;
        destination_input = temp;
        unsigned tem = start_point_id;
        start_point_id = end_point_id;
        direction_search_found = false;
        display_path_list.clear();
        instruction.clear();
        display_path = false;
        end_point_id = tem;
        end_point_input_from_click = true;
        start_point_input_from_click = true;
        draw_direction_bar_background();
        draw_cross_button();
        draw_start_point_input();
        draw_destination_input();
        //draw_screen();
    }
}

void act_on_key_press(char key_pressed, int keysym) {
    set_coordinate_system(GL_SCREEN);

    if (has_clicked_help_button) return;

    if (!direction_mode) {
        draw_search_bar_background();
        if (key_pressed == 8 && !input.empty()) { //if backspace is pressed
            input.pop_back();
            if (input.size() != 0) {
                matches.clear();
                auto_complete();
                draw_info_bar_auto_complete();
            }
        } else if (key_pressed == 13) { //if enter is pressed, start searching
            if (selected_auto_complete != -1) {
                input = matches[selected_auto_complete];
            }
            place_search();
            selected_auto_complete = -1;
            matches.clear();
        } else if (key_pressed > 31) { // only draw printable char
            std::string temp_char(1, key_pressed);
            input += temp_char;
            auto_complete();
            draw_info_bar_auto_complete();
        } else if (key_pressed == 9) { //if tab is pressed, auto_complete the first result
            input = matches[0];
        } else if (keysym == 65362 || keysym == 65364) { //65362 up arrow, 65364 down arrow
            if (keysym == 65362 && selected_auto_complete >= 0) {
                draw_info_bar_auto_complete();
                set_coordinate_system(GL_SCREEN);
                selected_auto_complete--;
                if (selected_auto_complete >= 0) {
                    setcolor(245, 245, 245);
                    fillrect(11, 61 + selected_auto_complete * 58, 400, 118 + selected_auto_complete * 58);
                    t_bound_box temp(15, 60 + selected_auto_complete * 58, 395, 118 + selected_auto_complete * 58);
                    setcolor(BLACK);
                    setfontsize(18);
                    drawtext_in(temp, matches[selected_auto_complete]);

                }
            } else if (keysym == 65364 && selected_auto_complete < 4) {
                draw_info_bar_auto_complete();
                set_coordinate_system(GL_SCREEN);
                selected_auto_complete++;
                setcolor(245, 245, 245);
                fillrect(11, 61 + selected_auto_complete * 58, 400, 118 + selected_auto_complete * 58);
                t_bound_box temp(15, 60 + selected_auto_complete * 58, 395, 118 + selected_auto_complete * 58);
                setcolor(BLACK);
                setfontsize(18);
                drawtext_in(temp, matches[selected_auto_complete]);
            }
        }
        if (input.size() != 0) draw_text();

    } else if (has_clicked_first_direction_bar) {
        draw_first_direction_bar_input_background();
        if (key_pressed == 8 && !start_point_input.empty()) { //if backspace is pressed
            start_point_input.pop_back();
            start_point_input_from_click = false;
        } else if (key_pressed == 13) { //if enter is pressed, start searching
            if (start_point_input == destination_input) {
                same_message();
            } else{
                direction_search();
            }
        } else if (key_pressed > 31) { // only draw printable char
            std::string temp_char(1, key_pressed);
            start_point_input += temp_char;
            start_point_input_from_click = false;
        }

        draw_start_point_input();
    } else if (has_clicked_second_direction_bar) {
        draw_second_direction_bar_input_background();
        if (key_pressed == 8 && !destination_input.empty()) { //if backspace is pressed
            destination_input.pop_back();
            end_point_input_from_click = false;
        } else if (key_pressed == 13) { //if enter is pressed, start searching
            if (start_point_input == destination_input) {
                same_message();
            } else {
                direction_search();
            }
        } else if (key_pressed > 31) { // only draw printable char
            std::string temp_char(1, key_pressed);
            destination_input += temp_char;
            end_point_input_from_click = false;
        }
        draw_destination_input();
    } else if (has_clicked_button_changed_map) {
        if (key_pressed == 8 && !user_input_map_name.empty()) { //if backspace is pressed
            user_input_map_name.pop_back();
        } else if (key_pressed == 13) { //if enter is pressed, start searching
            change_map();
        } else if (key_pressed > 31) { // only draw printable char
            std::string temp_char(1, key_pressed);
            user_input_map_name += temp_char;
        }
        update_message(user_input_map_name);
    }
    set_coordinate_system(GL_WORLD);
}

void auto_complete() {
    int count = -1;
    //Search through intersection_names until we find a match
    while (count < (int) mymap.street_name_list.size() - 1 && matches.size() < 5) {
        count++;
        if (strncasecmp(mymap.street_name_list[count].c_str(), input.c_str(), input.size()) == 0) {
            std::string temp = mymap.street_name_list[count];
            matches.push_back(temp);
        }
    }
}

void draw_info_bar_auto_complete() {
    if (matches.size() == 0) return;
    set_coordinate_system(GL_SCREEN);
    draw_info_bar_background();
    for (unsigned i = 0; i < matches.size(); i++) {
        t_bound_box box_poi_name(15, 60 + i * 58, 395, 118 + i * 58);
        setcolor(DARKGREY);
        setlinestyle(SOLID);
        setlinewidth(1);
        drawrect(10, 60 + i * 58, 400, 118 + i * 58);
        setfontsize(18);
        drawtext_in(box_poi_name, matches[i]);
    }
}

void direction_search() {
    // return if failed to set up
    if (!search_direction_setup()) {
        direction_search_found = false;
        return;
    }
    // response according to search type
    display_path = true;
    if (search_poi_flag) {
        instruction.clear();
        handle_search_poi(start_point_id, destination_input);       
        direction_search_found = true;
    } else {
         instruction.clear();
        handle_search_inters(start_point_id, end_point_id);       
        direction_search_found = true;
    }
}

void place_search() {
    auto start = std::chrono::high_resolution_clock::now();
    if (!input.empty()) {
        set_coordinate_system(GL_WORLD);
        if (input == "hospital" || input == "restaurant" || input == "bank" || input == "fuel" || input == "infrastructure" || input == "school" || input == "parking"
                || input == "entertainment" || input == "shop" || input == "postal" || input == "hotel" || input == "transportation" || input == "religion") {
            draw_screen();
            nearby_search_POI_from_POI_types(input);
        } else {
            search_input_intersections();
        }
    } else {
        search_bar_display_request_user_input();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << "the speed between user input ENTER and the time shown on map is: " << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << "ms\n";
}

void search_bar_display_request_user_input() {
    std::cout << "" << std::endl;
    draw_info_bar_background();
    setcolor(LIGHTGREY);
    setfontsize(12);
    t_bound_box box1(15, 90, 395, 110);
    drawtext_in(box1, "Please click Search Bar and input street names above");
    t_bound_box box2(15, 110, 395, 140);
    drawtext_in(box2, "Correct format: street name1 & street name2");
}

void search_input_intersections() {
    std::vector<unsigned> search_result = search_intersection_from_street_names(input);
    if (search_result.size() == 0) {
        search_bar_display_result_not_found();
    } else {
        has_marked_point = true;
        for (unsigned current_int = 0; current_int < search_result.size(); current_int++) {
            LatLon temp = mymap.intersection_list[search_result[current_int]].position;

            float x = mymap.longitude_to_cartesian(temp.lon());
            float y = mymap.latitude_to_cartesian(temp.lat());
            //std::cout << x << "," << y << std::endl;
            //std::cout << "marker drawn from search_input_intersections" << std::endl;
            draw_marker(temp);
            if (current_int == 0) {
                marked_latlon = temp;
                marked_intersection = search_result[0];
                float per_zoom = percentage_zoom();
                float zoom_to_start_level = 1 / per_zoom;
                t_bound_box bounds = get_visible_world();
                search_zoom_in(zoom_to_start_level, bounds.get_xcenter(), bounds.get_ycenter(), draw_screen);
                to_specific_intersection(search_result[0], x, y);
                draw_info_bar_background();
                draw_info_bar_intersection_name(search_result[0]);
                draw_info_bar_five_near_POI(search_result[0]);
            }
        }
    }
}

std::vector<unsigned> search_intersection_from_street_names(std::string inter_name) {
    //extract first & second street name
    size_t amp_position = inter_name.find_first_of("&");
    std::string street_name1 = inter_name.substr(0, amp_position);
    std::string street_name2 = inter_name.substr(amp_position + 1);

    //eliminate white leading and terminal white space
    size_t begin1 = street_name1.find_first_not_of(" ");
    size_t end1 = street_name1.find_last_not_of(" ");
    size_t range1 = end1 - begin1 + 1;
    street_name1 = street_name1.substr(begin1, range1);

    size_t begin2 = street_name2.find_first_not_of(" ");
    size_t end2 = street_name2.find_last_not_of(" ");
    size_t range2 = end2 - begin2 + 1;
    street_name2 = street_name2.substr(begin2, range2);

    std::vector <unsigned> id = find_intersection_ids_from_street_names(street_name1, street_name2);
    std::cout << id.size() << std::endl;
    return find_intersection_ids_from_street_names(street_name1, street_name2);
}

std::vector<value> nearby_search_POI_from_POI_types(std::string user_input_type) {
    draw_info_bar_background();
    set_coordinate_system(GL_WORLD);
    t_bound_box bounds = get_visible_world();
    std::vector<value> result;
    int count = 0;
    mymap.rtree_poi.query(bgi::nearest(point(bounds.get_xcenter(), bounds.get_ycenter()), 50), std::back_inserter(result));

    set_coordinate_system(GL_SCREEN);
    t_bound_box box1(15, 90, 345, 110);

    std::string symbol = "Nearby POI: " + user_input_type;
    drawtext_in(box1, symbol);

    for (unsigned i = 0; i < 50; i++) {
        std::string poi_type = mymap.poi_list[result[i].second].type;
        if (poi_type == user_input_type) {
            draw_name_on_map(mymap.poi_list[result[i].second].name, mymap.poi_list[result[i].second].position);
            if (count < 5) {
                t_bound_box box_poi_name(15, 145 + count * 25, 345, 165 + count * 25);
                std::string poi_name = mymap.poi_list[result[i].second].name;
                if (poi_name.size() < 40) {
                    setcolor(DARKGREY);
                    setfontsize(11);
                    drawtext_in(box_poi_name, poi_name);
                } else {
                    setcolor(LIGHTGREY);
                    setfontsize(10);
                    drawtext_in(box_poi_name, "please check terminal for name");
                }
            }
            count++;
        }
    }
    if (count == 0) {
        t_bound_box box_poi_name(15, 145, 345, 165);
        drawtext_in(box_poi_name, "NO Related POI FOUND");
    }
    return result;
}

/*******************************************************************************
 *                   Function Implementation: Calculation Functions
 *******************************************************************************/

// aquire maximum and minimum lat/lon information of the loaded city

void initialize_map_coordinate() {
    // store found max/min latlon in cartesian form
    t_bound_box ini_coord(mymap.minimum_longitude * cos(mymap.average_latitude) * DEG_TO_RAD,
            mymap.minimum_latitude * DEG_TO_RAD,
            mymap.maximum_longitude * cos(mymap.average_latitude) * DEG_TO_RAD,
            mymap.maximum_latitude * DEG_TO_RAD);
    coord_cs = ini_coord;
}

// convert cartesian to longitude

float cartesian_to_longitude(float cartesian_lon) {
    return cartesian_lon / (cos(mymap.average_latitude) * DEG_TO_RAD);
}

// convert cartesian to longitude

float cartesian_to_latitude(float cartesian_lat) {
    return cartesian_lat / DEG_TO_RAD;
}

// calculate percentage zoom in UI from visible world
// per_zoom > 1: current view is greater than the setup view
// per_zoom < 1: current view is smaller than the setup view

float percentage_zoom() {
    // compute original width and height
    float origin_width = coord_cs.right() - coord_cs.left();

    // compute current width and height
    t_bound_box current_view = get_visible_world();
    float current_width = current_view.right() - current_view.left();
    ;

    // compute percentage zoom
    float per_zoom = current_width / origin_width;
    return per_zoom;
}

/*******************************************************************************
 *                   Function Implementation: Draw Functions
 *******************************************************************************/
// draw streets according to pass in level

void draw_level_street(std::vector<StreetSegmentIndex> level, int name_size, bool direction_path_mode) {
    set_coordinate_system(GL_WORLD);
    setlinestyle(SOLID, ROUND);
    for (unsigned current_seg = 0; current_seg < level.size(); current_seg++) {
        unsigned intersection_from = mymap.street_segments_list[level[current_seg]].from;
        unsigned intersection_to = mymap.street_segments_list[level[current_seg]].to;
        std::string stree_name_this = mymap.street_segments_list[level[current_seg]].street_name;
        LatLon begin = getIntersectionPosition(intersection_from);
        LatLon finish = getIntersectionPosition(intersection_to);

        t_point start = mymap.latlon_to_cartesian(begin);
        t_point end = mymap.latlon_to_cartesian(finish);

        unsigned curvePoints = mymap.street_segments_list[level[current_seg]].curvePointCount;
        if (curvePoints != 0) {
            t_point firstCurvePoint = mymap.latlon_to_cartesian(mymap.street_segments_curvepoint[level[current_seg]].at(0));
            t_point lastCurvePoint = mymap.latlon_to_cartesian(mymap.street_segments_curvepoint[level[current_seg]].at(curvePoints - 1));
            drawline(start, firstCurvePoint);
            drawline(lastCurvePoint, end);
            draw_street_name(start, firstCurvePoint, stree_name_this, name_size);
            draw_street_name(lastCurvePoint, end, stree_name_this, name_size);
            if (!direction_path_mode) {
                draw_direction(start, firstCurvePoint, level[current_seg], direction_path_mode);
                draw_direction(lastCurvePoint, end, level[current_seg], direction_path_mode);
            }
            for (unsigned current_curve_point = 1; current_curve_point < curvePoints; current_curve_point++) {
                t_point p1 = mymap.latlon_to_cartesian(mymap.street_segments_curvepoint[level[current_seg]].at(current_curve_point - 1));
                t_point p2 = mymap.latlon_to_cartesian(mymap.street_segments_curvepoint[level[current_seg]].at(current_curve_point));
                drawline(p1, p2);
                if (!direction_path_mode) {
                    draw_direction(p1, p2, level[current_seg], direction_path_mode);
                }
                draw_street_name(p1, p2, stree_name_this, name_size);
            }
        } else {
            drawline(start, end);
            draw_street_name(start, end, stree_name_this, name_size);
            if (!direction_path_mode) {
                draw_direction(start, end, level[current_seg], direction_path_mode);
            }
        }
    }
    if (direction_path_mode){
        determine_direction(start_point_id, end_point_id, level);
    }
}

// graphic set up of start level street

void draw_screen_start_level(float per_zoom) {
    setcolor(240, 193, 131);
    float factor = std::min(12.0f, 5 / per_zoom);
    setlinewidth(factor);
    draw_level_street(mymap.start_level, factor, false);
}
// graphic set up of block level street

void draw_screen_block_level(float per_zoom) {
    setcolor(255, 255, 255);
    float factor = std::min(14.0f, 1 / per_zoom);
    setlinewidth(factor);
    draw_level_street(mymap.block_level, factor, false);
}
// graphic set up of street level street

void draw_screen_street_level(float per_zoom) {
    setcolor(255, 255, 255);
    float factor = std::min(10.0f, 1 / per_zoom);
    setlinewidth(factor);
    draw_level_street(mymap.street_level, factor, false);
}
// graphic set up of search level street

void draw_screen_search_level(float per_zoom) {
    setcolor(245, 245, 245);
    float factor = std::min(10.0f, 1 / per_zoom);
    setlinewidth(factor);
    draw_level_street(mymap.search_level, factor, false);
}

void draw_screen_park_path(float per_zoom) {
    setcolor(245, 245, 245);
    float factor = std::min(10.0f, 1 / per_zoom);
    setlinewidth(factor);
    draw_level_street(mymap.park_path, factor, false);
}

// draw street name according to street segment

void draw_help_information() {
    set_coordinate_system(GL_SCREEN);
    draw_info_bar_background();

    setcolor(BLACK);
    setfontsize(15);
    t_bound_box text_box1(15, 80, 300, 100);
    drawtext_in(text_box1, "X-map user instruction:");

    setfontsize(12);
    t_bound_box text_box2(15, 110, 395, 130);
    drawtext_in(text_box2, "Click search bar above to input");

    t_bound_box text_box3(15, 135, 395, 155);
    drawtext_in(text_box3, "Search nearby by input a poi type");

    t_bound_box text_box4(15, 160, 395, 180);
    drawtext_in(text_box4, "Press ENTER or click mag glass to start searching");

    t_bound_box text_box5(15, 185, 395, 205);
    drawtext_in(text_box5, "Click right turn icon to search direction");

    t_bound_box text_box6(15, 215, 395, 235);
    drawtext_in(text_box6, "To change map, press button on the right window");
    t_bound_box text_box7(15, 240, 395, 260);
    drawtext_in(text_box7, "and input map name to the bottom message bar");

    setcolor(DARKGREY);
    setfontsize(12);
    t_bound_box text_box8(15, 275, 395, 295);
    drawtext_in(text_box8, "please press help button again to close this window");
    set_coordinate_system(GL_WORLD);
}

/*
void draw_street_name(const t_point start, const t_point end, std::string name, int name_size) {
    set_coordinate_system(GL_WORLD);
    if (name == "<unknown>") return;
    // calculate the direction of the street segment
    double rotation_degree = atan((end.y - start.y) / (end.x - start.x)) * 180 / PI;

    t_color origin_color = getcolor();
    setcolor(DARKGREY);
    settextrotation(rotation_degree);
    setfontsize(name_size);
    // find text bound   
    double y_cen = (std::max(end.y, start.y) + std::min(end.y, start.y)) / 2;
    double x_cen = (std::max(end.x, start.x) + std::min(end.x, start.x)) / 2;
    double x_len = abs((std::max(end.x, start.x) - std::min(end.x, start.x)) / cos(rotation_degree));
    // find offset value in world coordinate
    double y_offset = non_static_yscrn_to_world(name_size) - non_static_yscrn_to_world(0);
    // set text bound
    double top = y_cen + y_offset;
    double bot = y_cen - y_offset;
    double right = x_cen + x_len / 2;
    double left = x_cen - x_len / 2;
    // draw street name onto graphics
    t_bound_box box(right, top, left, bot);
    t_bound_box box2(std::min(start.x, end.x), std::min(start.y, end.y), std::max(start.x, end.x), std::max(start.y, end.y));
    drawtext_in(box2, name);
    // reset drawing parameter
    settextrotation(0);
    setcolor(origin_color);
}
 */

void draw_street_name(t_point start, t_point end, std::string name, int name_size) {
    set_coordinate_system(GL_WORLD);
    if (name == "<unknown>") return;
    // calculate the direction of the street segment
    double rotation_degree = atan(fabs((end.y - start.y) / (end.x - start.x))) * 180 / PI;
    if ((end.y - start.y) > 0 && (end.x - start.x) > 0) {
        // first quad, rotation_degree stays
    } else if ((end.y - start.y) > 0 && (end.x - start.x) < 0) {
        // second quad, rotation_degree
        rotation_degree = 360 - rotation_degree;
    } else if ((end.y - start.y) < 0 && (end.x - start.x) < 0) {
        // third quad, rotation_degree
    } else if ((end.y - start.y) < 0 && (end.x - start.x) > 0) {
        // fourth quad, rotation_degree
        rotation_degree = 360 - rotation_degree;
    }
    // colour set up   
    t_color origin_color = getcolor();
    setcolor(DARKGREY);
    settextrotation(rotation_degree);
    setfontsize(name_size);
    // find text bound   
    double y_cen = (std::max(end.y, start.y) + std::min(end.y, start.y)) / 2;
    double x_cen = (std::max(end.x, start.x) + std::min(end.x, start.x)) / 2;
    double x_len = fabs((std::max(end.x, start.x) - std::min(end.x, start.x)) / cos(rotation_degree));
    // find offset value in world coordinate
    double y_offset = non_static_yscrn_to_world(name_size) - non_static_yscrn_to_world(0);
    // set text bound
    double top = y_cen + y_offset;
    double bot = y_cen - y_offset;
    double right = x_cen + x_len / 2;
    double left = x_cen - x_len / 2;
    // draw street name onto graphics
    t_bound_box box(right, top, left, bot);
    t_bound_box box2(left, std::min(start.y, end.y), right, std::max(start.y, end.y));
    drawtext_in(box2, name);
    // reset drawing parameter
    settextrotation(0);
    setcolor(origin_color);
}

// draw the arrow direction. very similar to draw street name 
// can combine into one function at the end in case of conflict 

void draw_direction(t_point start, t_point end, unsigned current_segment, bool direction_path_mode) {
    set_coordinate_system(GL_WORLD);
    if (mymap.street_segments_list[current_segment].oneWay == false && !direction_path_mode) {
        return;
    } else {
        t_color origin_color = getcolor();
        double rotation_degree = atan(fabs((end.y - start.y) / (end.x - start.x))) * 180 / PI;
        if ((end.y - start.y) > 0 && (end.x - start.x) > 0) {
            // first quad, rotation_degree stays
        } else if ((end.y - start.y) > 0 && (end.x - start.x) < 0) {
            // second quad, rotation_degree
            rotation_degree = 180 - rotation_degree;
        } else if ((end.y - start.y) < 0 && (end.x - start.x) < 0) {
            // third quad, rotation_degree
            rotation_degree = 180 + rotation_degree;
        } else if ((end.y - start.y) < 0 && (end.x - start.x) > 0) {
            // fourth quad, rotation_degree
            rotation_degree = 360 - rotation_degree;
        }
        //std::cout << rotation_degree << std::endl;
        if(!direction_path_mode){
            setcolor(RED);
        }
        else{
            setcolor(WHITE);
        }
        setfontsize(10);
        settextrotation(rotation_degree);
        t_point current = start;

        double del_x = (end.x - start.x);
        double del_y = (end.y - start.y);
        t_point first(current.x + del_x / 6, current.y + del_y / 6);
        t_point second(current.x + del_x / 6 * 5, current.y + del_y / 6 * 5);

        t_bound_box bounds(std::min(start.x, end.x), std::min(start.y, end.y), std::max(start.x, end.x), std::max(start.y, end.y));
        drawtext(first, "->", bounds);

        // reset graphic set up
        settextrotation(0);
        setcolor(origin_color);
    }
    return;
}



// draw the arrow direction. very similar to draw street name 
// can combine into one function at the end in case of conflict 

/*
void draw_direction(t_point start, t_point end, unsigned current_segment, bool direction_path_mode) {
    set_coordinate_system(GL_WORLD);
    if (mymap.street_segments_list[current_segment].oneWay == false && !direction_path_mode) {
        return;
    } else {
        t_color origin_color = getcolor();
        double rotation_degree = atan((end.y - start.y) / (end.x - start.x)) * 180 / PI;
        //std::cout << rotation_degree << std::endl;
        setcolor(RED);
        setfontsize(10);
        settextrotation(rotation_degree);
        t_point current = start;

        double del_x = (end.x - start.x);
        double del_y = (end.y - start.y);
        t_point first(current.x + del_x / 6, current.y + del_y / 6);
        t_point second(current.x + del_x / 6 * 5, current.y + del_y / 6 * 5);

        t_bound_box bounds(std::min(start.x, end.x), std::min(start.y, end.y), std::max(start.x, end.x), std::max(start.y, end.y));
        if ((del_x > 0 && del_y > 0) || ((del_x < 0 && del_y > 0))) {
            drawtext(first, "<-", bounds);
        } else {
            drawtext(first, "->", bounds);
        }
        // reset graphic set up
        settextrotation(0);
        setcolor(origin_color);
    }
    return;
}
 */
void draw_features() {
    set_coordinate_system(GL_WORLD);
    for (unsigned current_feature = 0; current_feature < mymap.total_number_feature; current_feature++) {

        switch (mymap.feature_area_list[current_feature].type) {
            case Park:
                setcolor(203, 230, 163, 255);
                break;
            case Beach:
                setcolor(250, 242, 199, 255);
                break;
            case Lake:
                setcolor(163, 204, 255, 255);
                break;
            case River:
                setcolor(163, 204, 255, 255);
                break;
            case Island:
                setcolor(203, 230, 163, 255);
                break;
            case Shoreline:
                setcolor(0xFF, 0XFF, 0X00, 255);
                break;
            case Building:
                setcolor(245, 236, 221, 255);
                break;
            case Greenspace:
                setcolor(221, 230, 213, 255);
                break;
            case Golfcourse:
                setcolor(214, 233, 185, 255);
                break;
            case Stream:
                setcolor(163, 204, 255, 255);
                break;
            default:
                break;
        }

        float per_zoom = percentage_zoom();
        if (mymap.feature_area_list[current_feature].closed) {
            fillpoly(&mymap.feature_area_list[current_feature].vertex_list[0], mymap.feature_area_list[current_feature].vertex_count);
        } else if (0.1 > per_zoom) {
            for (unsigned i = 0; i < mymap.feature_area_list[current_feature].vertex_count - 1; i++)
                drawline(mymap.feature_area_list[current_feature].vertex_list[i], mymap.feature_area_list[current_feature].vertex_list[i + 1]);
        }
    }
}

void draw_streets() {
    float per_zoom = percentage_zoom();
    // draw map according to the zoom level of the screen
    if (per_zoom >= 0.75) {
        draw_screen_start_level(per_zoom);
        update_message("start_level");
    } else if (0.75 > per_zoom && per_zoom >= 0.4) {
        draw_screen_block_level(per_zoom);
        draw_screen_start_level(per_zoom);
        update_message("block_level");
    } else if (0.4 > per_zoom && per_zoom >= 0.1) {
        draw_screen_street_level(per_zoom);
        draw_screen_block_level(per_zoom);
        draw_screen_start_level(per_zoom);
        update_message("street_level");
    } else if (0.1 > per_zoom && per_zoom >= 0.025) {
        draw_screen_search_level(per_zoom);
        draw_screen_street_level(per_zoom);
        draw_screen_block_level(per_zoom);
        draw_screen_start_level(per_zoom);
        update_message("search_level");
    } else if (0.025 > per_zoom && per_zoom >= 0.007) {
        draw_screen_park_path(per_zoom);
        draw_screen_search_level(per_zoom);
        draw_screen_street_level(per_zoom);
        draw_screen_block_level(per_zoom);
        draw_screen_start_level(per_zoom);
        update_message("park_path");
    } else if (0.007 > per_zoom) {
        draw_screen_park_path(per_zoom);
        draw_screen_search_level(per_zoom);
        draw_screen_street_level(per_zoom);
        draw_screen_block_level(per_zoom);
        draw_screen_start_level(per_zoom);
        draw_point_of_interest_name();
        update_message("park_path");
    }
}

void draw_point_of_interest_name() {
    set_coordinate_system(GL_WORLD);
    for (unsigned current_poi = 0; current_poi < mymap.total_number_poi; current_poi++) {
        poi_info poi = mymap.poi_list[current_poi];
        t_color origin_color = getcolor();
        setcolor(BLACK);
        setfontsize(7);
        float del_width = poi.name.size()*0.0000002;
        float del_high = 0.0000002;
        // draw POI onto graphics
        t_point CentreOfPoint(mymap.longitude_to_cartesian(poi.position.lon()), mymap.latitude_to_cartesian(poi.position.lat()));
        t_bound_box bounds(CentreOfPoint.x - del_width, CentreOfPoint.y - del_high, CentreOfPoint.x + del_width, CentreOfPoint.y + del_high);
        if (drawtext(CentreOfPoint, poi.name, bounds)) {
            if (0.007 > percentage_zoom() && percentage_zoom() > 0.0042) {
                draw_point_of_interest_icon(CentreOfPoint, 0.0000006, 0.0000016, current_poi);
            } else if (0.0042 > percentage_zoom() && percentage_zoom() > 0.00252) {
                draw_point_of_interest_icon(CentreOfPoint, 0.0000004, 0.0000012, current_poi);
            } else if (0.00252 > percentage_zoom() && percentage_zoom() > 0.001512) {
                draw_point_of_interest_icon(CentreOfPoint, 0.0000002, 0.0000008, current_poi);
            } else if (0.001512 > percentage_zoom() && percentage_zoom() > 0.0009072) {
                draw_point_of_interest_icon(CentreOfPoint, 0.0000001, 0.0000004, current_poi);
            } else if (0.0009072 > percentage_zoom() && percentage_zoom() > 0.00054432) {
                draw_point_of_interest_icon(CentreOfPoint, 0.00000008, 0.0000003, current_poi);
            } else {
                draw_point_of_interest_icon(CentreOfPoint, 0.00000006, 0.0000002, current_poi);
            }
        }
        // reset drawing parameter
        setcolor(origin_color);
    }
}

void draw_point_of_interest_icon(t_point centre, float off_set_x, float off_set_y, unsigned current_poi) {
    Surface icon;
    switch (hashit(mymap.poi_list[current_poi].type)) {
        case hospital:
        {
            icon.setSurface("libstreetmap/resources/hospital.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case restaurant:
        {
            icon.setSurface("libstreetmap/resources/restaurant.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case bank:
        {
            icon.setSurface("libstreetmap/resources/bank.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case fuel:
        {
            icon.setSurface("libstreetmap/resources/fuel.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case infrastructure:
        {
            icon.setSurface("libstreetmap/resources/infrastructure.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case school:
        {
            icon.setSurface("libstreetmap/resources/school.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case parking:
        {
            icon.setSurface("libstreetmap/resources/parking.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case entertainment:
        {
            icon.setSurface("libstreetmap/resources/entertainment.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case shop:
        {
            icon.setSurface("libstreetmap/resources/shop.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case postal:
        {
            icon.setSurface("libstreetmap/resources/postal.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case hotel:
        {
            icon.setSurface("libstreetmap/resources/hotel.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case transportation:
        {
            icon.setSurface("libstreetmap/resources/transportation.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        case religion:
        {
            icon.setSurface("libstreetmap/resources/religion.png");
            draw_surface(icon, centre.x - off_set_x, centre.y + off_set_y);
            break;
        }
        default: break;
    }
}

// categorize point of interest type

POI_type hashit(std::string const& inString) {
    if (inString == "pharmacy" || inString == "clinic" || inString == "doctors" || inString == "hospital" || inString == "dentist" || inString == "veterinary") return hospital;
    else if (inString == "fast_food" || inString == "cafe" || inString == "restaurant" || inString == "pub" || inString == "bar") return restaurant;
    else if (inString == "bank" || inString == "atm" || inString == "bureau_de_change") return bank;
    else if (inString == "fuel") return fuel;
    else if (inString == "community_centre" || inString == "social_facility" || inString == "police" || inString == "fire_station") return infrastructure;
    else if (inString == "tutoring" || inString == "kindergarten" || inString == "college" || inString == "school" || inString == "childcare" || inString == "university" || inString == "tutor" || inString == "library") return school;
    else if (inString == "parking") return parking;
    else if (inString == "theatre" || inString == "cinema" || inString == "internet_cafe" || inString == "nightclub" || inString == "fitness_center" || inString == "gym" || inString == "banquet_hall" || inString == "gymnasium" || inString == "swimming_pool" || inString == "stripclub") return entertainment;
    else if (inString == "marketplace") return shop;
    else if (inString == "post_office" || inString == "post_box" || inString == "ems_station") return postal;
    else if (inString == "hotel" || inString == "motel") return hotel;
    else if (inString == "post_office" || inString == "bus_station" || inString == "ferry_terminal") return transportation;
    else if (inString == "place_of_worship") return religion;
    else return nothing;
}

// display information of the searched point on screen

void draw_info_bar_intersection_name(unsigned intersection_id) {
    intersection_info this_intersection = mymap.intersection_list[intersection_id];

    // intersection name
    set_coordinate_system(GL_SCREEN);
    setcolor(LIGHTGREY);
    setfontsize(14);
    t_bound_box box_int(15, 65, 180, 85);
    drawtext_in(box_int, "Intersection Name: ");

    t_bound_box text_bound_box(15, 90, 395, 110);
    std::string inter_name = this_intersection.name;
    setcolor(DARKGREY);
    setfontsize(12);
    if (!drawtext_in(text_bound_box, inter_name)) {
        setcolor(LIGHTGREY);
        drawtext_in(text_bound_box, "(please check terminal for name)");
    }

    set_coordinate_system(GL_WORLD);
}

// draw information of five found nearby point of interest onto the info bar

void draw_info_bar_five_near_POI(unsigned intersection_id) {
    set_coordinate_system(GL_SCREEN);

    // nearby point of interests
    set_coordinate_system(GL_SCREEN);
    setcolor(LIGHTGREY);
    setfontsize(14);
    t_bound_box box_poi(15, 120, 230, 140);
    drawtext_in(box_poi, "Nearby Point of Interests: ");

    // find 5 closest point of interest
    LatLon position = mymap.intersection_list[intersection_id].position;
    std::vector<value> result;
    mymap.rtree_poi.query(bgi::nearest(point(mymap.longitude_to_cartesian(position.lon()), mymap.latitude_to_cartesian(position.lat())), 5), std::back_inserter(result));
    std::cout << "Nearby Point of Interests: " << std::endl;
    // output found point of interest onto screen  
    for (int i = 0; i < 5; i++) {
        t_bound_box box_poi_name(15, 145 + i * 25, 395, 165 + i * 25);
        std::string poi_name = mymap.poi_list[result[i].second].name;
        LatLon poi_ll = mymap.poi_list[result[i].second].position;
        std::cout << i + 1 << " th: " << poi_name << std::endl;
        setcolor(DARKGREY);
        setfontsize(11);
        if (!drawtext_in(box_poi_name, poi_name)) {
            setcolor(LIGHTGREY);
            setfontsize(10);
            drawtext_in(box_poi_name, "please check terminal for name");
        }
        draw_name_on_map(poi_name, poi_ll);
    }
    set_coordinate_system(GL_WORLD);
}

// draw an arbitrary name on map with the pass in latlon
// mainly used to draw 5 found point of interest name on map

void draw_name_on_map(std::string name, LatLon ll) {
    set_coordinate_system(GL_WORLD);

    t_color origin_color = getcolor();

    // point out POI can be replace with icon later
    setcolor(90, 2, 2);
    draw_star(ll);

    // set text style
    setcolor(74, 10, 33);
    setfontsize(10);
    // draw POI onto graphics
    t_point CentreOfPoint(mymap.longitude_to_cartesian(ll.lon()), mymap.latitude_to_cartesian(ll.lat()) + 0.0000007);
    drawtext(CentreOfPoint, name);

    // reset drawing parameter
    setcolor(origin_color);

    set_coordinate_system(GL_SCREEN);
}

// initialize search bar background

void draw_search_bar_background() {
    set_coordinate_system(GL_SCREEN);

    setcolor(WHITE);
    fillrect(10, 10, 300, 60);

    setcolor(DARKGREY);
    setlinestyle(SOLID);
    setlinewidth(1);
    drawrect(10, 10, 400, 60);
}

// draw magnifying glass on search bar

void draw_search_button() {
    set_coordinate_system(GL_SCREEN);

    // background color
    setcolor(DARKGREY);
    fillrect(300, 10, 350, 60);

    // extract button
    Surface mag_glass;
    mag_glass.setSurface("libstreetmap/resources/rsz_mag_glass_white.png");
    draw_surface(mag_glass, 307, 15);
}

// initialize info bar background

void draw_info_bar_background() {
    set_coordinate_system(GL_SCREEN);

    setcolor(WHITE);
    fillrect(10, 60, 400, 350);
    setcolor(DARKGREY);
    setlinestyle(SOLID);
    setlinewidth(1);
    drawrect(10, 60, 400, 350);
}

// draw red pin for found intersection

void draw_marker(LatLon position) {
    set_coordinate_system(GL_WORLD);
    Surface marker;
    marker.setSurface("libstreetmap/resources/rsz_marker.png");

    float x_position = non_static_xscrn_to_world((non_static_xworld_to_scrn(mymap.longitude_to_cartesian(position.lon())) - 15));
    float y_position = non_static_yscrn_to_world(non_static_yworld_to_scrn(mymap.latitude_to_cartesian(position.lat())) - 30);

    draw_surface(marker, x_position, y_position);
    //draw_surface(marker, longitude_to_cartesian(position.lon()) - 0.0000007, latitude_to_cartesian(position.lat()) + 0.00000155);   
}

void draw_start_pin(LatLon position) {
    set_coordinate_system(GL_WORLD);
    Surface start_pin;
    start_pin.setSurface("libstreetmap/resources/rsz_1start_point_pin(2).png");

    float x_position = non_static_xscrn_to_world((non_static_xworld_to_scrn(mymap.longitude_to_cartesian(position.lon())) - 7.5));
    float y_position = non_static_yscrn_to_world(non_static_yworld_to_scrn(mymap.latitude_to_cartesian(position.lat())) - 7.5);

    draw_surface(start_pin, x_position, y_position);
}

// draw star on map for five nearby POI

void draw_star(LatLon position) {
    set_coordinate_system(GL_WORLD);
    Surface star;
    star.setSurface("libstreetmap/resources/star_red.png");
    draw_surface(star, mymap.longitude_to_cartesian(position.lon()) - 0.0000004, mymap.latitude_to_cartesian(position.lat()) + 0.0000005);
}

// draw text on search bar for user input

void draw_text() {
    set_coordinate_system(GL_SCREEN);
    setcolor(BLACK);
    setfontsize(15);
    t_bound_box text_box(20, 10, 290, 60);
    int count = 0;
    while (!drawtext_in(text_box, input.substr(count))) {
        count++;
    }
}

// zoom to the specified intersection, pin location with map marker, and display info bar with five POI on map

void to_specific_intersection(unsigned id, float x, float y) {
    set_coordinate_system(GL_WORLD);

    // find percentage to zoom to 1% of the world coordinate 
    float per_zoom = percentage_zoom();
    float zoom_factor = 0.01 / per_zoom;
    draw_screen_from_act_on_button_pressed = true;
    search_zoom_in(zoom_factor, x, y, draw_screen);
    draw_screen_from_act_on_button_pressed = false;
    draw_marker(mymap.intersection_list[id].position);
}

void search_bar_display_result_not_found() {
    std::cout << "Result not found" << std::endl;
    std::cout << "Correct format: street name1 & street name2" << std::endl;
    draw_info_bar_background();
    set_coordinate_system(GL_SCREEN);
    setcolor(LIGHTGREY);
    setfontsize(12);
    t_bound_box box1(15, 90, 395, 110);
    drawtext_in(box1, "Result not found");
    t_bound_box box2(15, 110, 395, 140);
    drawtext_in(box2, "Correct format: street name1 & street name2");
    set_coordinate_system(GL_WORLD);
}

void search_bar_display_click_on_search_bar() {
    std::cout << "Please click on input search bar" << std::endl;
    draw_info_bar_background();
    set_coordinate_system(GL_SCREEN);
    setcolor(LIGHTGREY);
    setfontsize(12);
    t_bound_box box1(15, 90, 395, 110);
    drawtext_in(box1, "Please click on input search bar");
    set_coordinate_system(GL_WORLD);
}

// draw direction icon for purpose to show two input bar

void draw_direction_button() {
    set_coordinate_system(GL_SCREEN);

    // background color
    setcolor(DARKGREY);
    fillrect(350, 10, 400, 60);

    // seperation line
    setcolor(WHITE);
    drawline(350, 10, 350, 60);

    // extract button
    Surface direction;
    direction.setSurface("libstreetmap/resources/rsz_direction_icon_no_background.png");
    draw_surface(direction, 355, 15);
    set_coordinate_system(GL_WORLD);
}

// draw direction icon for purpose to show two input bar

void draw_cross_button() {
    set_coordinate_system(GL_SCREEN);

    // extract button
    Surface cross;
    cross.setSurface("libstreetmap/resources/rsz_cross.png");
    draw_surface(cross, 380, 15);
    set_coordinate_system(GL_WORLD);
}

void draw_direction_bar_background() {
    set_coordinate_system(GL_SCREEN);
    setcolor(87, 117, 247);
    fillrect(10, 10, 400, 120);

    // two line
    setcolor(183, 213, 234);
    setlinewidth(2);
    drawline(30, 55, 320, 55);
    drawline(30, 105, 320, 105);
    // switch direction arrow
    Surface switch_arrow;
    switch_arrow.setSurface("libstreetmap/resources/rsz_switch.png");
    draw_surface(switch_arrow, 350, 60);


    set_coordinate_system(GL_WORLD);
}

void draw_first_direction_bar_input_background() {
    set_coordinate_system(GL_SCREEN);

    setcolor(87, 117, 247);
    fillrect(10, 10, 350, 60);
    setcolor(183, 213, 234);
    setlinewidth(2);
    drawline(30, 55, 320, 55);

    set_coordinate_system(GL_WORLD);
}

void draw_second_direction_bar_input_background() {
    set_coordinate_system(GL_SCREEN);

    setcolor(87, 117, 247);
    fillrect(10, 63, 350, 110);
    setcolor(183, 213, 234);
    setlinewidth(2);
    drawline(30, 105, 320, 105);

    set_coordinate_system(GL_WORLD);
}

/*******************************************************************************
 *                          M3 related
 *******************************************************************************/
void draw_path() {
    // calculate percentage zoom
    float per_zoom = percentage_zoom();
    // draw setup
    if (per_zoom >= 0.75) {
        setlinewidth(std::min(12.0f, 1 / per_zoom));
    } else if (0.75 > per_zoom && per_zoom >= 0.4) {
        setlinewidth(std::min(14.0f, 1 / per_zoom));
    } else if (0.4 > per_zoom && per_zoom >= 0.1) {
        setlinewidth(std::min(10.0f, 1 / per_zoom));
    } else if (0.1 > per_zoom && per_zoom >= 0.025) {
        setlinewidth(std::min(10.0f, 1 / per_zoom));
    } else if (0.025 > per_zoom && per_zoom >= 0.007) {
        setlinewidth(std::min(10.0f, 1 / per_zoom));
    } else if (0.007 > per_zoom) {
        setlinewidth(std::min(10.0f, 1 / per_zoom));
    }
    setlinestyle(SOLID, ROUND);
    //setcolor(38,150,232);
    setcolor(67, 167, 240);
    // draw path and arrow
    draw_level_street(display_path_list, 10, true);
    draw_start_pin(mymap.intersection_list[start_point_id].position);
    if (search_poi_flag) {
        draw_marker(mymap.intersection_list[destination_intersection_id].position);
    } else {
        draw_marker(mymap.intersection_list[end_point_id].position);
    }
    
    direction_instruction(display_path_list);
}

// find intersection id for input search name

bool search_direction_setup() {
    // find start intersection id if not from click
    if (!start_point_input_from_click) {
        std::vector<unsigned > start = search_intersection_from_street_names(start_point_input);
        if (start.size() == 0) {
            result_not_found_message();
            return false;
        } else if (start.size() == 1) {
            start_point_id = start[0];
        } else {
            multi_point_message();
            return false;
        }
    }

    // find destination, may be inter or poi name
    if (!end_point_input_from_click) {
        std::vector<unsigned> end = search_intersection_from_street_names(destination_input);
        if (end.size() == 0) {
            search_poi_flag = true;
            return true;
        } else if (end.size() == 1) {
            end_point_id = end[0];
            search_poi_flag = false;
        } else {
            multi_point_message();
            return false;
        }
    }
    std::cout << "search_poi_flag at direction search: " << search_poi_flag << std::endl;
    search_poi_flag = false;
    std::cout << "dirction search set up bot";
    return true;
}

// response to search two intersection

void handle_search_inters(unsigned start_id, unsigned end_id) {
    // find path
    std::vector<unsigned> dir_path = find_path_between_intersections(start_id, end_id, 15);
    display_path_list = dir_path;
    if (dir_path.size() == 0) {
        result_not_found_message();
        return;
    }
    display_path = true;
    path_zoom(dir_path);
    draw_path();    
}

void multi_point_message() {
    set_coordinate_system(GL_SCREEN);
    setcolor(WHITE);
    fillrect(10, 120, 400, 200);
    t_bound_box box(15, 110, 350, 150);
    setcolor(87, 117, 247);
    drawrect(10, 120, 400, 200);
    setcolor(DARKGREY);
    setfontsize(14);
    drawtext_in(box, "More than one possible start/destination points");
    t_bound_box box1(15, 150, 400, 200);
    setcolor(DARKGREY);
    setfontsize(14);
    drawtext_in(box1, "Please click on map to reselect inputs");
    set_coordinate_system(GL_WORLD);
}

void result_not_found_message() {
    set_coordinate_system(GL_SCREEN);
    setcolor(WHITE);
    fillrect(10, 120, 400, 250);
    setcolor(87, 117, 247);
    drawrect(10, 120, 400, 250);
    t_bound_box box(15, 110, 400, 250);
    setcolor(DARKGREY);
    setfontsize(15);
    drawtext_in(box, "Result not found");
    set_coordinate_system(GL_WORLD);
}

void same_message() {
    set_coordinate_system(GL_SCREEN);
    setcolor(WHITE);
    fillrect(10, 120, 400, 250);
    setcolor(87, 117, 247);
    drawrect(10, 120, 400, 250);
    t_bound_box box(15, 110, 400, 250);  
    setcolor(DARKGREY);
    setfontsize(13);
    drawtext_in(box, "Start point and destination are the same!");
    set_coordinate_system(GL_WORLD);
}

void handle_search_poi(unsigned start_id, std::string poi_name) {
    std::vector<unsigned> dir_path = find_path_to_point_of_interest(start_id, poi_name, 15);
    display_path_list = dir_path;
    if (dir_path.size() == 0) {
        result_not_found_message();
        return;
    }
    display_path = true;
    path_zoom(dir_path);
    draw_path();
}

void direction_instruction(std::vector<unsigned> path) {
    // reset old instruction
    instruction.clear();
    // decleration
    unsigned i = 0;
    if (path.size() == 0) return;

    std::string prv_name = mymap.street_segments_list[path[i]].street_name;
    unsigned prv_id = path[i];
    std::string first_ins = "Head straight on " + prv_name + ".";
    instruction.push_back(first_ins);
    std::cout << std::endl << "Navigation Instruction:" << std::endl << first_ins << std::endl;
    while (i < path.size()) {
        std::string street_name = mymap.street_segments_list[path[i]].street_name;
        if (street_name != prv_name) {
            // determine turn angle
            // find intersecting point
            unsigned prv_from = mymap.street_segments_list[prv_id].from;
            unsigned prv_to = mymap.street_segments_list[prv_id].to;
            unsigned cur_from = mymap.street_segments_list[path[i]].from;
            unsigned cur_to = mymap.street_segments_list[path[i]].to;
            unsigned rel_pt;
            unsigned srt_pt;
            unsigned end_pt;
            LatLon rel_ll;
            LatLon srt_ll;
            LatLon end_ll;
            if (prv_from == cur_from) {
                rel_pt = prv_from;
                srt_pt = prv_to;
                end_pt = cur_to;
            } else if (prv_from == cur_to) {
                rel_pt = prv_from;
                srt_pt = prv_to;
                end_pt = cur_from;
            } else if (prv_to == cur_from) {
                rel_pt = prv_to;
                srt_pt = prv_from;
                end_pt = cur_to;
            } else if (prv_to == cur_to) {
                rel_pt = prv_to;
                srt_pt = prv_from;
                end_pt = cur_from;
            }
            rel_ll = mymap.intersection_list[rel_pt].position;
            srt_ll = mymap.intersection_list[srt_pt].position;
            end_ll = mymap.intersection_list[end_pt].position;

            // find slope, y = mx + b, b = y - mx
            float m = (end_ll.lat() - srt_ll.lat()) / (end_ll.lon() - srt_ll.lon());
            float b = end_ll.lat() - m * end_ll.lon();
            // find g(rel_lon) = expected lat
            float g_rel_lon = m * rel_ll.lon() + b;
            // determine turn direction
            if (m >= 0) {
                if (rel_ll.lat() > g_rel_lon) {
                    // right
                    std::string instr = "turn right onto " + street_name;
                    instruction.push_back(instr);
                    std::cout << instr << std::endl;
                } else if (rel_ll.lat() < g_rel_lon) {
                    // left
                    std::string instr = "turn left onto " + street_name;
                    instruction.push_back(instr);
                    std::cout << instr << std::endl;
                } else {
                    // straight
                    std::string instr = "head straight onto " + street_name;
                    instruction.push_back(instr);
                    std::cout << instr << std::endl;
                }
            } else {
                if (rel_ll.lat() > g_rel_lon) {
                    // left
                    std::string instr = "turn left onto " + street_name;
                    instruction.push_back(instr);
                    std::cout << instr << std::endl;
                } else if (
                        rel_ll.lat() < g_rel_lon) {
                    // right
                    std::string instr = "turn right onto " + street_name;
                    instruction.push_back(instr);
                    std::cout << instr << std::endl;
                } else {
                    // straight
                    std::string instr = "head straight onto " + street_name;
                    instruction.push_back(instr);
                    std::cout << instr << std::endl;
                }
            }
        }
        // save new previous street
        prv_name = street_name;
        prv_id = path[i];
        i++;
    }
    std::string end_ins = "Destination: " + destination_input;
    if (search_poi_flag) {
        end_ins = end_ins + " locate at: " + mymap.intersection_list[destination_intersection_id].name;
    }
    instruction.push_back(end_ins);
    //instruction.push_back(end_ins);
    std::cout << end_ins << std::endl << std::endl << std::endl;
    draw_instruction();
}

void draw_instruction() {
    set_coordinate_system(GL_SCREEN);
    // draw background (white base, blue line)
    setcolor(WHITE);
    setlinewidth(1);
    if (instruction.size() > 15) {
        fillrect(10,120,400,800);
        setcolor(87, 117, 247);
        drawrect(10,120,400,800);
    }
    else {
        fillrect(10,120,400,160 + instruction.size() * 40);
        setcolor(87, 117, 247);
        drawrect(10,120,400,160 + instruction.size() * 40);
    }
    
    // display starting message
    setfontsize(15);
    setcolor(BLACK);
    t_bound_box start_box(10, 120, 400, 160);
    drawtext_in(start_box, "Navigation Instruction:");
    
    // draw instruction
    setfontsize(12);
    setcolor(DARKGREY);
    
    for (unsigned i = 0; i < instruction.size() && i < 15 ; i++) {     
        t_bound_box box(10, 160 + i*40, 400, 160 + (i+1)*40);
        drawtext_in(box, instruction[i]);
    }
    set_coordinate_system(GL_WORLD);
}


void path_zoom(std::vector<unsigned> dir_path) {
    // determine path bound
    // initialize top bot left right
    float top = mymap.latitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[0]].from].position.lat());
    float bot = mymap.latitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[0]].from].position.lat());
    float left = mymap.longitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[0]].from].position.lon());
    float right = mymap.longitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[0]].from].position.lon());
    // loop through all from/to intersection of path street segment
    for (unsigned i = 0; i < dir_path.size(); i++) {
        top = std::max(top, mymap.latitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[i]].from].position.lat()));
        top = std::max(top, mymap.latitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[i]].to].position.lat()));
        bot = std::min(bot, mymap.latitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[i]].from].position.lat()));
        bot = std::min(bot, mymap.latitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[i]].to].position.lat()));
        right = std::max(right,mymap.longitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[i]].from].position.lon()));
        right = std::max(right, mymap.longitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[i]].to].position.lon()));
        left = std::min(left, mymap.longitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[i]].from].position.lon()));
        left = std::min(left, mymap.longitude_to_cartesian(mymap.intersection_list[mymap.street_segments_list[dir_path[i]].to].position.lon()));      
    }
    
    // offset to half of the path
    float x_len = right - left;
    float y_len = top - bot;
    float x_cen = (right + left) / 2;
    float y_cen = (top + bot) / 2;
    top = y_cen + y_len * 1.5;
    bot = y_cen - y_len * 1.5;
    right = x_cen + x_len * 1.5;
    left = x_cen - x_len * 1.5;
    // set screen to path bound
    set_screen_bound(top,bot,left,right, y_cen, x_cen, draw_screen);    
}

void determine_direction(unsigned intersection_start, unsigned intersection_end, std::vector<unsigned> &street_segments) {
    unsigned intersection_id_from = intersection_start;
    unsigned intersection_id_to;
    if (street_segments.size() == 0) return;
    if (intersection_start == mymap.street_segments_list[street_segments[0]].to)
        intersection_id_to = mymap.street_segments_list[street_segments[0]].to;
    else if (intersection_start == mymap.street_segments_list[street_segments[0]].from) {
        intersection_id_to = mymap.street_segments_list[street_segments[0]].from;
    } else {
        std::cout << "start point is found" << std::endl;
    }

    for (unsigned i = 0; i < street_segments.size(); ++i) {
        determine_direction_help(intersection_id_from, intersection_id_to, street_segments[i]);
        intersection_id_from = intersection_id_to;
        if (intersection_id_from != mymap.street_segments_list[street_segments[i]].from)
            intersection_id_to = mymap.street_segments_list[street_segments[i]].to;
        else
            intersection_id_to = mymap.street_segments_list[street_segments[i]].from;
    }
    if (intersection_end == intersection_id_to) std::cout << "success in draw direction of map" << std::endl;
    else std::cout << "fail to draw direction of map" << std::endl;
    return;
}

void determine_direction_help(unsigned intersection_id_from, unsigned intersection_id_to, unsigned cross_street_segment) {
    if (mymap.street_segments_list[cross_street_segment].curvePointCount == 0) {
        LatLon start_ll = mymap.intersection_list[intersection_id_from].position;
        LatLon end_ll = mymap.intersection_list[intersection_id_to].position;
        draw_direction(mymap.latlon_to_cartesian(start_ll), mymap.latlon_to_cartesian(end_ll), cross_street_segment, true);
    } else {
        LatLon start_ll = mymap.intersection_list[intersection_id_from].position;
        LatLon end_ll = mymap.intersection_list[intersection_id_to].position;
        LatLon first_curve_point = mymap.street_segments_curvepoint[cross_street_segment].at(0);
        LatLon last_curve_point = mymap.street_segments_curvepoint[cross_street_segment].at(mymap.street_segments_list[cross_street_segment].curvePointCount - 1);
        draw_direction(mymap.latlon_to_cartesian(start_ll), mymap.latlon_to_cartesian(first_curve_point), cross_street_segment, true);
        draw_direction(mymap.latlon_to_cartesian(last_curve_point), mymap.latlon_to_cartesian(end_ll), cross_street_segment, true);
        for (size_t i = 0; i < mymap.street_segments_list[cross_street_segment].curvePointCount - 1; i++) {
            LatLon from = mymap.street_segments_curvepoint[cross_street_segment].at(i);
            LatLon to = mymap.street_segments_curvepoint[cross_street_segment].at(i + 1);
            draw_direction(mymap.latlon_to_cartesian(from), mymap.latlon_to_cartesian(to), cross_street_segment, true);
        }
    }
    return;
}