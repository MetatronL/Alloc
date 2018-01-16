#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "pd_malloc.h"
#include <stdio.h>
#include <math.h>


int main(){

	const int N = 1;
	char *pointer_array[N];
	int index = 0;
	int Z = 256;

	printf("\n\nData zones of 256 bytes:\n");
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( 256 );
		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);	
	}

	printf("\n\nData zones of 128 bytes:\n");
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( 128 );
		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);	
	}
	
	printf("\n\nData zones of 64 bytes:\n");
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( 64 );
		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);	
	}

	printf("\n\nData zones of 32 bytes:\n");
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( 32 );
		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);	
	}

	
	printf("\n\nData zones of 16 bytes:\n");
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( 16 );
		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);	
	}

	printf("\n\nData zones of 8 bytes:\n");
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( 8 );
		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);	
	}

	printf("\n\nData zones of 4 bytes:\n");
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( 4 );
		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);	
	}

	

	
	printf("END\n");

	return 0;

}


