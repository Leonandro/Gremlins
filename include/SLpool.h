#include <new>
#include <exception>
#include "mempool_common.h"
#include <cmath>
#include <string>
#include <iostream>
#include <ostream>

namespace mp {

/* Classe filha da Storagepool que implementa uma pool personalizada para gerenciamento de memória */
template < size_t BLK_SIZE = 16 >
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

    public:
        static constexpr size_t BLK_SZ = sizeof( mp::SLPool<BLK_SIZE>::Block ); //!< The block size in bytes.
        static constexpr size_t TAG_SZ = sizeof( Tag ); //!< The Tag size in bytes (each reserved area has a tag).
        static constexpr size_t HEADER_SZ = sizeof( mp::SLPool<BLK_SIZE>::Header ); //!< The header size in bytes.
    
    private:
        size_t m_n_blocks ; // ! < Number of blocks in the list .
        Block * m_pool; // ! < Head of list .
        Block & m_sentinel; // ! < End of the list .

    public:
        SLPool( size_t bytes ) : m_n_blocks { (size_t)ceil( ( (double)bytes + sizeof(Header) ) / (double)BLK_SIZE)}, m_pool {new Block[m_n_blocks+1]}
										, m_sentinel{m_pool[m_n_blocks]}, StoragePool()
        {
           m_sentinel.m_length = 0;
		   m_sentinel.m_next = & m_pool[0];
		   m_pool[0].m_next = nullptr;
		   m_pool[0].m_length = m_n_blocks;
		}

        /* destrutor default */
		virtual ~SLPool ()
        {
			delete [] this->m_pool;
        }

