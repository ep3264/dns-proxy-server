/*
 * dns_server.c
 *
 *  Created on: Apr 7, 2017
 *      Author: user
 */

#include "dns_server.h"

int error(const char *msg, int code)
{
	fprintf(stderr, "%s\n", msg);
	return code;
}
//перенаправляем запрос вышестоящему серверу
int forward(struct dns_server_s *srv, int len , const char * server_ip)
{
	int sockfd, recvlen;
	struct sockaddr_in serv;
	memset(&serv, 0, sizeof(serv));
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr=inet_addr(server_ip);
	serv.sin_port=htons(53);
	sendto(	sockfd, srv->buffer, len, 0,
		(struct sockaddr *)&serv, sizeof(serv));
	recvlen = recvfrom(sockfd, srv->buffer, BUFF_SIZE, 0, NULL, NULL);
	close(sockfd);
	sendto(	srv->sock, srv->buffer, recvlen, 0,
		(struct sockaddr *)&srv->client, sizeof(srv->client));
#if DEBUG == 1
	printf("Packet forward success\n");
#endif
	return 0;
}


int handle_request(struct dns_server_s *srv, int len, config_t config)
{
	struct dns_header_s head;
	struct question_s question;
	resource_record_t answer;
	int8_t *ptr = srv->buffer;
	char *buf = NULL;
	uint16_t size;
	ptr = read_dns_header(&head, ptr);
	ptr = read_dns_question(&question, ptr);

#if DEBUG == 1
		printf("Received packet from %s:%d\tID: %04x\n",
			inet_ntoa(srv->client.sin_addr), ntohs(srv->client.sin_port), head.id);
		printf("Domain: %s\t\n", question.qname);
#endif

	if ((question.qtype != TYPE_A  || question.qclass != CLASS_IN) || !hs_ismember(config.blasklist, question.qname) )
	{
#if DEBUG == 1
		printf("Forward\n");
#endif
		forward(srv, len , config.dns_server_ip);
	}
	else
	{
#if DEBUG == 1
		printf("Blacklist\n");
#endif
		answer = create_dns_TXTanswer(question.qname, config.message);
		create_dns_response(&head, &question, &answer, (void **)&buf, &size);
		sendto(srv->sock, buf, size, 0, (struct sockaddr*)&srv->client, sizeof(srv->client));
	}
	return 0; //packet sent
}

int start_server(struct dns_server_s *srv, config_t config)
{
	int recvlen;
	int slen = sizeof(srv->client);

	if ((srv->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		return error("SOCKET", -1);
	memset(&srv->server, 0, sizeof(srv->server));
	srv->server.sin_family = AF_INET;
	srv->server.sin_port = htons(srv->port);
	srv->server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(srv->sock, (struct sockaddr*)&srv->server, sizeof(srv->server)) == -1)
		return error("BIND", -1);
	for (;;)
	{

		if ((recvlen = recvfrom(srv->sock, srv->buffer, BUFF_SIZE,
		     0, (struct sockaddr *)&srv->client, &slen)) == -1)
		{
			close(srv->sock);
			return error("RECV", -1);
		}
		handle_request(srv, recvlen, config);
	}
	close(srv->sock);
	return 0;
}
