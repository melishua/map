#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <vector>
#include "NODE.h"
using namespace std;

class MinHeap
{
private:
    vector<NODE> _vector;
    
    void BubbleDown(int index);
    void BubbleUp(int index);
    void Heapify();

public:
    MinHeap(NODE* array, int length);
    MinHeap(const vector<NODE>& vector);
    MinHeap();
    
    int size();
    void Insert(NODE newValue);
    NODE GetMin();
    void DeleteMin();
};


#endif /* MIN_HEAP_H */