        /* Aloca o número de bytes solicitados em n blocos e retorna um ponteiro para essa área na pool */
		void * Allocate ( size_t bytes)
        {
			Block * it = m_sentinel.m_next;
            Block * prev = m_sentinel.m_next; 
            size_t required_blocks = (size_t)ceil(( ((double)bytes + sizeof(Header)) / (double)BLK_SIZE));
            bool is_first_free = true;

            /* Percorre todas as áreas livres */
            while( it != nullptr )
	        {
                /*Checa se a área atual tem um tamanho suficiente */
                if (it->m_length >= required_blocks)
                {
                    /* Trivial */
                    if(it->m_length == required_blocks)
                    {
                        if(is_first_free) m_sentinel.m_next = it->m_next;
                        else prev->m_next = it->m_next;
                        return reinterpret_cast<void*> (reinterpret_cast<Header *> (it) + (1U));
                    }

                    /* Caso a área seja maior que o necessário, divide em duas áreas e ajusta o ponteiro anterior 
                        para a próxima área livre */
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

        /* Aloca no modelo best-fit */
        void * BAllocate(size_t bytes)
        {
            Block * it = m_sentinel.m_next;
            Block * prev = m_sentinel.m_next; 
            size_t required_blocks = (size_t)ceil(( ((double)bytes + sizeof(Header)) / (double)BLK_SIZE));
            bool is_first_free = true;
            Block *best_fit = it;
            Block *prev_best = m_sentinel.m_next;;

            /* Percorre todas as áreas livres */
            while( it != nullptr )
	        {
                /*Checa se a área atual tem um tamanho suficiente */
                if (it->m_length >= required_blocks)
                {
                    /* Trivial */
                    if(it->m_length == required_blocks)
                    {
                        if(is_first_free) m_sentinel.m_next = it->m_next;
                        else prev->m_next = it->m_next;
                        return reinterpret_cast<void*> (reinterpret_cast<Header *> (it) + (1U));
                    }

                    else
                    {
                        if(it->m_length > required_blocks && it->m_length < best_fit->m_length)
                        {
                            best_fit = it;   
                        }
                    }
                }
                it = it->m_next;
                if(!is_first_free) prev = prev->m_next;
                is_first_free = false;
            
            }

            prev = prev_best;
            while(prev_best != nullptr)
            {
                if(prev_best == best_fit) break;
                prev = best_fit;
                best_fit = best_fit->m_next;
            }

            prev_best = prev;

            if(best_fit->m_length < required_blocks)throw std::bad_alloc();

            else
            {
                Block * new_area =  best_fit + required_blocks;
                if(best_fit ==  m_sentinel.m_next)m_sentinel.m_next = new_area;
                else prev_best->m_next = new_area;
                new_area->m_length = prev_best->m_length - required_blocks;
                best_fit->m_length = required_blocks;
                return reinterpret_cast<void*> ( reinterpret_cast <Header *> (best_fit) + (1U));
            }
            
        }

        /* Desaloca da pool toda a memória associada a área apontada pelo ponteiro target_block */
        void Free ( void * target_block)
        {
            target_block = reinterpret_cast<Block *> (reinterpret_cast <Header *> (target_block) - (1U));
            Block * pos_target = m_sentinel.m_next;		//!< Ponteiro para a primeira área livre após o bloco alvo
            Block * cur_target = (Block *)target_block;	//!< Ponteiro para o bloco alvo
            Block * prev_target = &m_pool[m_n_blocks];	//!< Ponteiro para a primeira área livre antes do bloco alvo

            /* Percorre as áreas livres para ajustar os ponteiros prev e pos */
            while(pos_target != nullptr)
            {
                if(pos_target > cur_target)	break;		

                prev_target = pos_target;
                pos_target = pos_target->m_next;
            }

            /* Caso ele (bloco alvo) esteja exatamente entre duas áreas livres */
            if (( prev_target + prev_target->m_length == cur_target ) && (cur_target + cur_target->m_length == pos_target))
            {
                prev_target->m_length += pos_target->m_length + cur_target->m_length;
                prev_target->m_next = pos_target->m_next;
            } // Em caso de vazamento de memória, fazer um trash colector para ficar apagando as memória deletadas

            /* Caso ele esteja com uma área livre a esquerda, mas não a direita */
            else if ( ( prev_target + prev_target->m_length == cur_target ) && ( cur_target + cur_target->m_length != pos_target ) )
            {
                prev_target->m_length += cur_target->m_length;
                prev_target->m_next = pos_target;
            }

            /* Caso ele esteja com uma área livre a direita, mas não a esquerda */
            else if ( ( prev_target + prev_target->m_length != cur_target ) && ( cur_target + cur_target->m_length == pos_target ) )
            {
                cur_target->m_length += pos_target->m_length;
                prev_target->m_next = cur_target;
                cur_target->m_next = pos_target->m_next;
            }

            /* Caso ele não esteja entre nenhuma área livre */
            else
            {
                prev_target->m_next = cur_target;
                cur_target->m_next = pos_target;
            }
            
        }

        void print ()
        {
            auto it = m_sentinel.m_next;
            int cont = 0;
            while(it != nullptr)
            {
                for(size_t size = 0; size < it->m_length; size++)
                {
                    std::cout << "\033[1;32m[FREE] \033[0m ";
                    cont++;
                }
                it = it->m_next;
            }

            for(size_t a = 0; a < (m_n_blocks - cont); a++)
            {
                std::cout << "\033[1;31m[USED] \033[0m";
            }

            std::cout << std::endl;
        }

        /* Sobrecarga do operador << */
        friend std::ostream & operator << (std::ostream & os, SLPool & mypool)
        {
            auto it = mypool.m_sentinel.m_next;
            int cont = 0;
            bool isfirst = true;
            while(it != nullptr)
            {
                for(size_t size = 0; size < it->m_length; size++)
                {
                    if(it != nullptr) 
                    {    
                        if(!isfirst)os << "\033[1;32m[FREE] \033[0m ";
                        else os << "\033[1;36m[SEN] \033[0m ";

                        isfirst = false;
                    }
                    cont++;
                }
                it = it->m_next;
            }

            for(size_t a = 0; a < ((mypool.m_n_blocks) - cont); a++)
            {
                os << "\033[1;31m[USED] \033[0m";
            }

            os << std::endl;

            os << "| Block size = " << mypool.BLK_SZ << "| Number of blocks = " << mypool.m_n_blocks - 1 
               << "| Pool size = " <<  (mypool.m_n_blocks - 1) * mypool.BLK_SZ;

            return os;

        }
};

}