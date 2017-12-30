#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void *initial_sbrk = NULL;	//debug usage

struct memory_block{
	size_t size;
	struct memory_block* prev;	
	struct memory_block* next;	 
	int free;	// boolean value
	 
};

typedef struct memory_block  s_block;	//static
typedef struct memory_block* p_block;	//pointer


#define BLOCK_SIZE (sizeof(struct memory_block))
//sizeof(struct memory_block)

#define align4(x) (((((x)-1)>>2)<<2)+4)

p_block g_base = NULL, g_last = NULL ;	//Head and end of list 


void* extend_stack( size_t s){
	p_block b = sbrk(0);
	if( initial_sbrk == NULL)
		initial_sbrk = b;
	if( sbrk(BLOCK_SIZE + s) == (void*)-1 ){
		//failed to increase stack
		return NULL;
	}

	b->size = s;
	b->next = NULL;	
	b->prev = NULL;

	if( g_last ){
		g_last->next = b;
		b->prev = g_last;	
		g_last = b;
	}else
		g_base = g_last = b;

	return b;
}

void* find_block(size_t s){		//need optimization  ^_^  *_*
	p_block b = g_base;
	while( b && !(b->free && b->size <= s) )
		b = b->next;
	return b;
}


void* try_split_block( p_block b, size_t s){
	if( !b)
		return NULL;
	if( b->size < s + BLOCK_SIZE + sizeof(int) )
		return NULL;

	p_block new = &b[1] + s;
	new->size = b->size - (BLOCK_SIZE + s);
	new->next = b->next ;
	new->free = 1;
	new->prev = b;

	b->size = s;
	b->next = new;

	return new;
}
 
void* malloc(size_t s){
	s  = align4(s);	//align to int
	p_block b = g_base;
	
	if( g_base ){
		b = find_block(s);
		if(	b )	
			try_split_block(b,s);
	}

	if( !b )
		b = extend_stack(s);

	if( b == NULL)
		return NULL;
	
	b->free = 0; 
	
	return &b[1] ;
}	


void* calloc(size_t c, size_t s){
	size_t size = align4( c * s);
	size_t *new = malloc(size);
	if( new ){
		size_t i , n = size/4;
		for( i = 0 ; i < n ; ++i)
			new[i] = 0;
	}
	return new;
}
int valid(p_block source){
	
	if( !source )
		return 0;
	if( source->next &&  source->next->prev == source)
		return 1;
	if( source->prev &&  source->prev->next == source )
		return 1;
	if( sbrk(0) == ((void*)source + source->size + BLOCK_SIZE) )
		return 1;
	return 0; 
}

void* get_block(void* source){
	return source - BLOCK_SIZE;
}

p_block fusion(p_block b){
	

	if( b->next && b->next->free ){	
		if( b->next == g_last )
			g_last = b;
		
		b->size += (BLOCK_SIZE + b->next->size);
		b->next = b->next->next;
		if( b->next)
			b->next->prev = b;
		
	}
	
	
	return b;	
}



void free(void* source){
	p_block b = get_block(source);

	if( !valid(b) ){
		 	
		return;	
	}
	b->free = 1;
	if( b->prev && b->prev->free)
		b = fusion( b->prev );
	if( b->next && b->next->free )
		b = fusion( b );
	
	b->free = 1;

	if( b == g_last ){
		if( b->prev )
			b->prev->next = NULL;
		g_last = b->prev;
		brk(b);
	}
		
	
	
	if( g_last == NULL){
		g_base = g_last = NULL;
	} 
	
}



