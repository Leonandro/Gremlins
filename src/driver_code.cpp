#include "../include/SLpool.h"
//#include "../include/mempool_common.h"
#include <iostream>
#include <chrono>
#include <cstdlib>

int main ()
{
    mp::SLPool <16> arena (160000);
    int * test_ptr = nullptr;
    double tempos = 0;

    std::cout << "Eficience Test - Alocating/Desallocating time\n\n";

    double medias;

    for(int alfa = 0; alfa < 1000; alfa++)
    {
        for(int i = 0; i < (1000); i++)
        {
            std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
            auto size = rand() % 200;
            test_ptr = new (arena) int[size];
            delete [] test_ptr;
            std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
            tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
        }

        medias += tempos/1000;

    }

    std::cout << "Tempo médio total para 1000 alocações no SLpool: " << medias/1000 
                /*<< " / Média por alocação: " << */ << std::endl;

    test_ptr = nullptr;
    tempos = 0;
    medias = 0;
    for(int alfa = 0; alfa < 1000; alfa++)
    {
        for(int i = 0; i < (1000); i++)
        {
            std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
            auto size = rand() % 200;
            test_ptr = new int[size];
            std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
            tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
        }

        medias += tempos/1000;

    }

    std::cout << "Tempo médio total para 1000 alocações no SO: " << medias/1000 
               /* << " / Média por alocação: " << medias/1000 <<*/ << std::endl;

    std::cout << "----------------------------------------------------------------------------------------------\n\n";
    std::cout << "Integrity Test\n\n";
    mp::SLPool <16> arena2(50000);
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

    mp::SLPool <16> arena3 (220);
    int * transao = nullptr;
    transao = new (arena3) int[4];
    arena3.print();
    delete [] transao;
    arena3.print();
}