#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>


const size_t __MAX_SIZE = 1<<29;
const size_t __MMAP_THRESHOLD = 16; 
const size_t __MAX_LIST_SIZE = 512;
const size_t __PAGE_SIZE = 4096;


#define __MINIMUM_MEMORY_OPTIMIZATION_THRESHOLD  128
#define __i i

const size_t __HAS_FREE_MEM = 1 << 31;

const int IS_MAPPED = 4;

//void *metadata_holder;

void list_holder[10];



void *initial_sbrk = NULL;	//debug usage

struct memory_block{
	size_t size;
	struct memory_block* prev;	
	struct memory_block* next;	 
	struct memory_block* next_mmap_block;	
	 
};

typedef struct memory_block  s_block;	//static
typedef struct memory_block* p_block;	//pointer


struct mmap_metadata{
	size_t count;
	//s_block* slot[_MMAP_THRESHOLD];

};

struct list_metadata{
	void* next;
	void* addr;
}
typedef struct list_metadata m_list;


typedef struct mmap_metadata mmap_info;
mmap_info mmap_m; 



#define BLOCK_SIZE (sizeof(struct memory_block))
//sizeof(struct memory_block)

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define align8(x) (((((x)-1)>>3)<<3)+8)

p_block g_base = NULL, g_last = NULL ;	//Head and end of list 


size_t sbrk_initialised = 0;


void* _int_malloc(size_t s){
// 	WRITE ME 
//	 PLEASE
	
}


size_t get_log2(size_t s){
	size_t result = 0;
	while(s){
		++result;
		s = s >> 1;
	}
	--result;
	return result;
}


void* try_insert_list( void* x ,size_t nth_bucket ){
	size_t s = 1 << nth_bucket;
	size_t elem = __PAGE_SIZE / s;
// to do 
	
}

void* _list_malloc(size_t s){
	size_t nth_bucket = get_log2(s);	
	if(s > __MAX_LIST_SIZE )
		return 0;
	if( sbrk_initialised == 0){
		sbrk_initialised = 1;
		s_block* p = sbrk( 8 * sizeof(m_list) );
		int i ;		
		for( i = 2; i <= 9; ++i){
			list_holder[i] = &p[i-2];
		}
		memset(p,0,8 * sizeof(m_list));
	} 
	
	if(	list_holder[nth_bucket].addr == 0 ){
		list_holder[nth_bucket].addr = (mmap (0, __PAGE_SIZE , PROT_READ | PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE , -1, 0 ));
		size_t *x =  list_holder[nth_bucket].addr;
 		x = x | __HAS_FREE_MEM ;
	}

	size_t *x =  list_holder[nth_bucket].addr;
	void* result = 0;
	while( x ){
		if( (*x & __HAS_FREE_MEM) )
			continue;
		result = try_insert_list( x , nth_bucket ); 
		if( result )
			break;
			
		x = x->next;
	}
	
	return result;
	
	
	
}


void* _mmap_malloc(size_t size){
	if( mmap_m.count >= _MMAP_THRESHOLD )
		return 0;
	
	s_block *result;
	result = (s_block *) (mmap (0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE , -1, 0 ));
	result->size = (size | IS_MAPPED) ;
	
	

}



size_t upper_power_of_two(size_t v)
{
    if( v < 8 )
	return 8;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}



void* malloc(const size_t s){
	
	void* result ;
	
	if( s >= _MAX_SIZE )
		return 0;

	s = s + BLOCK_SIZE;
	size_t _size = upper_power_of_two( f_size );
	
			
	
	if( f_size > _MAX_LIST_SIZE){
		if( mmap_m.count < _MMAP_THRESHOLD ){
			result =  _mmap_malloc( f_size );
			if( result == 0 ){
				//try second approach
			}
			return result;
		}
		return 0;
		//TO DO
		//optimize										// here
	}

	if( s < (f_size >> 1) && s > _MINIMUM_MEMORY_OPTIMIZATION_THRESHOLD  ){
		//too much unused memory
		//try to use other sources
	} 
	

	result = _list_malloc(f_size);
	if( result == 0 )
		result = _int_malloc(f_size);

	return result;
}	

void* get_block(void *addr){
	return (s_block*)(addr) - 1;
}

void free(void* addr){
	s_block *mblck = get_block(addr);
	if( mblck->size & IS_MAPPED ){
		munmap( mblck , mblck->size ); 
	}
}


