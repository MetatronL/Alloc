#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "pd_malloc.h"
#include <stdio.h>


int main(){
	const int N = 128;
	char *pointer_array[N];
	int index = 0;
	int Z = 256;
	
	/*
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( 4 );
		//printf("\n%llu at %d\n",pointer_array[index],index);		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != 1 )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);
		if( pointer_array[index] == 0 )
			printf("x");
	
		printf("%d\n",*pointer_array[index]);
		
	}
*/
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( Z );
		
		if( index > 0 &&  abs( pointer_array[index] - pointer_array[index-1] ) != Z )
			printf("\n%llu -> %llu at %d\n",pointer_array[index],pointer_array[index-1],index);
		
		
	}
	
	//printf("\n\n");
	printf("END\n");

	return 0;

}
