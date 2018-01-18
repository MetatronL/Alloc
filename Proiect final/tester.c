#include "pd_malloc.h"
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <math.h>
//#include <stdlib.h>

long long abs( long long x){
	if( x < 0 )
		return -x;
	return x;
}


#define DEBUG_MALLOC 1


int main(){

	const int N = 1032;
	char *pointer_array[N];
	int index = 0;
	int Z = PAGE_FILE;

	for( index = 0 ; index < N; ++index){
		//printf("\n\nData zones of %d bytes:\n",4);
		pointer_array[index] = _malloc( 4 );

		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != 4 )
			printf("\n%llu -> %llu at %d\n",pointer_array[index-1],pointer_array[index],index);
	}

	for( index = 0 ; index < N; ++index){
		//printf("\n\nFree zones of %d bytes:\n",4);
		_free (pointer_array[index] , 4 );
	}

	_malloc_check_remaining_data();

	for( index = 0 ; index < N; ++index){
		//printf("\n\nData zones of %d bytes:\n",512);
		//pointer_array[index] = (int*) _calloc(2, 64 );
        pointer_array[index] = (int*) _malloc( 256 );
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != 256 )
			printf("\n%llu -> %llu at %d\n",pointer_array[index-1],pointer_array[index],index);
	}

	for( index = 0 ; index < N; ++index){
		//printf("\n\nFree zones of %d bytes:\n",512);
		_free (pointer_array[index] , 256 );
	}

    _malloc_check_remaining_data();

    printf("CALLOC\n");
	for( index = 0 ; index < N; ++index){
		//printf("\n\nData zones of %d bytes:\n",512);
		//pointer_array[index] = (int*) _calloc(2, 64 );
        pointer_array[index] = (int*) _calloc( 128 , 2 );
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != 256 )
			printf("\n%llu -> %llu at %d\n",pointer_array[index-1],pointer_array[index],index);
	}

	for( index = 0 ; index < N; ++index){
		//printf("\n\nFree zones of %d bytes:\n",512);
		_free (pointer_array[index] , 256 );
	}

   _malloc_check_remaining_data();

	for( index = 0 ; index < N; ++index){
		printf("\n\nData zones of %d bytes:\n",Z * index );
		//pointer_array[index] = (int*) calloc(1, Z );
		pointer_array[index] =  _malloc(  Z * index );
        if( pointer_array[index] )
            printf("Succesfully allocated at %llu\n",pointer_array[index]);
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z * index )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);
	}

	for( index = 0 ; index < N; ++index){
		printf("\n\nFree zones of %d bytes:\n",Z * index );
		_realloc (pointer_array[index] , Z * index   , 4 + Z *index  );
	}

    _malloc_check_remaining_data();


	printf("\nSUCCES\nEND\n");
	return 0;

	}


