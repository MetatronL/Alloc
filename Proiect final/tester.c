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

int main(){

	const int N = 20;
	char *pointer_array[N];
	int index = 0;
	int Z = PAGE_FILE;

	/*
	for( index = 0 ; index < N; ++index){
		printf("\n\nData zones of %d bytes:\n",Z/(1<<index));
		pointer_array[index] = (int*) calloc(1, Z/(1<<index) );

		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);
	}

	for( index = 0 ; index < N; ++index){
		printf("\n\nFree zones of %d bytes:\n",Z/(1<<index));
		free (pointer_array[index] );
	}

	for( index = 0 ; index < N; ++index){
		printf("\n\nData zones of %d bytes:\n",Z/(1<<index));
		pointer_array[index] = (int*) calloc(1, Z/(1<<index) );

		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);
	}

	for( index = 0 ; index < N; ++index){
		printf("\n\nFree zones of %d bytes:\n",Z/(1<<index));
		free (pointer_array[index] );
	}
	*/
	for( index = 2 ; index < N; ++index){
		printf("\n\nData zones of %d bytes:\n",Z * index );
		//pointer_array[index] = (int*) calloc(1, Z );
		pointer_array[index] =  _malloc(  Z * index );
        if( pointer_array[index] )
            printf("Succesfully allocated at %llu\n",pointer_array[index]);
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z * index )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);
	}

	for( index = 2 ; index < N; ++index){
		printf("\n\nFree zones of %d bytes:\n",Z * index);
		_realloc (pointer_array[index] , Z * index  , Z *  (index + (index %2 ? 1 : -1 ) )  );
	}



	printf("END\n");
	return 0;

	}


