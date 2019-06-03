/***
 * CPU cache simulation
 */

#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-variable"


/**
 * Cache architecture:
 */
#define ADDRSIZE		32		// 32 bits
#define BLOCKSIZE		32		// size of each block or line data
#define SETS			128		// number of sets 
#define LINESPERSET		4		// lines per set 
#define CACHESIZE		(BLOCKSIZE*SETS*LINESPERSET)  // total cache size (bytes)


/* MY CACHE */
unsigned Cvalid[SETS][LINESPERSET];  // just 0 or 1
unsigned Cdirty[SETS][LINESPERSET];  // just for write-back, 0 or 1 
unsigned Clru[SETS][LINESPERSET];    // just 0 or 1
unsigned Ctag[SETS][LINESPERSET];


/* counters for statistics */
int R, memR, hitR; // read ops,  mem reads,  cache hits
int W, memW, hitW; // write ops, mem writes, cache hits

/* find tag in a cache set */
int findInSet( int set, int tag ) {

	int i;
	for(i = 0; i < LINESPERSET; i++) {
		if(Cvalid[set][i] == 1) {
			if(Ctag[set][i] == tag) {
				return i;
			}	
		}
	}
	return -1; // not found
}

/* replace a cache line with new block read from mem */

void cacheUpdate( int set, int line, int tag ) {

	Cvalid[set][line]=1;   // valid entry
	Ctag[set][line] = tag; // tag
	Clru[set][line] = 1;   // accessed
	Cdirty[set][line] = 0; // not writen (just for write-back)
	
}


/* simulate one memory access */
void cacheAccess( unsigned int addr, char op ) {

	int block = 0;
	//int offset = 0;
	int set = 0;
	int tag = 0;
	int line = 0;
	
	//offset = addr % BLOCKSIZE;
	block = addr / BLOCKSIZE;
	
	set = block % SETS;
	tag = block / SETS;

	line = findInSet( set, tag );
 
	if ( line == -1 ) { // MISS
		
		int j, k;
		int found = 0;
		
		memR++;
		
	
		for(k = 0; k < LINESPERSET && found == 0; k++) {
		
			if(Cvalid[set][k] == 0) {
				
				cacheUpdate(set, k, tag );
				found = 1; 
				
				if(op == 'W') {
					Cdirty[set][k] = 1;
				}
				
			}
		}
		
		
		if(found == 0) {
		
			for(j = 0; j < LINESPERSET && found == 0; j++) {
			
					if(Clru[set][j] == 0) {
					
						if(Cdirty[set][j] == 1 ) {
							memW++;
						}
						
						cacheUpdate(set, j, tag );
						found = 1;
						
						if(op == 'W'){ 
							Cdirty[set][j] = 1;
						}
							
						
							
					}
				}
		}
		
			
		
	}
	
	else { // HIT
		
		Clru[set][line] = 1;
		
		
		if(op == 'W') {
			Cdirty[set][line] = 1;
			hitW++;
		} else {
			hitR++;
		}

	}
	
	int l,m;
	int found = 0;
	
	for(m = 0; m < LINESPERSET; m++) {
		if(Clru[set][m] == 0) {
			found = 1;
		}
	}
	
	
	if(found == 0) {
		
		for(l = 0; l < LINESPERSET; l++) {
			Clru[set][l] = 0;
		}
	
	}
	
}





















































/* print access statistics DON'T CHANGE */

void cache_printstats(void) {
    printf("%d bytes in cache as %d sets, %d lines per set with %d bytes (block size)\n",
				CACHESIZE, SETS, LINESPERSET, BLOCKSIZE);
	printf("Reads:  %7d, %7d hits (%.1f%%)\n", R, hitR, 100.0*(double)hitR/R);
	printf("Writes: %7d, %7d hits (%.1f%%)\n", W, hitW, 100.0*(double)hitW/W);
	printf("Total:  %7d, %7d hits (%.1f%%)\n", R+W, hitR+hitW, 100.0*((double)(hitR+hitW))/(R+W));
	printf("MemR: %d, MemW: %d (total %d, %.1f%%)\n",
				memR, memW, memR+memW, 100.0*((double)(memR+memW))/(R+W));
				
}

/*
 * MAIN function DON'T CHANGE
 */
int main( int ac, char *av[] ) {
	char op;
	unsigned int addr;
	FILE *traceFile;

	if ( ac != 1 && ac != 2 ) {	// check if there is a filename
		printf ("%s [trace-file]\n",  av[0]);
		return 1;
	}
	if ( ac==2 && (traceFile = fopen (av[1], "r")) == NULL) {
		perror( av[1] );
		return 2;
	}
	else if (ac==1) traceFile = stdin;  // no file, use stdin

	// simulation cycle:
	while ( fscanf( traceFile, "%x %c \n", &addr, &op ) == 2 ) {
		cacheAccess( addr, op );
    	if ( op == 'R' ) R++;
		else W++;
	}

	cache_printstats();
	return 0;
}
