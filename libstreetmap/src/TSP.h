/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TSPalgorithm.h
 * Author: panzhi2
 *
 * Created on April 1, 2017, 12:19 AM
 */


#ifndef TSP_H
#define TSP_H

#include "Map.h"
#include "m4.h"
#include "global.h"
#include <math.h>
#include <vector>

struct delivery_pt_info {
    unsigned inter_id;  // intersection point id
    bool pick_up;       // true if pickup, false if drop_off
    bool available;     // true if pt is available to go to
    unsigned drop_off_id;   // if the intersection point is a pickup location, this tells where the drop-off is, if not it's initialize to be 99999999
    delivery_pt_info *corres_ptr;
    // constructor
    delivery_pt_info() {
        inter_id = 0;
        pick_up = true;
        available = false;
        drop_off_id = 99999999;
        corres_ptr = NULL;
    }
    delivery_pt_info(unsigned id, bool pp, bool avail, unsigned do_id, delivery_pt_info *cp) {
        inter_id = id;
        pick_up = pp;
        available = avail;
        drop_off_id = do_id;
        corres_ptr = cp;
    }
};

struct swap_result {
    bool swap_valid;
    std::vector<delivery_pt_info> new_list;
    swap_result() {
        swap_valid = false;
        new_list.resize(0);
    }
    swap_result(bool sv, std::vector<delivery_pt_info> nl) {
        swap_valid = sv;
        new_list = nl;
    }
};



class TSP {
private:
    std::vector<unsigned> drop_off_list;
    std::vector<unsigned> pick_up_list;
    std::vector<std::vector<float>> wei_mat_geo;
    std::vector<std::vector<unsigned>> geographic_closest_path;
    std::vector<delivery_pt_info> traversal_list;
    unsigned num_delivery = 0;
public:
    // constructor
    TSP();
    TSP(const std::vector<DeliveryInfo>& deliveries);
    ~TSP();
    // asscible function
    //    void set_drop_off(std::vector<unsigned> list);
    //    void set_pick_up(std::vector<unsigned> list);
    //    void set_weight(std::vector<std::vector<float>> weight);
    //    void set_geo_best_path(std::vector<std::vector<unsigned>> list);
    //    void set_traversal(std::vector<delivery_pt_info> list);
    void two_opt();
    float wm_distance_calculation(unsigned row, unsigned col);
    swap_result swap(unsigned i, unsigned k, const std::vector<delivery_pt_info>& old_list);
    bool distance_compare(unsigned i, unsigned k, const std::vector<float>& best_distance, const std::vector<delivery_pt_info>& new_path);
    std::vector<float> list_geo_distance_calculation(const std::vector<delivery_pt_info>& list);
    void update_best_geographic_closest_path(const std::vector<delivery_pt_info>& new_best);
    std::vector<std::vector<unsigned>> get_geo_list();   
};
#endif /* TSP_H */

