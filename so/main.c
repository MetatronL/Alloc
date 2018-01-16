
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <math.h>
//#include <stdlib.h>
#include "pd_malloc.h"
long long abs( long long x){
	if( x < 0 )
		return -x;
	return x;
}

int main(){

	const int N = 20;
	char *pointer_array[N];
	int index = 0;
	int Z = 256;

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
	for( index = 0 ; index < N; ++index){
		//printf("\n\nData zones of %d bytes:\n",Z );
		//pointer_array[index] = (int*) calloc(1, Z );
		pointer_array[index] =  _malloc(  Z );

		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);
	}

	for( index = 0 ; index < N; ++index){
		//printf("\n\nFree zones of %d bytes:\n",Z);
		_free (pointer_array[index] , Z );
	}



	printf("END\n");
	return 0;

	}

