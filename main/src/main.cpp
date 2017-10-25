#include <iostream>
#include <string>
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "global.h"
#include "graphics.h"
std::string user_input_map_name;
std::string map_path;

int main(int argc, char** argv) {
//
//    /**************************** m4 debug ************************************/
//    map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";
//    bool load_success = load_map(map_path);
//
//    std::vector<DeliveryInfo> deliveries;
    //    std::vector<unsigned> depots;
    //    std::vector<unsigned> result_path;
    //    float turn_penalty;
    //
    //    deliveries = {DeliveryInfo(73593, 104453), DeliveryInfo(1981, 47060), DeliveryInfo(58204, 31403), DeliveryInfo(102129, 63975), DeliveryInfo(41153, 63608)};
    //    depots = {2521, 5153, 41138};
    //    turn_penalty = 15;
    //    result_path = traveling_courier(deliveries, depots, turn_penalty);

    /**************************************************************************/


    while (1) {
        if (argc == 1) {
            std::cout << "Please enter map name (e.g. toronto_canada): ";
            std::cin >> user_input_map_name;
            std::cout << "loading..." << std::endl;
            map_path = "/cad2/ece297s/public/maps/" + user_input_map_name + ".streets.bin";
        } else if (argc == 2) {
            //Get the map from the command line
            map_path = argv[1];
            user_input_map_name = map_path;
        } else {
            //Invalid arguments
            std::cerr << "Usage: " << argv[0] << " [map_file_path]\n";
            std::cerr << "  If no map_file_path is provided a default map is loaded.\n";
            return 1;
        }

        //Load the map and related data structures
        bool load_success = load_map(map_path);
        if (!load_success) {
            std::cerr << "Failed to load map '" << map_path << "'\n";
            std::cout << "map name is not valid" << std::endl; //return 2;
        } else break;
    }

    std::cout << "Successfully loaded map '" << map_path << "'\n";
    //You can now do something with the map    
    draw_map();


    //Clean-up the map related data structures
    std::cout << "Closing map\n";
    close_map();

    return 0;
}
