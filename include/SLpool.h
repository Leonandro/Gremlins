#include <new>
#include "mempool_common.h"
#include <cmath>

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
        unsigned int m_n_blocks ; // ! < Number of blocks in the list .
        Block * m_pool; // ! < Head of list .
        Block &m_sentinel; // ! < End of the list .

    public:
        explicit SLPool( size_t bytes ) : m_n_blocks {(size_t)ceil( (bytes) / BLK_SIZE)}, m_pool {new Block[m_n_blocks+1]}
										, m_sentinel{m_pool[m_n_blocks+1]}, StoragePool()
        {
           m_sentinel.m_length = 0;
		   m_sentinel.m_next = &m_pool[0];
		   m_pool[0].m_next = nullptr;
		   m_pool[0].m_length = m_n_blocks;
		}
		virtual ~SLPool ()
        {
			delete [] m_pool;
        }
		virtual void * Allocate ( size_t bytes)
        {
			size_t q_blocks = (bytes+ sizeof(Header))/BLK_SIZE;
			Block *it = &m_sentinel;
			int cont = 0;
			while (it->m_next->m_length <= q_blocks and it->m_next!=nullptr) 
				it = it->m_next;
			if (it->m_next == nullptr) throw std::bad_alloc();
			Block* to_return = it->m_next;
			size_t nextlength = to_return->m_length - q_blocks;
			it->m_next = it->m_next + nextlength;
			to_return->m_length = q_blocks;

			return to_return;	
        }
        virtual void Free ( void * )
        {
            int a = 0;
        }

        virtual void Release( void * )
        {
            int a = 0;
        }
};
