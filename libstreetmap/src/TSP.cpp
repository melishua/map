#include "TSP.h"

// constructor and destructor
TSP::TSP() {
}

TSP::~TSP() {
    drop_off_list.clear();
    pick_up_list.clear();
    for (unsigned i = 0; i < wei_mat_geo.size(); i++) {
        wei_mat_geo[i].clear();
    }
    wei_mat_geo.clear();
    for (unsigned i = 0; i < geographic_closest_path.size(); i++) {
        geographic_closest_path[i].clear();
    }
    geographic_closest_path.clear();
    traversal_list.clear();
}

TSP::TSP(const std::vector<DeliveryInfo>& deliveries) {
    /****************** set up pickup drop off and traversal list **************/
    // resize vectors
    num_delivery = deliveries.size();
    std::vector<unsigned> empty_list(num_delivery);
    drop_off_list = empty_list;
    pick_up_list = empty_list;
    traversal_list.resize(num_delivery * 2);
//    std::cout << "constructor//initialization: crush here " << std::endl;
    // loop through all points to extract information
    for (unsigned i = 0; i < num_delivery; i++) {
        // store corresponding info
        drop_off_list[i] = deliveries[i].dropOff;
        pick_up_list[i] = deliveries[i].pickUp;
        delivery_pt_info drop_pt(deliveries[i].dropOff, false, false, 99999999, &traversal_list[i]);
        delivery_pt_info pick_pt(deliveries[i].pickUp, true, true, deliveries[i].dropOff, &traversal_list[i + num_delivery]);
        // initialize traversal list to be all pick up then all drop off
        traversal_list[i] = pick_pt;
        traversal_list[i + num_delivery] = drop_pt;
    }
//    std::cout << "constructor//after for loop: crush here " << std::endl;
    /************************ set up for weight matrix **************************/
    wei_mat_geo.resize(mymap.total_number_intersection);
//    std::cout << "num delivery is " << num_delivery << std::endl;
    // i for the row = pick up
    for (unsigned i = 0; i < num_delivery * 2; i++) {
        //        std::cout << "constructor//weight matrix before resize: crush here " << std::endl;
        wei_mat_geo[traversal_list[i].inter_id].resize(mymap.total_number_intersection);
        
        //        std::cout << "constructor//weight matrix after resize: crush here " << std::endl;
        // j for the col = drop off
        for (unsigned j = 0; j < num_delivery * 2; j++) {
            //            std::cout << "constructor//weight matrix inside loop 1: crush here " << std::endl;
//            std::cout << "traversal_list[" << i << "].inter_id is " << traversal_list[i].inter_id << std::endl;
//            std::cout << "traversal_list[" << j << "].inter_id is " << traversal_list[j].inter_id << std::endl;
            //            std::cout << "mymap.intersection_list[pick_up_list[i]].position.lon() is " << mymap.intersection_list[pick_up_list[i]].position.lon() << std::endl;
            //            std::cout << "mymap.intersection_list[drop_off_list[j]].position.lon() is " << mymap.intersection_list[drop_off_list[j]].position.lon() << std::endl;
            //            std::cout << "x is " << mymap.intersection_list[pick_up_list[i]].position.lon() - mymap.intersection_list[drop_off_list[j]].position.lon() << std::endl;

            float x = mymap.intersection_list[traversal_list[i].inter_id].position.lon() - mymap.intersection_list[traversal_list[j].inter_id].position.lon();
            //            std::cout << "constructor//weight matrix inside loop 2: crush here " << std::endl;
            float y = mymap.intersection_list[traversal_list[i].inter_id].position.lat() - mymap.intersection_list[traversal_list[j].inter_id].position.lat();
            //            std::cout << "constructor//weight matrix inside loop 3: crush here " << std::endl;
            //            
            //            std::cout << "distance is " << sqrt(pow(mymap.longitude_to_cartesian(x), 2.0) + pow(mymap.latitude_to_cartesian(y), 2.0)) << std::endl;
            //            std::cout << "size of wei_mat_geo is " << wei_mat_geo.size() << std::endl;
            //                    std::cout << " size of wei_mat_geo[] is " << wei_mat_geo[pick_up_list[i]].size() << std::endl;
            
//            std::cout << "distance calculation is: " << sqrt(pow(mymap.longitude_to_cartesian(x), 2.0) + pow(mymap.latitude_to_cartesian(y), 2.0))  << std::endl;
//            std::cout << "matrix size "<< wei_mat_geo.size() << " matrix inner size: " << wei_mat_geo[traversal_list[i].inter_id].size() << std::endl;
            wei_mat_geo[traversal_list[i].inter_id][traversal_list[j].inter_id] = sqrt(pow(mymap.longitude_to_cartesian(x), 2.0) + pow(mymap.latitude_to_cartesian(y), 2.0));

//            std::cout << "constructor//weight_matrix_set_up//distance is: " << wei_mat_geo[traversal_list[i].inter_id][traversal_list[j].inter_id] << std::endl;
        }
    }
}





