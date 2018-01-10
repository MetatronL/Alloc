#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "pd_malloc.h"
#include <stdio.h>


int main(){
	const int N = 10;
	int *pointer_array[N];
	int index = 0;
	
	
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*) malloc( index + 2048 );
		
		// free(pointer_array[index]);
	}
	
	 
	

	return 0;

}
