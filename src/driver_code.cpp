#include "../include/SLpool.h"
//#include "../include/mempool_common.h"
#include <iostream>
#include <chrono>
#include <cstdlib>

struct data
{
    int dia;
    int mes;
    int ano;
    int hora;
    float minuto;
    double segundo;
};

struct for_test_one
{
    data um;
    data dois;
    int hue [37];
    char byte;
};

struct for_test_two
{
    data um;
    float kk[15];
    char byte[12];
};

int main ()
{
    mp::SLPool <16> arena (20000000 * sizeof(for_test_one));
    for_test_one * test_ptr = nullptr;
    for_test_two * test_ptr_dois = nullptr;
    double tempos = 0;

    std::cout << "Eficience Test - Alocating time\n\n";

    double medias = 0;

    for(int alfa = 0; alfa < 100; alfa++)
    {
        for(int i = 0; i < (1000); i++)
        {
            std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
            auto size = rand() % 200;
            if(i % 2 == 0)
            {
                test_ptr = new (arena) for_test_one[size];
            }
            else
            {
                test_ptr_dois = new (arena) for_test_two[size];
            }
            
            std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
            tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
        }

        medias += tempos/1000;

    }
    double mediaSL = medias/1000;
    std::cout << "Tempo médio total para 1000 alocações no SLpool: " << medias/1000 
                /*<< " / Média por alocação: " << */ << std::endl;

    test_ptr = nullptr;
    tempos = 0;
    medias = 0;
    for(int alfa = 0; alfa < 100; alfa++)
    {
        for(int i = 0; i < (1000); i++)
        {
            std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
            auto size = rand() % 200;
            if(i % 2 == 0)
            {
                test_ptr = new for_test_one[size];
            }
            else
            {
                test_ptr_dois = new for_test_two[size];

            }
            
            std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
            tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
        }

        medias += tempos/1000;

    }
    double mediaSO = medias/1000;
    std::cout << "Tempo médio total para 1000 alocações no SO: " << medias/1000 
               /* << " / Média por alocação: " << medias/1000 <<*/ << std::endl;

    std::cout << "\n\e[1;36mRESULTADO:\e[0m O Sistema Operacional precisou de \e[1;35m" << mediaSO/mediaSL 
                    << " \e[0mvezes o tempo que a SLPool levou para realizar a mesma operação.\n";

    if(mediaSO/mediaSL > 1) std::cout << "\e[1;32m|PASSED|\e[0m\n";
    else if(mediaSO/mediaSL < 1) std::cout << "\e[1;31m|FAILED|\e[0m\n";
    else std::cout << "\e[1;38m|DRAW|\e[0m\n";

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