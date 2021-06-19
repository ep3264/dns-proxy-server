/*
 * hashset.h
 *
 *  Created on: Apr 6, 2017
 *      Author: user
 */
#ifndef HASHSET_H
#define HASHSET_H 1


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

struct element_s {
	char *key;
	struct element_s *next;
};

typedef struct element_s element_t;

struct hashset_s {
	int size;
	struct element_s **table;
};

typedef struct hashset_s hashset_t;
hashset_t *hs_create( int size ) ;
void hs_insert( hashset_t *, char *);
uint8_t  hs_ismember( hashset_t *, char * );
#endif
