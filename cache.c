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
#define LINES			(SETS*LINESPERSET)		// number of lines 
#define CACHESIZE		(BLOCKSIZE*SETS*LINESPERSET)  // total cache size (bytes)


/* MY CACHE */
unsigned Cvalid[SETS][LINESPERSET];  // just 0 or 1
unsigned Cdirty[SETS][LINESPERSET];  // just for write-back, 0 or 1 
unsigned Clru[SETS][LINESPERSET];    // just 0 or 1
unsigned Ctag[SETS][LINESPERSET];
//unsigned Cdata[SETS][LINESPERSET][BLOCKSIZE]; // data - not used in this simulation


/* counters for statistics */
int R, memR, hitR; // read ops,  mem reads,  cache hits
int W, memW, hitW; // write ops, mem writes, cache hits





/* find tag in a cache set */
int findInSet( int set, int tag ) {
	
	if(Cvalid[set][lineForThis] == 1) {
		if(Ctag[set][lineForThis] == tag) {
			return lineForThis;
		}
	} else {
		return -1; // not found
	}

	
}

/* replace a cache line with new block read from mem */
void cacheUpdate( int set, int line, int tag ) {
	Cvalid[set][line]=1;   // valid entry
	Ctag[set][line] = tag; // tag
	Clru[set][line] = 1;   // accessed
	Cdirty[set][line] = 0; // not writen (just for write-back)
    //Cdata[set][line] <- mem block. Not used in this simulation
}

/* simulate one memory access */
void cacheAccess( unsigned int addr, char op ) {
	// TODO
	int block = 0;
	int offset = 0;
	int set = 0;
	int tag = 0;
	int line = 0;
	
	offset = addr % BLOCKSIZE;
	block = addr / BLOCKSIZE;
	set = block % SETS;
	tag = block / SETS;

	line = findInSet( set, tag );
	
	cacheUpdate(set, line, tag );

	if ( line == -1 ) { // MISS
	// TODO
	int i, j, numOf1;
		for(i = 0; i < SETS; i++) {
			for(j = 0, i < LINESPERSET; j++) {
				if(Clru[i][j] == 1) {
					numOf1++;
				}
			}
	
		}
	if(numOf1 == LINES) {
		for(i = 0; i < SETS; i++) {
			for(j = 0, i < LINESPERSET; j++) {
				Clru[i][j] = 0;
			}
	
		}
	}
		
	} else { // HIT
		// TODO

	}
	if ( op == 'W' )  // if write-through always writes to mem
		memW++;
	
	

	// TODO
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
