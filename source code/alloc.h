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

#define BLOCK_META_SIZE sizeof(struct block_meta)


void *global_base = NULL;

struct block_meta create_block_meta(size_t _size, struct block_meta* _next,char _free){
	struct block_meta block;
	block.next = _next;
	block.size = _size;
	block.free = _free;
	return block;
}


struct block_meta* pd_request_memory(struct block_meta *last_block_in_chain, size_t size ){
	struct block_meta *old_position;
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


struct block_meta* pd_find_free_block(struct block_meta **last_block,size_t size){
	struct block_meta *current_block = global_base;
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
	struct block_meta *block;
	
	if( size <= 0 )
		return NULL;
	
	if( global_base == NULL){	/* first run of malloc */
		block = pd_request_memory( NULL, size );
		if( block == NULL)	
			return NULL;
		global_base = block;
	}else{	
		struct block_meta *last_block = global_base;
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
