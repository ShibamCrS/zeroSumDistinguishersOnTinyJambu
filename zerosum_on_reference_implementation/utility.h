void printreg_to_file(const void *a, FILE *fp){
    int i;
    unsigned char *f = (unsigned char *)a;
    for(i=0; i < 16; i++){
        fprintf(fp, "%02x ",(unsigned char) f[15-i]);
    }
    fprintf(fp, "\n");
}
void printreg(const void *a){
    printreg_to_file(a, stdout);
}
void printreg192_to_file(const void *a, FILE *fp){
    int i;
    unsigned char *f = (unsigned char *)a;
    for(i=0; i < 24; i++){
        fprintf(fp, "%02x ",(unsigned char) f[23-i]);
    }
    fprintf(fp, "\n");
}
void printreg192(const void *a){
    printreg192_to_file(a, stdout);
}
uint8_t rotate8(uint8_t state){
    
    uint8_t temp = 0;
    for(int i=0; i<8; i++){
        if (((state >> i) & 0x1) == 1)
            temp = temp | (0x1 << (7-i));
        else
            temp = temp | (0x0 << (7-i));
    }
    //printf("state : %02x temp : %02x\n",state,temp);
    return temp;
}
void rotate128(uint8_t *input){
    uint8_t temp[16];
    for(int i = 0; i < 16; i++){
        temp[i] = input[15-i];
        temp[i] = rotate8(temp[i]);
    }
    for(int i = 0; i < 16; i++){
        input[i] = temp[i];
    }
}
/*
void rotate128_32(uint32_t *input){
    uint8_t temp[16];
    uint8_t *input8 = (uint8_t *)input;
    for(int i = 0; i < 16; i++){
        temp[i] = input8[15-i];
        temp[i] = rotate8(temp[i]);
    }
    for(int i = 0; i < 16; i++){
        input8[i] = temp[i];
    }
    input = (uint32_t *)input8;
}
*/

void xor_32(uint32_t *source, uint32_t *dest){
	for (int i=0; i<4; i++){
		dest[i] = source[i] ^ dest[i];
	}
}

void generate_random_state_or_key(uint8_t *random_data, int nrof_byte){
		for(int i=0; i<nrof_byte; i++){
			uint8_t a = rand() % 256;
			random_data[i] = a;
		}
}


void rotate128_in32(uint32_t *input, uint32_t *output){
    uint8_t *input8 = (uint8_t *)input;
    uint8_t output8[16];
    for(int i = 0; i < 16; i++){
        output8[i] = input8[15-i];
        output8[i] = rotate8(output8[i]);
    }
    for(int i = 0; i < 4; i++){
        output[i] = ((uint32_t *)output8)[i];
    }
}

void left_shift_47_and_mask(uint32_t *state, uint32_t *dest){
	  dest[0] = state[0];
	  dest[1] = (state[0] << 15);
	  dest[2] = (state[0] >> 17);
	  dest[3] = state[3];
}


