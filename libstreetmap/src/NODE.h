#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <ostream>
#include <vector>
#include <list>
#include "global.h"


class NODE {
public:
    double weight;
    unsigned intersection_id;
    int reaching_edge;
    bool visited_flag; // true means visited, false means not visited yet
    
    NODE();
    NODE(double _weight, unsigned intersection_ID,int _reaching_edge);
    NODE(const NODE& src);
    ~NODE();
    bool operator>(const NODE& src);
    void operator=(const NODE& src);
    friend std::ostream& operator<<(std::ostream& out, NODE& src);
};

std::ostream& operator<< (std::ostream& out, NODE& src);
bool IsTurned (unsigned street_seg_id1, unsigned street_seg_id2);
std::vector<unsigned> TrackBack(std::vector<NODE> &store_created_node,NODE currNODE);
NODE get_NODE_by_street_segments_id(std::vector<NODE> &store_created_node,int street_seg_id, NODE currNODE);


#endif /* NODE_H */

