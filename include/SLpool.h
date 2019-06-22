#include <new>
#include <exception>
#include "mempool_common.h"
#include <cmath>
#include <string>
#include <iostream>

template < size_t BLK_SIZE =16 >
class SLPool : public StoragePool {
    public :
        struct Header {
            size_t m_length ;
            Header () : m_length (0u) { /* Empty */ };
            };
        struct Block : public Header {
            union {
                Block * m_next ; // Pointer to next block OR ..
                char m_raw [ BLK_SIZE - sizeof( Header ) ]; // Client ’s raw area
            };
            Block () : Header () , m_next ( nullptr ) { /* Empty */ };
        };

    private:
        size_t m_n_blocks ; // ! < Number of blocks in the list .
        Block * m_pool; // ! < Head of list .
        Block & m_sentinel; // ! < End of the list .

    public:
        explicit SLPool( size_t bytes ) : m_n_blocks { (size_t)ceil((double)bytes / (double)BLK_SIZE)}, m_pool {new Block[m_n_blocks+1]}
										, m_sentinel{m_pool[m_n_blocks]}, StoragePool()
        {
           m_sentinel.m_length = 0;
		   m_sentinel.m_next = & m_pool[0];
		   m_pool[0].m_next = nullptr;
		   m_pool[0].m_length = m_n_blocks;
		}

		virtual ~SLPool ()
        {
			delete [] m_pool;
        }

		void * Allocate ( size_t bytes)
        {
			Block * it = m_sentinel.m_next;
            Block * prev = m_sentinel.m_next; 
            size_t required_blocks = (size_t)ceil(((double)bytes / (double)BLK_SIZE));
            bool is_first_free = true;
            while( it != nullptr )
	        {
                if (it->m_length >= required_blocks)
                {
                    if(it->m_length == required_blocks)
                    {
                        if(is_first_free) m_sentinel.m_next = it->m_next;
                        else prev->m_next = it->m_next;
                        return reinterpret_cast<void*> (reinterpret_cast<Header *> (it) + (1U));
                    }

                    else
                    {
                        Block * new_area = it + required_blocks;
                        if(is_first_free)m_sentinel.m_next = new_area;
                        else prev->m_next = new_area;
                        new_area->m_length = it->m_length - required_blocks;
                        it->m_length = required_blocks;
                        return reinterpret_cast<void*> ( reinterpret_cast <Header *> (it) + (1U));
                    }
                    
                }

                it = it->m_next;
                if(!is_first_free) prev = prev->m_next;
                is_first_free = false;
                
            }

            throw std::bad_alloc();
        }

        void Free ( void * )
        {
            int a = 0;
        }

        void print ()
        {
            auto it = m_sentinel.m_next;
            int cont = 0;
            while(it != nullptr)
            {
                for(int size = 0; size < it->m_length; size++)
                {
                    std::cout << "\033[1;32m[FREE] \033[0m ";
                    cont++;
                }
                it = it->m_next;
            }

            for(int a = 0; a < (m_n_blocks - cont); a++)
            {
                std::cout << "\033[1;31m[USED] \033[0m";
            }

            std::cout << std::endl;
        }
};
