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
        explicit SLPool( size_t bytes )
        {
            int result = ceil(bytes/BLK_SIZE);
            m_pool = new Block [result + 1];
            m_sentinel = m_pool[0];
        }
        ~ SLPool ()
        {
            int a = 0;
        }
        void * Allocate ( size_t bytes)
        {
            int a = 0;
        }
        void Free ( void * )
        {
            int a = 0;
        }

        void Release( void * )
        {
            int a = 0;
        }
};