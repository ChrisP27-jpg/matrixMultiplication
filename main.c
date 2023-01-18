//Author: Jayson Boubin -- Fall 2022
#include "matmul.h"

int* mat1 = NULL;
int* mat2 = NULL;
int* matRes = NULL;
int* matSol = NULL;
pthread_t *t = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int total=0;


void * thread_work(void * arg) {
	int * x = (int *)arg;
	int i = *x;
	x++;
	int d=*x;
	
	for(;i<=d;i++){
		if (total < M*K && i<M){
	    	for(int j=0; j<K; j++){
	    		matRes[(i*K) + j] = 0;
	    		for(int k=0; k <N; k++){
	    			matRes[(i*K)+j] += mat1[(i*N)+k] * mat2[(k*K)+j];	
	    		}
	    	}
	    	total++;
		}
	}
}

void multiply() {
    for(int i=0; i < M; i++){
    	for(int j=0; j<K; j++){
    		matRes[(i*K) + j] = 0;
    		for(int k=0; k <N; k++){
    			matRes[(i*K)+j] += mat1[(i*N)+k] * mat2[(k*K)+j];
    		}
    	}
    }
}

void multiplyWithThreads(int nThreads){
	int workload = M/nThreads;
	int start[2];
	for(int i =0; i <nThreads; i++){
		start[0]= i * workload - workload;
		start[1]=start[0]+workload-1;
		
		if(i+1==nThreads){
			start[1]=M+3;	
		}
		
		pthread_create(&t[i], NULL, thread_work, start);
		
	}
	for(int i =0; i <nThreads; i++){
		pthread_join(t[i], NULL);
	}
}

//--- DO NOT MODIFY BELOW HERE ---
int main(int argc, char* argv[])
{
    
    struct timeval start, end;
    double mult_time;

    if(argc != 2) {
        printf("Please provide 1 argument: #threads (int)\n");
        exit(-1);
    }

    int nthreads = atoi(argv[1]);
    
    if(nthreads <= 0){
        printf("Please enter a correct value for #threads\n");
        exit(-1);
    }
    printf("--- Reading in Matrices ---\n");

    mat1 = initialize(M, N, MAT_A);
    mat2 = initialize(N, K, MAT_B);
    matSol = initialize(M, K, MAT_C);
    matRes = malloc(M * K * sizeof(int)); 

    printf("--- Matrices Successfully Read, Multiplying Now ---\n");

    gettimeofday(&start, NULL);
    if(nthreads > 1){
        printf("--- Multiplying with %d threads ---\n", nthreads);
        t = (pthread_t *)malloc(sizeof(pthread_t)*nthreads);
        multiplyWithThreads(nthreads);
    } else {
        printf("--- Multiplying with a single thread ---\n");
        multiply();
    }
    //comment this out if you want to get rid of the matrices
    printMats();
    gettimeofday(&end, NULL); 
    mult_time = getTotalTime(start, end);

    if(compare()){
        printf("Correct! Your matrices multiplied properly\n");
    } else {
        printf("Incorrect! Your matrices did not multiply properly. Check your solution and try again.\n");
    }
    
    printf("This operation took %f seconds\n", mult_time);
    
    //Don't forget to free allocated heap memory!
    
    free(mat1);
    free(mat2);
    free(matRes);
    free(matSol);
    free(t);
    
    return 0;
}
