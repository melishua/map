#include "NODE.h"
#include "global.h"

NODE::NODE() {
    weight = 0;
    intersection_id = 0;
    reaching_edge = 0;
    visited_flag = false;
}

NODE::NODE(double _weight,unsigned _intersection_id, int _reaching_edge) {
    weight = _weight;
    intersection_id = _intersection_id;
    reaching_edge = _reaching_edge;
    visited_flag = false;
}
//copy constructor does not need to change

NODE::NODE(const NODE& src) {
    this->intersection_id = src.intersection_id;
    this->reaching_edge = src.reaching_edge;
    this->visited_flag = src.visited_flag;
    this->weight = src.weight;
}

//the destructor does not need to change

NODE::~NODE() {
}

bool NODE::operator>(const NODE& src) {
    if ((this->weight) > src.weight)
        return true;
    else
        return false;
}

void NODE::operator =(const NODE& src){
    this->intersection_id = src.intersection_id;
    this->reaching_edge = src.reaching_edge;
    this->weight = src.weight;
    this->visited_flag = src.visited_flag;
    return;
}

std::ostream& operator<<(std::ostream& out, NODE& src) {
    out << "the intersection is " << src.intersection_id << "        the weight  is " << src.weight << std::endl;
    return out;
}

bool IsTurned (unsigned street_seg_id1, unsigned street_seg_id2){
    if(mymap.street_segments_list[street_seg_id1].street_index != mymap.street_segments_list[street_seg_id2].street_index)
        return true;
    else
        return false;
}

std::vector<unsigned> TrackBack(std::vector<NODE> &store_created_node,NODE currNODE){
    std::list<int> path;
    
    int prevEdge = currNODE.reaching_edge;
    while(prevEdge != -1){
        path.push_front(prevEdge);
        currNODE = get_NODE_by_street_segments_id(store_created_node,prevEdge,currNODE);
        prevEdge = currNODE.reaching_edge;
    }
    std::vector<unsigned> my_path{std::make_move_iterator(std::begin(path)),std::make_move_iterator(std::end(path))};
    return my_path;
}

NODE get_NODE_by_street_segments_id(std::vector<NODE> &store_created_node, int street_seg_id, NODE currNODE){
    if(mymap.street_segments_list[street_seg_id].from == currNODE.intersection_id){
        return store_created_node[mymap.street_segments_list[street_seg_id].to];
    }
                
        return store_created_node[mymap.street_segments_list[street_seg_id].from];
}
