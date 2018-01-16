#define _GNU_SOURCE 1
#include <unistd.h>
#include <sys/mman.h>

#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include <stdio.h>

#include <limits.h>


#define PAGE_FILE sysconf(_SC_PAGE_SIZE)

const size_t SMALL_CHUNK_THRESHOLD = 512;
const size_t SMALL_CHUNK_THRESHOLD_LEVEL = 7;

//const int

const unsigned char _FREE = 255;

#define MAX_ALLOWED_SIZE UINT_MAX


struct metadata{
	struct metadata* next;
	struct metadata* prev;
	unsigned long long total_used_data;
	unsigned char size;
	unsigned char count;
	unsigned char total_pagini;
	//unsigned char to_be_deleted;
};

typedef struct metadata meta;

#define META_SZ sizeof(meta)


unsigned long long min( unsigned long long a, unsigned long long b){
	if ( a < b )
		return a;
	return b;
}


unsigned long long _max( unsigned long long a, unsigned long long b){
	if ( a > b )
		return a;
	return b;
}

/* source http://locklessinc.com/articles/next_pow2/ */
size_t get_final_size(size_t x){
	//x += META_SZ;

	x -= 1;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);

	return _max(x + 1,4);

}


size_t to_page_multiple(size_t size ){
	size = (size -1)/PAGE_FILE + 1;
	size = size *  PAGE_FILE;
	return size;
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
	return mmap( 0 , to_page_multiple(size) , PROT_READ | MAP_SHARED , MAP_PRIVATE | MAP_ANONYMOUS , -1 , 0  );
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


char get_level(size_t align_to){
	switch(align_to){
		case 4:		return 0;
		case 8:		return 1;
		case 16:	return 2;
		case 32:	return 3;
		case 64:	return 4;
		case 128:	return 5;
		case 256:	return 6;
		case 512:	return 7;
		case 1024:	return 8;
	}
	return -1;
}


unsigned char* table_head[8]  ;


void calculate_metadata(size_t *total_memory_zones, size_t *local_metadata, size_t *offset, size_t align_to ){
	*total_memory_zones = PAGE_FILE / align_to ; 	//initial total data zones

	*local_metadata = *(total_memory_zones+7) / 8;		//count of bytes necesary to memorize the metadata
	*local_metadata -= (*local_metadata+7)/8;

	*offset = align_to - (*local_metadata + META_SZ) % align_to;	//offset necesary to keep data aligned

	if(  *offset == align_to )
		*offset = 0;

	*total_memory_zones -= ( (*local_metadata + META_SZ + *offset)/align_to );		//recalculate total data zones
}



void* small_chunck_malloc(size_t align_to){
	char level = get_level( align_to );

	if( level == -1)
		return 0;
	if( table_head[level] == NULL ){
	//first run for this level
		if( (table_head[level] = create_new_metadata_page( NULL ) ) == NULL )
		//failed to allocate new page file
			return 0;
		((meta*)table_head[level])->total_pagini = 1;
		((meta*)table_head[level])->size =  level;
	}

	size_t total_memory_zones, local_metadata, offset ;
	calculate_metadata(&total_memory_zones, &local_metadata, &offset, align_to );



	unsigned char *current_table = table_head[level];	//pointer used to check pages


	if( total_memory_zones == 8 * local_metadata){		//first we try to take a byte from the offset( if offset > 0 )
		if( offset > 0 ){
			--offset; ++local_metadata;
		}else{						//if last points to a zone, we ignore that zone   --total_memory_zones;
			--total_memory_zones;
		}
	}
	printf("small_chunck_malloc: Total_memory_zones= %llu, offset= %llu, total_metadata_space= %llu\n", total_memory_zones, offset , (local_metadata + META_SZ + offset) );

	while( 1 ){	//while we have valid/empty pages -> we search for a free zone

		unsigned char *after_struct = page_to_after_struct( current_table);	//pointer to metadata


		if( after_struct[local_metadata -1] & 1 ){	//we check the last byte to see if we marked the page as full

			if( (( meta* )current_table)->next == NULL ){	//if this is the last page -> create new page
				(( meta* )current_table)->next = create_new_metadata_page( NULL );

				if( MAP_FAILED ==  (( meta* )current_table)->next){	//if we failed to create a new page
					(( meta* )current_table)->next = 0;		//reset next page as NULL
					return 0;					//return
				}
				printf("Created new page of level %d\n",level);
				++(((meta*)table_head[level])->total_pagini) ;
				(( meta* )current_table)->next->prev = current_table;
				((meta*)current_table)->size =  level;
			}
			current_table = (( meta* )current_table)->next ;	//if succesful -> move to this page
			if(  current_table == NULL )
				break;
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

				++(((meta*)current_table)->count) ;
				++(((meta*)table_head[level])->total_used_data) ;



				return &c[ local_metadata + real_poz + offset ]; //return the pointer to data
			}


		after_struct[local_metadata -1] |= 1; 			//if we get here -> failed to find a free zone
									//so we mark the page as used
	}


	return 0;							//preventive return for unexpected logic error

}


void* _malloc(size_t size){

	if( size <= SMALL_CHUNK_THRESHOLD )
		return small_chunck_malloc( get_final_size( size ) );

	return create_big_page_pool(size);


}

void free_small_chunk(meta* data){
	unsigned long long start = data;
	start /= PAGE_FILE;
	start *= PAGE_FILE;	//merg la inceputul paginii

	unsigned long long position = (unsigned long long)(data) - start;
	unsigned char *origin = (unsigned char*)start;

	unsigned char level = 	((meta*)origin)->size;

	size_t total_memory_zones, local_metadata, offset ;	//calculez metadata pentru pagina
	calculate_metadata( &total_memory_zones, &local_metadata, &offset, 1 << (level+2) );

	position -= ( local_metadata +  offset + META_SZ );
	position /= 1 << (level+2);	//calculez pozitia relativa pe pagina

	printf("Succesfully freed a zone of %d bytes, index = %d\n", 1 << (level +2), position );

	unsigned char byte = 1 << (7 - ( position % 8 ) );	//vom marca zona folosita ca fiind libera
	position = (position)/8 ;	//calculez byte-ul care stocheaza informatia despre zona

	(((unsigned char*)origin + META_SZ))[position] =  ~(byte | ~((unsigned char*)(origin + META_SZ))[position] );	//marcam bitul free ( 0 )
	byte = 1; byte = ~byte;
	(((unsigned char*)origin + META_SZ))[local_metadata -1] &= byte;	//marcam pagina ca nu e complet ocupata
	--(((meta*)origin)->count) ;
	if( table_head[level] == NULL )
		return;


	--(((meta*)table_head[level])->total_used_data) ;
	if( ((meta*)table_head[level])->total_pagini > 1 && (((meta*)origin)->count) == 0
							&& ( ( total_memory_zones * (((meta*)table_head[level])->total_pagini - 1 ) - ((meta*)table_head[level])->total_used_data ) > (1<<(7-level)) )){
		printf("Attempt to free a page of level %d\n",level);
		if( origin == table_head[level] ){

			((meta*)origin)->next->total_used_data = ((meta*)origin)->total_used_data;
			((meta*)origin)->next->total_pagini = ((meta*)origin)->total_pagini - 1;
			((meta*)origin)->next->prev = 0;
			table_head[level] = ((meta*)origin)->next;
		}else{
			if( ((meta*)origin)->next  )
				((meta*)origin)->next->prev = ((meta*)origin)->prev;
			if( ((meta*)origin)->prev )
				((meta*)origin)->prev->next = ((meta*)origin)->next;
		}
		--((meta*)table_head[level])->total_pagini;
		munmap( origin , PAGE_FILE );
	}

}

void _free(void* data, size_t size  ){
	if( (unsigned long long)data % PAGE_FILE == 0  && size >  SMALL_CHUNK_THRESHOLD){	//big data mmap
		munmap( data , to_page_multiple(size) );
		return;
	}

	if( (unsigned long long)data % PAGE_FILE == 0   )  	//big data mmap
		return;

	free_small_chunk(data );
}




void* calloc(size_t count, size_t size){
	if( count == 0 || size == 0)
		return 0;
	if( count > (MAX_ALLOWED_SIZE / size) )	//too much memory
		return 0;
	char *data = malloc( count * size );
	if( count * size <  SMALL_CHUNK_THRESHOLD)
		memset( data , 0 , count * size );
	return data;

}



void* realloc(void *data, size_t new_size , size_t old_size){
	//return data -> if reallocation is not necessary
	//return 0 -> if size(s) is/are too big
	//return MAP_FAILED if failed to remap + failed to map a new zone or failed malloc

	new_size = min( new_size , 4) ;
	if( new_size > MAX_ALLOWED_SIZE )
		return 0;
	if( old_size > MAX_ALLOWED_SIZE )
		return 0;

	old_size = get_final_size(old_size);
	new_size = get_final_size(new_size);
	char *new_data = 0;

	if( old_size > SMALL_CHUNK_THRESHOLD && (unsigned long long)data % PAGE_FILE == 0 && new_size > SMALL_CHUNK_THRESHOLD){
		new_size = to_page_multiple( new_size );
		old_size = to_page_multiple( old_size );
		new_data = mremap(data , old_size , new_size , MREMAP_MAYMOVE | 0  );
		if( new_data != MAP_FAILED )
			return new_data;
		new_data = malloc( new_size );
		if( new_data == MAP_FAILED )
			return -1;
		size_t s = min( new_size , old_size );
		memcpy( new_data , data , s );
		_free( data , old_size );
		return new_data;
	}


	if( old_size > SMALL_CHUNK_THRESHOLD )
		old_size = to_page_multiple( old_size );
	if( new_size > SMALL_CHUNK_THRESHOLD )
		new_size = to_page_multiple( new_size );

	if( new_size == old_size )
		return data;

	new_data = malloc(new_size);
	if( new_data == 0 )
		return -1;
	size_t s = min( new_size , old_size );
	memcpy( new_data , data , s );

	_free( data , old_size );

	return new_data;


}


