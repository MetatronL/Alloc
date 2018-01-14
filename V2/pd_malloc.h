#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAGE_FILE 4096

const size_t SMALL_CHUNK_THRESHOLD = 256;



const unsigned char _FREE = 255;


struct metadata{
	struct metadata* next;
	
};

typedef struct metadata meta;

#define META_SZ sizeof(meta)


/* source http://locklessinc.com/articles/next_pow2/ */
static size_t get_final_size(size_t x){
	//x += META_SZ;
	
	x -= 1;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	
	return x + 1;

}


size_t get_poz(unsigned char x,size_t pw){
	if( x == 255 )
		return -1;
	x = ~x;
	size_t old = pw;
	while( pw ){
		if( x & ( (1 << pw) ) )
			break;
		--pw;
	}
	return old - pw;
}


void* create_new_metadata_page(void* next_page){
	char *result = mmap( 0 , PAGE_FILE , PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS , -1 , 0  );
	((meta*)result)->next = next_page;
}


void* page_to_after_struct( char* current_table){
	return (current_table + META_SZ);
}

char* after_metadata( char* current_table , size_t meta_data_size){
	return (current_table + META_SZ + meta_data_size);
}

unsigned char* table_head[8]  ;
void* small_chunck_malloc(size_t align_to){
	int level ;
	

	RETRY_TO_FIND_LEVEL:
	switch(align_to){
		case 4:		level = 0; break;
		case 8:		level = 1; break;
		case 16:	level = 2; break;
		case 32:	level = 3; break;
		case 64:	level = 4; break;
		case 128:	level = 5; break;
		case 256:	level = 6; break;
		default:	return 0;
	}


	if( table_head[level] == NULL ){
		table_head[level] = create_new_metadata_page( NULL );
	} 
	
	
	
	size_t total_memory_zones = PAGE_FILE / align_to ;
	
	
	size_t local_metadata = total_memory_zones / 8;
	
	
	size_t offset = align_to - (local_metadata + META_SZ) % align_to;
	if(  offset == align_to )
		offset = 0;
	
	total_memory_zones -= ( (local_metadata + META_SZ + offset)/align_to );

	
	unsigned char *current_table = table_head[level];
	

	if( total_memory_zones == 8 * local_metadata)
		--total_memory_zones; 

	

	while( 1 ){
		size_t poz = 0;
		unsigned char *after_struct = page_to_after_struct( current_table);
		if( after_struct[local_metadata -1] & 1 ){
			if( (( meta* )current_table)->next == NULL )
				(( meta* )current_table)->next = create_new_metadata_page( NULL );
			if( MAP_FAILED ==  (( meta* )current_table)->next){
				(( meta* )current_table)->next = 0;
			return 0;
		}
			current_table = (( meta* )current_table)->next ;
			 
			continue;
		}
		unsigned char *c = page_to_after_struct( current_table);
		
		int i = 0;
		for(   ; i <  local_metadata ; ++i){
			 if( (c[i] ^  _FREE) != 0  ){
				size_t real_poz = get_poz(c[i] , 7);
			
				
				
				
				poz = 1<<(7 - real_poz );
				
				real_poz = real_poz + ( i * 8 );
				

				
				if( real_poz >= total_memory_zones  )
					break;
				
				c[i] |= poz;

				real_poz = (real_poz) * align_to;
				
				
				c += local_metadata;
				c += real_poz;
				c += offset;
				 
				
 				return c;
			}
		
		}
		after_struct[local_metadata -1] |= 1;

				
		if( (( meta* )current_table)->next == NULL )
				(( meta* )current_table)->next = create_new_metadata_page( NULL );
		if( MAP_FAILED ==  (( meta* )current_table)->next){
			(( meta* )current_table)->next = 0;
			return 0;
		}
		current_table = (( meta* )current_table)->next ;
	}
	

	
	
	

	
}


void* malloc(size_t size){
	size = get_final_size( size );
		

	if( size > SMALL_CHUNK_THRESHOLD ){
		// add mmap for large data
		return 0;
	}
	
	void* result = small_chunck_malloc(size);


}

