/*   
     TinyJAMBU-128: 128-bit key, 96-bit IV
     Optimized implementation 
     The state consists of four 32-bit registers       
     state[3] || state[2] || state[1] || state[0] 

     Implemented by: Hongjun Wu
*/ 

/*no-optimized date update function*/    
void state_update(unsigned int *state, const unsigned char *key, unsigned int number_of_steps) 
{
        unsigned int i;  
        unsigned int t1, t2, t3, t4, feedback; 
        for (i = 0; i < (number_of_steps >> 5); i++)
        {
                t1 = (state[1] >> 15) | (state[2] << 17);  // 47 = 1*32+15 
                t2 = (state[2] >> 6)  | (state[3] << 26);  // 47 + 23 = 70 = 2*32 + 6 
                t3 = (state[2] >> 21) | (state[3] << 11);  // 47 + 23 + 15 = 85 = 2*32 + 21      
                t4 = (state[2] >> 27) | (state[3] << 5);   // 47 + 23 + 15 + 6 = 91 = 2*32 + 27 
                feedback = state[0] ^ t1 ^ (~(t2 & t3)) ^ t4 ^ ((unsigned int*)key)[i & 3];
                // shift 32 bit positions 
                state[0] = state[1]; state[1] = state[2]; state[2] = state[3]; 
                state[3] = feedback ;
        }
}

/*
For inverse permutation, at first the state and key rotated so that ith bit becomes 127 - i
Thus the ith feedback function is 1 + x_0 + x_37 + x_43*x_58 + x_81 + k_i   

Inverse Permutation Implemented by: Shibam Ghosh
*/
void state_update_inverse(unsigned int *state, const unsigned char *key, unsigned int number_of_steps) 
{
        unsigned int i;  
        unsigned int t1, t2, t3, t4, feedback; 
        for (i = 0; i < (number_of_steps >> 5); i++){
		t1 = (state[1] >> 5) | (state[2] << 27);  // 37 = 1*32+5
		t2 = (state[1] >> 11)  | (state[2] << 21);  // 43 = 32 + 11
		t3 = (state[1] >> 26) | (state[2] << 6);  // 58 = 32 + 26
		t4 = (state[2] >> 17) | (state[3] << 15);   // 81 = 2*32 + 17
		feedback = state[0] ^ t1 ^ (~(t2 & t3)) ^ t4 ^ ((uint32_t*)key)[i & 3];
 		// shift 32 bit positions
		state[0] = state[1]; state[1] = state[2]; state[2] = state[3];
		state[3] = feedback ;
	}
}

void apply(const unsigned int *inputs, 
		   const unsigned char *key, 
		         unsigned int number_of_steps,
			              int inverse_flag,
		         unsigned int *outputs){
	for(int i=0; i<4; i++){
		outputs[i] = inputs[i];
	}
	if(inverse_flag == 0){
		state_update(outputs, key, number_of_steps);
	}
	else{
		state_update_inverse(outputs, key, number_of_steps);
	}
		      
}

void key_update_32(unsigned char *key, unsigned int number_of_steps){
	unsigned int i;
	unsigned int *round_keys = ((unsigned int *)key);
	for (i = 0; i < (number_of_steps >> 5); i++){
		unsigned int temp = round_keys[0];
		round_keys[0] = round_keys[1]; round_keys[1] = round_keys[2]; round_keys[2] = round_keys[3];
		round_keys[3] = temp;
	}
	key = ((unsigned char *)round_keys);
}
