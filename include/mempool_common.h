#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <cstdlib>
#include <iostream>

typedef unsigned long int size_t; 

class StoragePool {

    public:
        virtual ~StoragePool () {/*Empty */}
        virtual void * Allocate ( size_t ) = 0;
        virtual void Free ( void * ) = 0;
        
};

struct Tag {
    StoragePool * pool; 
};

void * operator new ( size_t bytes , StoragePool & p ) {
    Tag * const tag = reinterpret_cast < Tag * > ( p . Allocate ( bytes + sizeof( Tag )) );
    tag -> pool = & p ;
    // skip sizeof tag to get the raw data - block .
    return ( reinterpret_cast < void * >( tag + 1U ) );
}

void * operator new[] ( size_t bytes, StoragePool & p ){
    size_t new_size = bytes + sizeof(Tag);
    //std::cout << "New size is: " << new_size << std::endl;
    Tag * const m_tag = reinterpret_cast<Tag *const>( p.Allocate(new_size) );
    m_tag->pool = &p;
    return reinterpret_cast<void *>( m_tag + 1 );
}

void * operator new ( size_t bytes ) { // Regular new
    Tag * const tag = reinterpret_cast < Tag * >( std::malloc( bytes + sizeof( Tag )) );
    tag -> pool = nullptr ;
    // skip sizeof tag to get the raw data - block .
    return ( reinterpret_cast < void * >( tag + 1U ) );
}

void operator delete ( void * arg ) noexcept {
// We need to subtract 1 U ( in fact , pointer arithmetics ) because arg
// points to the raw data ( second block of information ).
// The pool id ( tag ) is located ‘ sizeof ( Tag ) ’ bytes before .
    Tag * const tag = reinterpret_cast < Tag * >( arg ) - 1U ;
    if( nullptr != tag->pool ) // Memory block belongs to a particular GM .
        tag->pool->Free( tag );
    else
        std::free( tag ); // Memory block belongs to the operational system .
}

void operator delete[] ( void * arg ) noexcept 
{
    // We need to subtract 1U ( in fact , pointer arithmetics ) because arg
    // points to the raw data ( second block of information ).
    // The pool id ( tag ) is located ‘ sizeof ( Tag ) ’ bytes before.
    Tag * const tag = reinterpret_cast<Tag*>( arg ) - 1U;
    if ( nullptr != tag->pool ) // Memory block belongs to a particular GM.
    {
        tag->pool->Free( tag );
    }
    else // Memory block belongs to the operational system.
    {
        std::free ( tag );
    }
}

#endif
