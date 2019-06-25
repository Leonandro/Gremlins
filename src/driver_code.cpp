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
    std::cout << "Tempo médio para uma alocação no SLpool: " << medias/1000 
                /*<< " / Média por alocação: " << */ << std::endl;

    delete [] test_ptr;
    delete [] test_ptr_dois;
    
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
    std::cout << "Tempo médio  para uma alocação no SO: " << medias/1000 
               /* << " / Média por alocação: " << medias/1000 <<*/ << std::endl;

    std::cout << "\n\e[1;36mRESULTADO:\e[0m O Sistema Operacional precisou de \e[1;35m" << mediaSO/mediaSL 
                    << " \e[0mvezes o tempo que a SLPool levou para realizar a mesma operação.\n";

    if(mediaSO/mediaSL > 1 ) std::cout << "\e[1;32m[PASSED]:\e[0m ";
    else if(mediaSO/mediaSL == 0 ) std::cout << "\e[1;33m[DRAW]:\e[0m ";
    else if(mediaSO/mediaSL) std::cout << "\e[1;31m[FAILED]:\e[0m ";

    delete [] test_ptr;
    delete [] test_ptr_dois;

    std::cout << "\n\n-----------------------------------------------------------------------------------------------------\n";
    std::cout << "Eficience Test - Desallocating\n";
    medias = 0;
    tempos = 0;

    for(int alfa = 0; alfa < 100; alfa++)
    {
        auto size = rand() % 200;
        new for_test_one[size]; 
        test_ptr_dois = new for_test_two[size];
        test_ptr = new for_test_one[size]; 

        for(int i = 0; i < (1000); i++)
        {
            auto size = rand() % 200;
            if(i % 2 == 0) test_ptr_dois = new for_test_two[size];
            else test_ptr = new for_test_one[size]; 

            std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
            if(i % 2 == 0)
            {
                delete [] test_ptr;
            }
            else
            {
                delete [] test_ptr_dois;
            }
            
            std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
            tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
        }

        medias += tempos/1000;

    }

    mediaSL = medias/1000;
    std::cout << "Média de tempo para uma desalocação na SLPool" << mediaSL << std::endl;

    medias = 0;
    tempos = 0;

    for(int alfa = 0; alfa < 100; alfa++)
    {
        auto size = rand() % 200;
        new for_test_one[size]; 
        test_ptr_dois = new for_test_two[size];
        test_ptr = new for_test_one[size]; 

        for(int i = 0; i < (1000); i++)
        {
            auto size = rand() % 200;
            if(i % 2 == 0) test_ptr_dois = new for_test_two[size];
            else test_ptr = new for_test_one[size]; 
            std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now()); 
            if(i % 2 == 0)
            {
                delete [] test_ptr;
            }
            else
            {
                delete [] test_ptr_dois;
            }
            
            std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
            tempos += std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(_end - _start).count(); 
        }

        medias += tempos/1000;

    }

    mediaSO = medias/1000;
    std::cout << "Média de tempo para uma desalocação no SO" << mediaSO << std::endl;

    if(mediaSO/mediaSL >  1) std::cout << "\e[1;32m[PASSED]:\e[0m ";
    else if(mediaSO/mediaSL >= 0.60 ) std::cout << "\e[1;33m[DRAW]:\e[0m ";
    else if(mediaSO/mediaSL < 0.60) std::cout << "\e[1;31m[FAILED]:\e[0m ";

    std::cout << "\n\n-----------------------------------------------------------------------------------------------------\n" << std::endl;
    std::cout << "Ferramenta de Visualização de memória\n\n";
    
    mp::SLPool <16> arena3 (220);

    std::cout <<  "Apos a criação\n" << arena3 << std::endl;
    int * tool = nullptr;
    size_t hue=0;
    std::cout << "\nPor favor digite um tamanho para alocação de um ponteiro de inteiro\n";
    std::cin >> hue;
    tool = new (arena3) int[hue];
    std::cout <<  "\nApos a requisição de alocação\n" << arena3 << "\n\nTecle enter para ver como fica após a desalocação da memória" << std::endl;
    std::cin.get();
    std::cin.ignore();
    delete [] tool;
    std::cout <<  "\nApos deletar a memória apagada\n" << arena3 << std::endl;
}
