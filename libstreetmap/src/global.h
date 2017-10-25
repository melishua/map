#ifndef GLOBAL_H
#define GLOBAL_H

#include "Map.h"
#include "NODE.h"
extern Map mymap;

extern std::string user_input_map_name;

extern std::string layer1_map_name;

extern unsigned destination_intersection_id;

extern std::vector<unsigned> find_path_between_intersections_help(const unsigned intersect_id_start, std::vector<unsigned> valid_end_intersections, const double turn_penalty);

#endif /* GLOBAL_H */

