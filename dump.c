/* Copyright (c) 2023, Julia Desmazes. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial
 * 4.0 International License.
 *
 * This code is provided "as is" without any express or implied warranties. */

#include "dump.h"
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "defs.h"

/* Example of ascii dump : 

> 0:00:00.265620 a130368b000000080060
> 0:00:00.280836 a1216c8b00000000000089086b0b82020407
< 0:00:00.295459 a2010800000000000000000800000000
> 0:00:00.296982 a1303c8b00000008007088286b0bc1ffcbf0f9ff
> 0:00:00.305644 a121718b0000000000008ba86a0b8008
< 0:00:00.319061 a2010900000000000000001000600000
> 0:00:00.330937 a130428b00000008007589186b0bb9ffd9f0fdfa3eb4295e99f3aaffd2f005
> 0:00:00.356037 a121788b0000000000008a18

encoding : 

regex: ^(?<dir>[<>])\s(?<time>\d+:\d\d:\d\d.\d+)\s(?<data>[0-9a-fA-F]+)$
timestamp: %H:%M:%S.%f
dir: in: <   out: >
encoding: HEX

*/

#define DUMP_FILE "network_dump.hex"

static FILE *fptr = NULL;


void init_dump_file(const char *path){
	/* open file */
	assert(path);
	fptr = fopen(path, "w");
	if(fptr == NULL)
   	{
    	printf("ERROR: dump file open failed, path %s\n", path);  
		assert(0); 
	}
}

void dump_eth_packet(
	const uint8_t *buff,
	const size_t len,
	const bool inbound
){

	if ( fptr == NULL ){
		init_dump_file(DUMP_FILE);
	}
	char dir = (inbound)? '<' : '>';
	fprintf(fptr,"%c ", dir);

	/* write time */
	long            ms; // Milliseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ms = (long)((double)spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }
 	
	struct tm * tinfo;
    tinfo = localtime(&s);

	fprintf(fptr,"%u:%02u:%02u.%06lu ",
		tinfo->tm_hour,
		tinfo->tm_min,
		tinfo->tm_sec,
		ms);

	/* start dump after MAC preamble */
	for(size_t i=8; i<len; i++){
		fprintf(fptr,"%02x",buff[i]);
		info("[%ld] %02x\n", i, buff[i]);
	}
	fprintf(fptr,"\n");
}

void close_dump(){
	if(fptr)
		fclose(fptr);
}
