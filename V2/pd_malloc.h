#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAGE_FILE sysconf(_SC_PAGE_SIZE)

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


void* create_big_page_pool(size_t  size){
	return mmap( 0 , (size -1) / PAGE_FILE + 1 , PROT_READ | MAP_SHARED , MAP_PRIVATE | MAP_ANONYMOUS , -1 , 0  );
}


void* create_new_metadata_page(void* next_page){
	char *result = mmap( 0 , PAGE_FILE , PROT_READ | PROT_WRITE , MAP_SHARED | MAP_ANONYMOUS , -1 , 0  );
	if( result != MAP_FAILED )
		((meta*)result)->next = next_page;
	return result;
}



void* page_to_after_struct( char* current_table){
	return (current_table + META_SZ);
}

char* after_metadata( char* current_table , size_t meta_data_size){
	return (current_table + META_SZ + meta_data_size);
}


size_t get_level(size_t align_to){
	switch(align_to){
		case 4:		return 0;
		case 8:		return 1;
		case 16:	return 2;
		case 32:	return 3;
		case 64:	return 4;
		case 128:	return 5;
		case 256:	return 6;
	}
	return -1;
}


unsigned char* table_head[8]  ;

void* small_chunck_malloc(size_t align_to){
	int level = get_level( align_to );


	if( table_head[level] == NULL )			
	//first run for this level 
		if( (table_head[level] = create_new_metadata_page( NULL ) ) == NULL )
		//failed to allocate new page file 			
			return 0;	
		
	size_t total_memory_zones = PAGE_FILE / align_to ; 	//initial total data zones
	
	size_t local_metadata = total_memory_zones / 8;		//count of bytes necesary to memorize the metadata
	local_metadata -= (local_metadata+7)/8;	
	
	size_t offset = align_to - (local_metadata + META_SZ) % align_to;	//offset necesary to keep data aligned
	
	if(  offset == align_to )
		offset = 0;
	
	total_memory_zones -= ( (local_metadata + META_SZ + offset)/align_to );		//recalculate total data zones

	
	unsigned char *current_table = table_head[level];	//pointer used to check pages
	

	if( total_memory_zones == 8 * local_metadata){		//first we try to take a byte from the offset( if offset > 0 )	
		if( offset > 0 ){
			--offset; ++local_metadata; 
	}else							//if last points to a zone, we ignore that zone   --total_memory_zones; 
		--total_memory_zones; 			
	
	printf("small_chunck_malloc: Total_memory_zones= %llu, offset= %llu, total_metadata_space= %llu\n", total_memory_zones, offset , (local_metadata + META_SZ + offset) );

	while( 1 ){	//while we have valid/empty pages -> we search for a free zone
		
		unsigned char *after_struct = page_to_after_struct( current_table);	//pointer to metadata
	

		if( after_struct[local_metadata -1] & 1 ){	//we check the last byte to see if we marked the page as full

			if( (( meta* )current_table)->next == NULL )	//if this is the last page -> create new page
				(( meta* )current_table)->next = create_new_metadata_page( NULL );

			if( MAP_FAILED ==  (( meta* )current_table)->next){	//if we failed to create a new page
				(( meta* )current_table)->next = 0;		//reset next page as NULL
				return 0;					//return
			}
			current_table = (( meta* )current_table)->next ;	//if succesful -> move to this page
		}
		
		unsigned char *c = page_to_after_struct( current_table);	//auxiliar non-constant pointer
		
		
		int i = 0;
		for(   ; i <  local_metadata ; ++i)		//we check all metadata bytes ( we also stop if we found a free byte with the index >= total_memory_zones )
			 if( (c[i] ^  _FREE) != 0  ){		//found free zone(s)
				
				size_t real_poz = get_poz(c[i] , 7);	//we select the first free zone from byte
			
				size_t poz = 1<<(7 - real_poz );	//necesarry to mark the bit as used
				
				real_poz = real_poz + ( i * 8 );	//we calculate the real index of data in page
				
				if( real_poz >= total_memory_zones  )	//index out of  valod range
					break;
				
				c[i] |= poz;				//we marked the zone used

				real_poz = (real_poz) * align_to;	//we calculate the offset(bytes) to the real data zone
				
				return &c[ local_metadata + real_poz + offset ]; //return the pointer to data
			}
		
		
		after_struct[local_metadata -1] |= 1; 			//if we get here -> failed to find a free zone
									//so we mark the page as used
	}
	

	return 0;							//preventive return for unexpected logic error
	
}


void* malloc(size_t size){
	
	if( size > SMALL_CHUNK_THRESHOLD ){
		// add mmap for large data
		if( size >= PAGE_FILE )
			return create_big_page_pool(size);
		return 0;
	}

	size = get_final_size( size );	
	void* result = small_chunck_malloc(size);

	return result;
}

/*
void free(void* data){
	

}
*/

