#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "alloc.h"


int main(int argc, char *argv[]){
	const int N = 10;
	int *pointer_array[N];
	int index = 0;
	for( index = 0 ; index < N; ++index){
		pointer_array[index] = (int*)pd_malloc( index + 10 );
	}
	
	for( index = 0 ; index < N; ++index){
		pd_free(  pointer_array[index] );
	}

	return 0;

}
