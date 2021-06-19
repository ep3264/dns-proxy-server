/*
 * dns.c
 *
 *  Created on: Apr 5, 2017
 *      Author: user
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "message.h"

int set_dns_headerfields(struct dns_header_s *head, enum header_fields field, uint16_t val)
{
	val = htons(val); //convert to big endian
	switch (field)
	{
		case FIELD_ID:		return head->id = val;
		case FIELD_QUESTIONS:	return head->qc = val;
		case FIELD_ANSWERS:	return head->ac = val;
		case FIELD_ADDITIONAL:	return head->dc = val;
		default:		return 0; //unknown field
	}
}

uint16_t get_dns_headerfields(struct dns_header_s *head, enum header_fields field)
{
	switch (field)
	{
		case FIELD_ID:		return htons(head->id);
		case FIELD_QUESTIONS:	return htons(head->qc);
		case FIELD_ANSWERS:	return htons(head->ac);
		case FIELD_NS:		return htons(head->nc);
		case FIELD_ADDITIONAL:	return htons(head->dc);
		default:		return 0xFFFF; //unknown field
	}
}

int set_dns_headeroptions(struct dns_header_s *head, enum header_options opt, uint8_t val)
{
	switch (opt)
	{
		case OPT_QR:			return head->qr = val;
		case OPT_OPCODE:		return head->op = val;
		case OPT_AUTHORITIVE_ANSWER:	return head->aa = val;
		case OPT_TRUNCATION:		return head->tc = val;
		case OPT_REQUEST_RECURSION:	return head->rd = val;
		case OPT_RECURSION_AVAILABLE:	return head->ra = val;
		case OPT_ZBIT:			return head->zf = val;
		case OPT_RCODE:			return head->rc = val;
		case OPT_AUTHENTICATED:		return head->ad = val;
		default: 			return 0; //unknown option
	}
}

uint8_t get_dns_headeroptions(struct dns_header_s *head, enum header_options opt)
{
	switch (opt)
	{
		case OPT_QR:			return head->qr;
		case OPT_OPCODE:		return head->op;
		case OPT_AUTHORITIVE_ANSWER:	return head->aa;
		case OPT_TRUNCATION:		return head->tc;
		case OPT_REQUEST_RECURSION:	return head->rd;
		case OPT_RECURSION_AVAILABLE:	return head->ra;
		case OPT_ZBIT:			return head->zf;
		case OPT_RCODE:			return head->rc;
		case OPT_AUTHENTICATED:		return head->ad;
	}
	return 0xFF; //unknown option
}

void init_dns_header(struct dns_header_s *head) //zeroes the entire header
{ memset(head, 0, sizeof(struct dns_header_s)); }

int8_t *int8ptr_postinc(int8_t **ptr, uint32_t increment)
{
	int8_t *ret = *ptr;
	*ptr += increment;
	return ret;
}

int8_t *read_dns_header(struct dns_header_s *head, int8_t *ptr)
{
	init_dns_header(head);
	memcpy((void *)head, int8ptr_postinc(&ptr, sizeof(struct dns_header_s)),
	       sizeof(struct dns_header_s));
	return ptr;
}

int8_t *read_dns_question(struct question_s *question, int8_t *ptr)
{
	int8_t *orig_ptr = ptr;
	uint8_t qname_tlen = 0;
	uint16_t qname_buf_size = 0;
	int8_t * qname_buf = NULL;
	int8_t * qname_tbuf = NULL;
	// Конвертируем доменное имя в обычный формат
	do
	{
		memcpy(&qname_tlen, int8ptr_postinc(&ptr, sizeof(uint8_t)),
			sizeof(uint8_t));
		if (qname_tlen == 0)
			break;
		qname_tbuf = (int8_t *)malloc(qname_tlen);
		memcpy(qname_tbuf, int8ptr_postinc(&ptr, qname_tlen),
			qname_tlen);
		if (qname_buf_size == 0)
		{
			qname_buf = (int8_t *)malloc(qname_tlen);
			qname_buf_size = qname_tlen;
			memcpy(qname_buf, qname_tbuf, qname_tlen);
		}
		else
		{
			int8_t *tmpbuf = (int8_t *)malloc(qname_buf_size +
						qname_tlen + 1);
			tmpbuf[qname_buf_size] = '.';
			memcpy(&tmpbuf[qname_buf_size + 1], qname_tbuf, qname_tlen);
			memcpy(tmpbuf, qname_buf, qname_buf_size);
			free(qname_buf);
			qname_buf = tmpbuf;
			qname_buf_size += qname_tlen + 1; //update size
		}
		free(qname_tbuf);
	} while (qname_tlen > 0);
	int8_t *qname_ptr = (int8_t *)malloc(qname_buf_size + 1);
	memcpy(qname_ptr, qname_buf, qname_buf_size);
	qname_ptr[qname_buf_size] = 0;
	free(qname_buf);
	question->qname = qname_ptr;

	memcpy(&question->qtype, int8ptr_postinc(&ptr, sizeof(uint16_t)),
		sizeof(uint16_t)); //read the type
	memcpy(&question->qclass, int8ptr_postinc(&ptr, sizeof(uint16_t)),
		sizeof(uint16_t)); //read the class
	question->qtype = htons(question->qtype);
	question->qclass = htons(question->qclass);
	return ptr; //return pointer to end of question
}

// Создаем ответ типа TEXT готовый к отправке
resource_record_t create_dns_TXTanswer(char * name,  char * message)
{
	resource_record_t answer;
	answer.name = strdup(name);
	answer.type = htons(0x10); //16 TXT
	answer.class= htons(0x1);
	answer.ttl = 0x0;
	int8_t len_m = strlen(message);
	answer.rdata =malloc(len_m+1);
	int8_t * ptr =  answer.rdata;
	*ptr++=len_m;
	for(int i=0;i<len_m;i++)
		*ptr++= *message++;
	answer.rdlength =htons(len_m+1);

	return answer;
}

// Подготовить данные к отправке и собрать в буфер
void create_dns_response(struct dns_header_s *head, struct question_s *question, resource_record_t *answer,
			void **buf, uint16_t *size)
{
	if (*buf == NULL)
		*buf = malloc(0xFF);
	struct dns_header_s resphead;
	char *ptr = (char *)*buf, *curr = ptr;
	uint16_t qname_size;
	uint8_t *qname;
	/****header section****/
	init_dns_header(&resphead);
	set_dns_headeroptions(&resphead, OPT_QR, 1);
	set_dns_headeroptions(&resphead, OPT_AUTHORITIVE_ANSWER, 1);
	set_dns_headeroptions(&resphead, OPT_AUTHENTICATED, 1);
	set_dns_headeroptions(&resphead, OPT_RECURSION_AVAILABLE, 1);
	set_dns_headeroptions(&resphead, OPT_REQUEST_RECURSION,
			   get_dns_headeroptions(head, OPT_REQUEST_RECURSION));
	set_dns_headerfields(&resphead, FIELD_ID,
			  get_dns_headerfields(head, FIELD_ID)); //mimic the ID
	set_dns_headerfields(&resphead, FIELD_QUESTIONS, 1); //reply with the question
	set_dns_headerfields(&resphead, FIELD_ANSWERS, 1);
	set_dns_headerfields(&resphead, FIELD_ADDITIONAL, 0);
	memcpy(int8ptr_postinc((int8_t **)&curr, sizeof(struct dns_header_s)),
	       &resphead, sizeof(struct dns_header_s)); //copy the header in
	/****question section****/
	qname = convert_name(question->qname, &qname_size);
	memcpy(int8ptr_postinc((int8_t **)&curr, qname_size), qname, qname_size); //copy the name
	*curr++ = 0x00; *curr++ = answer->type; //type TEXT
	*curr++ = 0x00; *curr++ = 0x01; //class=1 (IN)
	/****RR section****/
	uint16_t rd_len =ntohs(answer->rdlength);
	memcpy(int8ptr_postinc((int8_t **)&curr, qname_size), qname, qname_size); //copy domain name
	free(qname);
	memcpy(int8ptr_postinc((int8_t **)&curr, 0x02), &answer->type, 0x02);
	memcpy(int8ptr_postinc((int8_t **)&curr, 0x02), &answer->class, 0x02);
	memcpy(int8ptr_postinc((int8_t **)&curr, 0x04), &answer->ttl, 0x04);
	memcpy(int8ptr_postinc((int8_t **)&curr, 0x02), &answer->rdlength, 0x02); //copy RLENGTH
	memcpy(int8ptr_postinc((int8_t **)&curr, rd_len),answer->rdata, rd_len); //copy message
	*size = curr - ptr;
}

uint8_t *convert_name(int8_t *host, uint16_t *size)
{
	uint8_t *buf = malloc(255), *ptr = buf, *ret;
	int8_t *pch = strtok(host, ".");
	while (pch != NULL)
	{
		uint8_t len = strlen(pch);
		*ptr++ = len;
		memcpy(ptr, pch, len);
		ptr += len;
		pch = strtok(NULL, ".");
	}
	*ptr = 0;
	*size = ptr - buf + 1;
	ret = malloc(*size);
	memcpy(ret, buf, *size);
	free(buf);
	return ret;
}
