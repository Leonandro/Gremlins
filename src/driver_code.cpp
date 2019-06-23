#include "../include/SLpool.h"
//#include "../include/mempool_common.h"
#include <iostream>
#include <chrono>

int main ()
{
    SLPool <16> arena (5000000);
    int * test_ptr = nullptr;
    double tempos = 0;

    std::cout << "Eficience Test - Alocating time\n\n";
    for(int i = 0; i < (200); i++)
    {
        std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
        test_ptr = (int *)arena.Allocate(4);
        std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
        tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
    }
    std::cout << "Tempo total para 200 alocações no SLpool: " << tempos 
              << " / Média por alocação: " << tempos/200 << std::endl;

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
              << " / Média por alocação: " << tempos/200 << std::endl;

    std::cout << "----------------------------------------------------------------------------------------------\n\n";
    std::cout << "Integrity Test\n\n";
    SLPool <16> arena2(50000);
    int * integrity_ptr;
    integrity_ptr = (int*) arena2.Allocate( 10 * sizeof(int) );
    for(int a = 0; a < 10; a++)
    {
        integrity_ptr[a] = a+1;
    }

    for(int a = 0; a < 10; a++)
    {
        std::cout << integrity_ptr[a] << " ";
    }

    std::cout << std::endl;

    std::cout << "----------------------------------------------------------------------------------------------\n\n";

    SLPool <16> arena3 (220);
    int * transao = nullptr;
    transao = (int *)arena3.Allocate(32);
    arena3.print();
    arena3.Free(transao);
    arena3.print();
}