//// initialize a list of travel intersection point
//void TSP::list_initialization(const std::vector<DeliveryInfo>& deliveries) {
//    // resize vectors
//    num_delivery = deliveries.size();
//    drop_off_list.resize(num_delivery);
//    pick_up_list.resize(num_delivery);
//    traversal_list.resize(num_delivery * 2);
//    
//    
//    // loop through all points to extract information
//    for (unsigned i = 0; i < num_delivery; i++) {
//        // store corresponding info
//        drop_off_list[i] = deliveries[i].dropOff;
//        pick_up_list[i] = deliveries[i].pickUp;
//        delivery_pt_info drop_pt(deliveries[i].dropOff, false, false, 99999999, &traversal_list[i]);
//        delivery_pt_info pick_pt(deliveries[i].pickUp, true, true, deliveries[i].dropOff, &traversal_list[i + num_delivery]);
//        // initialize traversal list to be all pick up then all drop off
//        traversal_list[i] = pick_pt;
//        traversal_list[i + num_delivery] = drop_pt;
//    }
//}

//// created a weight matrix of geographic distance in Cartesian of points
//void weight_matrix_geo_distance () {
//    // resize matrix vector
//    wei_mat_geo.resize(mymap.total_number_intersection);
//    // i for the row = pick up
//    for (unsigned i = 0; i < num_delivery; i++) {
//        wei_mat_geo[pick_up_list[i]].resize(mymap.total_number_intersection);
//        // j for the col = drop off
//        for (unsigned j = 0; j < num_delivery; j++) {
//            wei_mat_geo[pick_up_list[i]][drop_off_list[j]] = wm_distance_calculation(i,j);     
//        }
//    }   
//}

//// given two parameters to find distance between two intersection
//// row is pick_up_list index, col is drop_off_list index
//// function use specificlly for weight matrix
//float TSP::wm_distance_calculation (unsigned row, unsigned col) {
//   float x = mymap.intersection_list[pick_up_list[row]].position.lon() - mymap.intersection_list[drop_off_list[col]].position.lon();
//   float y = mymap.intersection_list[pick_up_list[row]].position.lat() - mymap.intersection_list[drop_off_list[col]].position.lat();
//   return sqrt(pow(mymap.longitude_to_cartesian(x), 2.0) + pow(mymap.latitude_to_cartesian(y), 2.0));
//}

void TSP::two_opt () {    
    std::cout << "fault here 1" << std::endl;
    update_best_geographic_closest_path(traversal_list);   
    // declareation
    std::vector<float> best_geo_distance;
    std::vector<unsigned> best_pt_list;
    
    // simulated annealing, if accumulated improvement is greater than 10, then assume ideal result is found
    int improve = 0;
    bool swap_flag = true;
    std::cout << "fault here 2" << std::endl;
    while (improve < 10) {
        // current path distance
        best_geo_distance = list_geo_distance_calculation(traversal_list);
        std::cout << "fault here 3" << std::endl;
        // two opt swap position and recalculate for best time
        for (unsigned i = 0; i < num_delivery * 2 - 1; i++) {
            for (unsigned k = i + 1; k < num_delivery && swap_flag; k++) {
                // swap, test next combination if swap is not valid
                std::cout << "fault here 4" << std::endl;
                swap_result swap_output = swap(i,k,traversal_list);
                std::cout << "fault here 5" << std::endl;
                if (!swap_output.swap_valid) swap_flag = false;                
                // if distance exceed best go to check the next combination
                // else new best is found, update information
                std::cout << "fault here 6" << std::endl;
                if (distance_compare(i,k,best_geo_distance, swap_output.new_list)) {
                    // update geographic_closest_path list
                    std::cout << "fault here 7" << std::endl;
                    update_best_geographic_closest_path(swap_output.new_list);
                    std::cout << "fault here 8" << std::endl;
                    // reset parameters
                    improve = 0;
                    traversal_list = swap_output.new_list;
                    std::cout << "fault here 9" << std::endl;
                    best_geo_distance = list_geo_distance_calculation(traversal_list);
                    std::cout << "fault here 10" << std::endl;
                }               
            }
        }
        improve ++;       
    }
}

