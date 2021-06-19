/*
 * config.h
 *
 *  Created on: Apr 6, 2017
 *      Author: user
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "hashset.h"

struct config_s
{
	char * dns_server_ip;
	char * message;
	hashset_t * blasklist;
};
typedef struct config_s config_t;
uint8_t read_config_file(config_t* );
#endif /* CONFIG_H_ */
