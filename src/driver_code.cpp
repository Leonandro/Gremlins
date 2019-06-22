#include "../include/SLpool.h"
//#include "../include/mempool_common.h"
#include <iostream>
#include <chrono>

int main ()
{
    SLPool <16> arena (220);
    int * ptr = (int *)arena.Allocate(4);
    *ptr = 4;
    std::cout << "PTR = " << *ptr /* <<  " - " << *hue <<*/ << std::endl;
    arena.print();
}