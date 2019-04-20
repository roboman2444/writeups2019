//solver by AWG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>



char hash[32] = {0xc2, 0x71, 0x25, 0x6e, 0xf0, 0xa7, 0xbf, 0xea, 0x41, 0xf4, 0x83, 0xf1, 0x6b, 0x83, 0xaa, 0xf0, 0xd9, 0xb6, 0x6b, 0xbf, 0x64, 0x83, 0x3, 0x83, 0xac, 0xd3, 0x6f, 0xc3, 0x1d, 0x55, 0xe6, 0xe2};


int checkhash(char * data){
	uint8_t sha_digest[32];
	SHA256_CTX sha_ctx;
	SHA256_Init(&sha_ctx);
	SHA256_Update(&sha_ctx, data, strlen(data));
	SHA256_Final(sha_digest, &sha_ctx);
	return (!strncmp(sha_digest, hash, 32));
}


char * targets[] ={"e89", "b87", "8ad", "7c7", "e7c", "3a3", "425", "275",
			"b54", "1a6", "2ed","1c5", "fdf", "331", "2db",	"74f"};





typedef struct locloc_s {
	size_t len;
	size_t * locs;
} locloc_t;

locloc_t locs[16] = {0};		//list of where each target shows up in pi


size_t addloc(locloc_t *loc, size_t l){
	loc->len++;
	loc->locs = realloc(loc->locs, loc->len * sizeof(size_t));
	loc->locs[loc->len-1] = l;
	return loc->len;
}

int buildlocs(void){
	char * pies = 0;
	size_t pielen = (32768)*(sizeof(targets)/sizeof(*targets)) + 65536;	// two chars encoded, first one must be < 1 28 because ascii. add on md5 2 byte thing

	FILE *f = fopen("digits","r");
	if(!f) exit(1);

	pies = malloc(pielen);
	fread(pies, pielen-1, 1, f);
	fclose(f);
	pies[pielen-1] = 0;	//nully just in case

	int i;
	for(i = 0; i< sizeof(targets)/sizeof(*targets); i++){
		locloc_t *l = locs+i;
		size_t start = 0;
		if(i){	//first existence of first string cant be earlier than first existence of previous one
			start = locs[i-1].locs[0];
		}

		for(; start < pielen; start++){
			if(!strncmp(pies+start, targets[i], 3)) addloc(l, start);
		}

	}

	free(pies);
}

void printlocs(void){
	int i;
	for(i = 0; i< sizeof(targets)/sizeof(*targets); i++){
		locloc_t *l = locs+i;
		printf("%s, %li\n", targets[i], l->len);
//		size_t s;
//		for(s = 0; s< l->len; s++){
//			printf("\t %li\n", l->locs[s]);
//		}
	}
}



static inline int isprintfast(char c){
	return (c >31 && c < 127);
}

char buffer [sizeof(targets)/sizeof(*targets) * 2 +1] = {0};

void sstep(int stepdepth, size_t sdist){
	if( stepdepth >= sizeof(targets)/sizeof(*targets)){
//	if(stepdepth > 1){
		buffer[stepdepth*2] = 0;
		if(checkhash(buffer)){
			printf("%s\n", buffer);
		}
		return;
	}
	locloc_t *l = locs+stepdepth;
	//search till i find sdist;
	//todo could be optimized with a binary search or some funky remembering shit
	int loccy = 0;
	for( loccy = 0; loccy < l->len; loccy++){
		if(l->locs[loccy] >= sdist) break;
	}
	size_t max = sdist + 32768;
	//max i can go is 32768 past sdist, or else it wont be printable
	for(; loccy < l->len && l->locs[loccy] < max; loccy++){
		size_t delta = l->locs[loccy] - sdist;
		char one = delta & 0xff;
		char two = (delta >> 8) & 0xff;
		if(!isprintfast(one) || !isprintfast(two)) continue;	//final printable check
		buffer[stepdepth*2] = one;
		buffer[stepdepth*2+1] = two;
		sstep(stepdepth+1, l->locs[loccy]);
	}
}


//instead of searching for an offset that would work for "WP", could just early out on if the second set isnt "I{" and try all starts from 0-65536. It wouldnt be that slow, just a bit more complex to implement.
void dfslocs(void){
	//the first thing has to be "WP" before the start
	//so lets go through all the markers "WP" < m < 65536+"WP", subtract "WP" from them, and try
	int biggo = 'W' | 'P'<<8;
//	printf("%8x\n", biggo);
	int loccy = 0;
	locloc_t *l = locs;
	for( loccy = 0; loccy < l->len; loccy++){
		if(l->locs[loccy] >= biggo) break;
	}
	size_t max = biggo + 65537;
	for(; loccy < l->len && l->locs[loccy] < max; loccy++){
//		printf("%li\n", l->locs[loccy]-biggo);
		sstep(0, l->locs[loccy]-biggo);
	}
}

int main(void){
	buildlocs();
	printlocs();
	dfslocs();
	printf("\n");

}
