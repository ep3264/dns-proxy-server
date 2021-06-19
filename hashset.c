/*
 * hashset.c
 *
 *  Created on: Apr 6, 2017
 *      Author: user
 */


#include "hashset.h"
#include <assert.h>



hashset_t *hs_create( int size ) {

	hashset_t *hashset = NULL;
	int i;

	if( size < 1 ) return NULL;

	/* Allocate the table itself. */
	if( ( hashset = malloc( sizeof( hashset_t ) ) ) == NULL ) {
		return NULL;
	}

	/* Allocate pointers to the head nodes. */
	if( ( hashset->table = malloc( sizeof( element_t * ) * size ) ) == NULL ) {
		return NULL;
	}
	for( i = 0; i < size; i++ ) {
		hashset->table[i] = NULL;
	}

	hashset->size = size;

	return hashset;
}
// хеш-функция для строк
int hs_hash( hashset_t *hashset, char *key ) {

	unsigned long int hashval;
	int i = 0;
	/* Convert our string to an integer */
	while( hashval < ULONG_MAX && i < strlen(key)) {
		hashval = hashval << 8;
		hashval += key[ i ];
		i++;
	}
	return hashval % hashset->size;
}


element_t *ht_new_el(char *key) {
	element_t *element;

	if( ( element = malloc( sizeof( element_t ) ) ) == NULL ) {
		return NULL;
	}

	if( ( element->key = strdup( key ) ) == NULL ) {
		return NULL;
	}

	element->next = NULL;

	return element;
}


void hs_insert( hashset_t *hashset, char *key) {
	int bin = 0;
	element_t *new_element = NULL;
	element_t *next = NULL;
	element_t *last = NULL;

	bin = hs_hash( hashset, key );

	next = hashset->table[ bin ];

	while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
		last = next;
		next = next->next;
	}

	/* Если ключ уже есть в таблице - выходим. */
	if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {
		return;
	}
	new_element = ht_new_el(key);

	if (next == hashset->table[bin]) {
		new_element->next = next;
		hashset->table[bin] = new_element;

	} else if (next == NULL) {
		last->next = new_element;

	} else {
		new_element->next = next;
		last->next = new_element;
	}

}

uint8_t  hs_ismember( hashset_t *hashset, char *key ) {
	int bin = 0;
	element_t *element;

	bin = hs_hash(hashset, key );

	element = hashset->table[ bin ];
	while( element != NULL && element->key != NULL && strcmp( key, element->key ) > 0 ) {
		element = element->next;
	}

	if( element == NULL || element->key == NULL || strcmp( key, element->key ) != 0 ) {
		return 0;

	} else {
		return 1;
	}

}
