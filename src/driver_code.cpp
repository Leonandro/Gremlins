#include "../include/SLpool.h"
//#include "../include/mempool_common.h"
#include <iostream>
#include <chrono>

int main ()
{
    SLPool <16> arena (5000000);
    int * test_ptr = nullptr;
    double tempos = 0;
    for(int i = 0; i < (200); i++)
    {
        std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
        test_ptr = (int *)arena.Allocate(4);
        std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
        tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
    }
    std::cout << "Tempo total para 200 alocações no SLpool: " << tempos 
              << " Média por alocação: " << tempos/200 << std::endl;

    test_ptr = nullptr;
    tempos = 0;
    for(int i = 0; i < (200); i++)
    {
        std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
        test_ptr = new int[1];
        std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
        tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
    }
    std::cout << "Tempo total para 200 alocações no Sistema Operacional: " << tempos 
              << " Média por alocação: " << tempos/200 << std::endl;
}