// swap the result as method indicated by 2-opt
swap_result TSP::swap(unsigned i, unsigned k, const std::vector<delivery_pt_info>& old_list) {
    std::vector<delivery_pt_info> new_list(num_delivery * 2);
    std::vector<delivery_pt_info> zero {delivery_pt_info()};
    // 1. route o to i - 1 remain same
    for (unsigned j = 0; j < i -1; j++) {
        delivery_pt_info this_pt = old_list[j];
        // if pick up point, update corresponding drop off point
        if (this_pt.pick_up) {
            this_pt.corres_ptr -> available = true;             // set drop off point available
        }
        // update pointer to new position
        this_pt.corres_ptr -> corres_ptr = &new_list[j];
        // save point to new list           
        new_list[j] = this_pt;       
    }
    // 2. reverse i to k
    for (unsigned j = i; j < k; j++) {
        delivery_pt_info this_pt = old_list[k-j];
        // check if point is drop off location and wheter if it's available           
        if (!this_pt.pick_up && !this_pt.available) {
            // return if drop off comes before pick up
                swap_result result(false,zero);
                return result;
        }
        // update ptr
        this_pt.corres_ptr -> corres_ptr = &new_list[j];
        // save point to new list
        new_list[j] = this_pt;               
    }
    // 3. route k to end remain the same
    for (unsigned j = k + 1; j < num_delivery * 2; j++) {
        delivery_pt_info this_pt = old_list[j];
        // update ptr
        this_pt.corres_ptr -> corres_ptr = &new_list[j];
        // save point to new list
        new_list[j] = this_pt;
    }
    
    return swap_result(true, new_list); 
}

// given two list: original version and swapped version. compute geographic distance
// to find the closer version by comparing the changed part indicated by i and k
// return true if new path is better than old path, exit otherwise
bool TSP::distance_compare(unsigned i, unsigned k, const std::vector<float>& best_distance, const std::vector<delivery_pt_info>& new_path) {
    float best_sum = 0;
    float new_sum = 0;
    // check swapped part
    std::vector<float> new_distance = list_geo_distance_calculation(new_path);
    for (unsigned j = i; j < k; j++) {
        best_sum += best_distance[j];
        new_sum += new_distance[j];
    }   
    // check connection part
    if (i > 0 ) best_sum += best_distance[i - 1];
    if (k < num_delivery * 2 - 1) new_sum += new_distance[k+1]; 
    // compare for shorter path
    if (new_sum > best_sum) return false;
    else return true;
}

std::vector<float> TSP::list_geo_distance_calculation(const std::vector<delivery_pt_info>& list) {
    std::vector<float> distance;
    std::cout << "num delivery * 2 is " << num_delivery * 2 - 1 << std::endl;
    for (unsigned i = 0; i < num_delivery * 2 - 1; i++) {
        std::cout << "list[i].inter_id is: " << list[i].inter_id << std::endl;
        std::cout << "list[i+1].inter_id is: " << list[i+1].inter_id << std::endl;
        std::cout << "distance is: " << wei_mat_geo[list[i].inter_id][list[i+1].inter_id] << std::endl;
        distance[i] = wei_mat_geo[list[i].inter_id][list[i+1].inter_id];
    }
    return distance;
}

// extract intersection id from the path list and save to geographic_closest_path
void TSP::update_best_geographic_closest_path (const std::vector<delivery_pt_info>& new_best) {
    // parse current list
    unsigned prv_id = new_best[0].inter_id;  
    std::vector<unsigned> path_inter_id(num_delivery*2);
    // save first element
    path_inter_id[0] = prv_id;
    // loop through all element except first one
    unsigned j = 0;
    for (unsigned i = 1; i < new_best.size(); i++) {
        unsigned cur_id = new_best[i].inter_id;
        // if consecutive id are the same this means the same pickup/dropoff location may exist
        if (cur_id != prv_id) {
            path_inter_id[j] = cur_id;
            prv_id = cur_id;
            j++;
        }
    }
    geographic_closest_path.insert(geographic_closest_path.begin(),path_inter_id);
}

std::vector<std::vector<unsigned>> TSP::get_geo_list() {
    return geographic_closest_path;
}