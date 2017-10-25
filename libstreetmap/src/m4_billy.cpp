/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "m4_modified_greedy.h"

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots, const float turn_penalty) {
    return traveling_courier_billy_modified(deliveries, depots, turn_penalty);
}

