#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAGE_FILE 4096

const size_t MALL_CHUNK_THRESHOLD = 512;

struct metadata{
	struct metadata* next;
	
}

typedef struct metadata meta;
#define META_SZ sizeof(meta);


/* source http://locklessinc.com/articles/next_pow2/ */
static size_t get_final_size(size_t x){
	x += META_SZ;
	
	x -= 1;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	
	return x + 1;

}


void* small_chunck_malloc(size_t size){
	int level ;
	static char* table_head[8];

	RETRY_TO_FIND_LEVEL:
	switch(size){
		case 4:		level = 0; break;
		case 8:		level = 1; break;
		case 16:	level = 2; break;
		case 32:	level = 3; break;
		case 64:	level = 4; break;
		case 128:	level = 5; break;
		default:	return 0;
	}

/*	IF metadata heap uninitialised 
	-> CREATE new metadata heap
*/
	if( table_head[level] == NULL ){
		table_head[level] = create_new_metadata_page( NULL );
	} 
	
	
	char *data = table_head[level];
	size_t align_to = 1 << (level+2) ;
	size_t total_memory_zones ;
	switch(level){
	case 0:		//uint32_t
		//no alignment problem as default aligned to uint32_t
		total_memory_zones -= META_SZ;
		// //total_memory_zones = (META_SZ % align_to == 0 ) ? (PAGE_FILE - META_SZ) : (  );
		total_memory_zones /=  align_to;
		size_t local_metadata = total_memory_zones / 8; //bits -> bytes
		size_t 
		
	}

	
}


void* malloc(size_t size){
	size = get_final_size( size );
	
	if( size > SMALL_CHUNK_THRESHOLD ){
		// add mmap for large data
		return 0;
	}

	void* result = small_chunck_malloc(size)


}
