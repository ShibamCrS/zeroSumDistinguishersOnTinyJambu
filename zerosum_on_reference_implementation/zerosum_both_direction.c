#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "utility.h"
#include "tinyjambu_permutation.h"
#define RES_PATH "./data/conditional_f_%03d_b_%03d_d_%03d.txt"
#define NROF_THREADS 8

struct SumArgs{
    uint8_t *key;
    uint64_t start;
    uint64_t end;
    int degree;
    int forward_rounds;
    int backward_rounds;
    uint32_t result[4];
    uint32_t result_inverse[4];
};

typedef struct SumArgs SumArgs;

struct Sum{
    uint32_t forward[4];
    uint32_t backward[4];
};

typedef struct Sum Sum;

void* sum_over_range(void *args){
    SumArgs *sumargs = (SumArgs*) args;

	uint32_t sum[4] = {0x00000000,0x00000000,0x00000000,0x00000000};
	uint32_t sum_inverse[4] = {0x00000000,0x00000000,0x00000000,0x00000000};
    
    uint32_t state[4];
    uint32_t masked_state[4];
    state[0]= sumargs->start;
    state[1]=0x00000000;
    state[2]=0x00000000;
    state[3]=0x00000000;

    uint8_t inv_k[16];
    for(int i=0; i<16; i++){
    	inv_k[i] = (sumargs->key)[i];
    }
    rotate128(inv_k);
	
    for(uint64_t i=sumargs->start; i<sumargs->end; i++){ 
    	//this function is set proper cube varable
        left_shift_47_and_mask(state, masked_state);
        
        //computing output sums
        uint32_t a[4];
        apply(masked_state, sumargs->key, sumargs->forward_rounds, 0, a);
        uint32_t a_inverse[4];
        uint32_t roatated[4];
        rotate128_in32(masked_state, roatated);
        apply(roatated, inv_k, sumargs->backward_rounds, 1, a_inverse);
        for(int j=0; j<4; j++){
        	sum[j] ^= a[j];
        	sum_inverse[j] ^= a_inverse[j];
        }
        
        //next vector from the vector space
        state[0]++; // this will not work with degree >32
    }
    //collect sum result from thread
    for(int j=0; j<4; j++){
    	*(sumargs->result + j) = sum[j];
    	*(sumargs->result_inverse+j) = sum_inverse[j];
    }

    pthread_exit(NULL);
}

Sum apply_zerosum_threaded(uint8_t *key, int degree, int forward_rounds, int backward_rounds){
    pthread_t thread_ids[NROF_THREADS]; 
    SumArgs thread_args[NROF_THREADS];
    
    /*
    key_update_32(key, backward_rounds);
    printf("\nUpdated key by %d:\n", backward_rounds);
    printreg(key);
    */
    
    uint64_t cube_size = (1ULL << (degree + 1));
    for(int i=0; i < NROF_THREADS; i++){
        thread_args[i].key = key;
        thread_args[i].start = i * (cube_size / NROF_THREADS);
        thread_args[i].end = (i+1) * (cube_size / NROF_THREADS);
        thread_args[i].degree = degree;
        thread_args[i].forward_rounds = forward_rounds;
        thread_args[i].backward_rounds = backward_rounds;
    }

    for(int i=0; i < NROF_THREADS; i++){
        pthread_create(thread_ids + i, NULL, sum_over_range, (void*) (thread_args + i));
    }
    Sum sum;
    for(int i=0; i<4; i++){
    	sum.forward[i] = 0x00000000;
    	sum.backward[i] = 0x00000000;
    }
    
    for(int i=0; i < NROF_THREADS; i++){
        pthread_join(thread_ids[i], NULL);
        for(int j=0; j<4; j++){
        	sum.forward[j] ^= thread_args[i].result[j];
        	sum.backward[j] ^= thread_args[i].result_inverse[j];
        }
    }
    return sum;
}


void test_zerosum(int degree, int forward_rounds, int backward_rounds){
    //File to save results
    char fname[256];
    sprintf(fname, RES_PATH, forward_rounds, backward_rounds, degree);
    FILE *fp = fopen(fname, "w");
    uint8_t sum_mem[16];

	uint8_t key[16];
    
    Sum result;
    for(int i=0; i < 32; i++){
        generate_random_state_or_key(key);
        printf("\nKey:\n");
        printreg(key);
        result = apply_zerosum_threaded(key, degree, forward_rounds, backward_rounds);
        
        fprintf(fp, "Result %04d: \n", i);
        printf("Result %04d: \n", i);
        
        printreg_to_file(result.forward, fp);
        printreg(result.forward);
        
        fprintf(fp, "-----------------\n");
        printf("-----------------\n");
        
        printreg_to_file(result.backward, fp);
        printreg(result.backward);
        
        fprintf(fp, "\n");
        printf("\n");
        fflush(fp);       
    }
    fclose(fp);
}

//test tinyjambu and inverse tinyjambu permutation
void random_test(){
	srand(time(NULL));
	unsigned int nrof_rounds = 576;
	uint8_t key[16];
    uint8_t state[16];
    generate_random_state_or_key(key);
    generate_random_state_or_key(state);
    printf("Key : ");
    printreg(key);
    printf("Plaintext : ");
    printreg(state);
    
    uint32_t *state32 = (uint32_t *)state;
    uint32_t output[4];
    //encryption
    apply(state32, key, nrof_rounds, 0, output);
    printf("Ciphertext :");
    printreg(output);
    printf("*************************Encryption Done***********************\n");
    
    printreg(output);
    uint32_t rotated[4];
    rotate128_in32(output, rotated);
    printf("Rotate Ciphertext : ");
    printreg(rotated);
    key_update_32(key, nrof_rounds);
    rotate128(key);
    printf("Rotate Key : ");
    printreg(key);
    
    //decryption
    apply(rotated, key, nrof_rounds, 1, output);
    uint32_t rotated_output[4];
    rotate128_in32(output, rotated_output);
    printf("Decryption : ");
    printreg(rotated_output);
}

int main()
{
 	random_test();
 	srand(time(NULL));
 	
 	/*
 	Test Zerosum Distinguisher
 	*/
    int degree = 15;
    int forward_rounds = 480;
    int backward_rounds = 544;
    test_zerosum(degree, forward_rounds, backward_rounds); 
    
    return 0;
}
