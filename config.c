/*
 * config.c
 *
 *  Created on: Apr 6, 2017
 *      Author: user
 */

#include "config.h"
#define FILENAME "config"

void remove_char_newline(char * str) {
	while (*str++ != '\0')
	{
		if(*str=='\n') *str ='\0';
	}
}
uint8_t read_config_file(config_t* config) {
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(FILENAME, "r");
	if (fp == NULL)
		return 1;

	if ((read = getline(&line, &len, fp)) != -1) {
		remove_char_newline(line);
		config->dns_server_ip = strdup(line);
	} else {
		return 1;
	}
	if ((read = getline(&line, &len, fp)) != -1) {
		remove_char_newline(line);
		config->message = strdup(line);
	} else {
		free(config->dns_server_ip);
		return 1;
	}
	config->blasklist = hs_create(65536);
	while ((read = getline(&line, &len, fp)) != -1) {
		remove_char_newline(line);
		hs_insert(config->blasklist, line);
	}

	fclose(fp);
	if (line)
		free(line);
	return 0;
}
