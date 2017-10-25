#ifndef M4_MODIFIED_GREEDY_H
#define M4_MODIFIED_GREEDY_H

#include "NODE.h"
#include "m4.h"
#include "m3.h"
#include "global.h"
#include "m1.h"
#include <limits>
#include <chrono>
#define TIME_LIMIT 30

class drop_off_point {
public:
    bool has_picked_up;
    unsigned dropOff;
    drop_off_point();
};
std::vector<unsigned> greedy_search(unsigned start_id, std::vector<unsigned> pick_up_list, std::vector<drop_off_point> drop_off_list, float turn_penalty);
bool erase_pick_up_point(std::vector<unsigned> &pick_up_list, std::vector<drop_off_point> &drop_off_list, unsigned checked_element);
void erase_drop_off_point(std::vector<drop_off_point> &drop_off_list, unsigned checked_element);
std::vector<unsigned> construct_path(std::vector<unsigned> &pick_up_list, std::vector<drop_off_point> &drop_off_list);
void debug_print_pick_up_list(std::vector<unsigned> pick_up_list);
void debug_print_drop_off_list(std::vector<drop_off_point> drop_off_list);
std::vector<unsigned> traveling_courier_billy_modified(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots, const float turn_penalty);

#endif /* M4_MODIFIED_GREEDY_H */

