#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>

#define PAGE_FILE 4096

const size_t SMALL_CHUNK_THRESHOLD = 512;



const unsigned char NOT_FIRST_FREE = 127;
const unsigned char FIRST_FREE = 255;
const unsigned char IS_FULL = 128;

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


size_t get_poz(size_t x,size_t pw){
	if( x == 0 )
		return -1;
	--pw;
	size_t old = pw;
	while( pw ){
		if( x ^ ( ~(1 << pw) ) )
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

void* small_chunck_malloc(size_t align_to){
	int level ;
	static char* table_head[8] = {0,0,0,0,0,0,0,0,0} ;

	RETRY_TO_FIND_LEVEL:
	switch(align_to){
		case 4:		level = 0; break;
		case 8:		level = 1; break;
		case 16:	level = 2; break;
		case 32:	level = 3; break;
		case 64:	level = 4; break;
		case 128:	level = 5; break;
		default:	return 0;
	}


	if( table_head[level] == NULL ){
		table_head[level] = create_new_metadata_page( NULL );
	} 
	//printf("b");
	
	
	size_t total_memory_zones = PAGE_FILE / align_to ;
	
	
	size_t local_metadata = total_memory_zones / 8;  
	
	
	size_t offset = align_to - (local_metadata + META_SZ) % align_to;
	if(  offset == align_to )
		offset = 0;
	

	total_memory_zones -= ( (local_metadata + META_SZ) / align_to );
	unsigned char *current_table = table_head[level];
	printf("align_to = %llu ",align_to);
	//printf("c");
	while( current_table != NULL){
		size_t poz = 0;
		unsigned char *after_struct = page_to_after_struct( current_table);
		if( *after_struct & IS_FULL ){
			current_table = (( meta* )current_table)->next ;
			continue;
		}
		unsigned char *c = after_struct;
		//printf(" %llu ",c);
		int i = 0;
		for(   ; i <  local_metadata ; ++i){
			if( i == 0 && (c[i] ^ FIRST_FREE) != 0  )
				 poz = c[i] ^ FIRST_FREE;
			else if( (c[i] ^ NOT_FIRST_FREE) != 0  )
				poz = c[i] ^ NOT_FIRST_FREE;
				
			
			if( poz != 0 ){
				
				poz = get_poz(poz , 7);
				if( poz == -1)
					continue;
				printf("poz =  %u\n",poz);
				printf("local_metadata =  %u\n",local_metadata);
				printf("offset =  %u\n",offset);
				//printf("d1");
				if(i != 0)
					 c[i] |= (1 << ( 7 - poz ) );
				else
					 c[i] |= (1 << ( 6 - poz ) );
				if( i > 0 )
					poz = poz + ( i * 8 - 1);
				if(poz != 0 )
					poz = (poz-1) * align_to;
				printf("poz = %u\n",poz);
				
				//printf("d2");
				c += local_metadata;
				c += poz;
				c += offset;
				 
				printf(" %llu\n",c - current_table);
				printf(" %llu , %llu",c,current_table);
 				return c;
			}
		
		}
		*after_struct |= IS_FULL; 
		//printf("e");
		current_table = (( meta* )current_table)->next ;
	}
	
	printf("f");
	

	
}


void* malloc(size_t size){
	size = get_final_size( size );
		

	if( size > SMALL_CHUNK_THRESHOLD ){
		// add mmap for large data
		return 0;
	}
	printf("a");
	void* result = small_chunck_malloc(size);


}
