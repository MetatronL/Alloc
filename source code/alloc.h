#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/*
	In order to print debugging informations:
	#include print_debug_info_malloc
*/


struct block_meta{
	size_t size;	//size of a memory block
	char free;	//used as boolean value to tell if the block is free
	struct block_meta *next;	//next struct in the chain
};

typedef struct block_meta block_m;
typedef struct block_meta* block_p;

#define BLOCK_META_SIZE sizeof(struct block_meta)


void *global_base = NULL;

struct block_meta create_block_meta(size_t _size, block_m* _next, char _free){
	block_m block;
	block.next = _next;
	block.size = _size;
	block.free = _free;
	return block;
}


block_m* pd_request_memory(block_m *last_block_in_chain, size_t size ){
	block_m *old_position;
	old_position = sbrk(0);
	void *new_position = sbrk( size + BLOCK_META_SIZE );
	if( new_position = (void*)(-1) ){ // Allocation error
		#ifdef print_debug_info_malloc
				printf("mem_alloc: Failed to allocate %u bytes of memory\n",size);
		#endif
		return NULL;
	}
	if( last_block_in_chain != NULL )
		last_block_in_chain -> next = old_position;
	*old_position = create_block_meta(size , NULL, 0);
	return old_position;
}


block_m* pd_find_free_block(block_m **last_block,size_t size){
	block_m *current_block = global_base;
	while( 1 == 1 ){
		if( current_block == NULL )
			break;
		if( current_block->free == 0 && current_block->size <= size)
			break;
		*last_block = current_block;
		current_block = current_block->next;
	}
		
	return current_block;
}


void* pd_malloc(size_t size){
	block_m *block;
	
	if( size <= 0 )
		return NULL;
	
	if( global_base == NULL){	/* first run of malloc */
		block = pd_request_memory( NULL, size );
		if( block == NULL)	
			return NULL;
		global_base = block;
	}else{	
		block_m *last_block = global_base;
		block = pd_find_free_block(&last_block,size);
		if( block == NULL){ 	/*failed to find free block -> memory allocation required */
 			block = pd_request_memory( last_block, size );
			if( block == NULL )	
				return NULL;
		}else{	/* found free block */
//To do : split the block if possible
		block->free = 0;		
		}		
	}
	return (block + 1);		//returns the address after the block_meta stuct
	
	
}


block_m* pd_pointer_to_block(void *ref){
//	to do
//check if the block is correct
//wrong -> return NULL;
	return (block_m*)ref - 1;
}


void pd_free(void* source){
	if( source == NULL )
		return ;
	block_m *original_block = pd_pointer_to_block( source );
	if( original_block == NULL ){
		#ifdef print_debug_info_malloc
			printf("mem_alloc: free(void*) received invalid adrress: %p\n",source);
		#endif
		return;
	}	
	if(  original_block->free == 1 ){
		#ifdef print_debug_info_malloc
			printf("mem_alloc: free(void*) received the adrress of a already free block of memory: %p\n",source);
		#endif
		return;
	}	

	original_block->free = 1;
	
}

void* realloc(void *source, size_t size){
	if( source == NULL){
		#ifdef print_debug_info_malloc
			printf("mem_alloc: realloc(void*,size_t) received NULL pointer\n");
		#endif
		return NULL;
	}
	block_m*  block = pd_pointer_to_block(source);
	if( block == NULL){
		#ifdef print_debug_info_malloc
			printf("mem_alloc: realloc(void*,size_t) received invalid adrress: %p\n",source);
		#endif
		return NULL;
	}
	if( block->size >= size)
		return source;
//to be optimized
	void *new_pointer = pd_malloc(size);
	if(  new_pointer == NULL )
		return NULL;
//errno
	memcpy(new_pointer , source , block->size );
	pd_free(source);
	return new_pointer;
 
}

void *pd_calloc(size_t count, size_t size) {
  size_t new_size = count * size; 
// to do: check for overflow.
  void *result = pd_malloc(new_size);
  if( result == NULL)
	return NULL;
  memset(result, 0, size);
  return result;
}

