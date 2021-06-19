/*
 * main.c
 *
 *  Created on: Apr 6, 2017
 *      Author: user
 */
#include "dns_server.h"




int main(void) {
	config_t config;

	if(read_config_file(&config) != 0 )
	{
		printf("Error read config file.\n");
	}
#if DEBUG == 1
	printf("Server :%s\n", config.dns_server_ip);
	printf("Message :%s\n", config.message);
#endif
	struct dns_server_s *srv = malloc(sizeof(struct dns_server_s));
	srv->port = PORT;
	return start_server(srv , config);

}
