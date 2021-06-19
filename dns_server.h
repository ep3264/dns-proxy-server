/*
 * dns_server.h
 *
 *  Created on: Apr 7, 2017
 *      Author: user
 */

#ifndef DNS_SERVER_H_
#define DNS_SERVER_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdint.h>
#include "message.h"
#include "config.h"
#define BUFF_SIZE 512
#define PORT 1155//53
#define DEBUG 1
/*
 *
 */
#define TYPE_A 		0x0001  // Доменное имя -> ip
#define TYPE_NS		0x0002 //  Соответсвие доменного имени и DNS сервера
#define TYPE_CNAME	0x0005  // Синонимы c.domain.com - > domain.com
#define TYPE_SOA	0x0006  // Описание доменной зоны
#define TYPE_WKS	0x000B
#define TYPE_PTR	0x000C  //ip -> Доменное имя
#define TYPE_MX		0x000F // Доставка почты по адресу user@domain.com
#define TYPE_SRV	0x0021
#define TYPE_A6		0x0026 // ipv6

#define CLASS_IN	0x0001 // class internet


struct dns_server_s
{
	uint16_t 		port;
	struct sockaddr_in	server,	client;
	int32_t			sock;
	int8_t			buffer[BUFF_SIZE];
};
typedef struct dns_server_s dns_server_t;

int start_server(struct dns_server_s *, config_t );
#endif /* DNS_SERVER_H_ */
