/*
 * message.h
 *
 *  Created on: Apr 5, 2017
 *      Author: user
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <stdint.h>

enum header_options {
		OPT_QR  		=	0x001,
		OPT_OPCODE 		=	0x002,
		OPT_AUTHORITIVE_ANSWER 	=	0x004,
		OPT_TRUNCATION		=	0x008,
		OPT_REQUEST_RECURSION	=	0x010,
		OPT_RECURSION_AVAILABLE	=	0x020,
		OPT_ZBIT 		=	0x040,
		OPT_RCODE		=	0x080,
		OPT_AUTHENTICATED	=	0x100};

enum header_fields {
		FIELD_ID		=	0x01,
		FIELD_QUESTIONS		=	0x02,
		FIELD_ANSWERS		=	0x04,
		FIELD_NS		=	0x08,
		FIELD_ADDITIONAL	=	0x10};

struct dns_header_s
{
	uint16_t id;
	uint8_t rd		:1; //request recursion
	uint8_t tc		:1; //truncated
	uint8_t aa		:1; //authoritive answer Принадлежность текущей зоне
	uint8_t op		:4; //opcode
	uint8_t qr		:1; //QUERY=1,RESPONSE=0
	uint8_t rc		:4; //response code
	uint8_t cd		:1; //checking disabled
	uint8_t ad		:1; //authenticated data
	uint8_t zf		:1; //Z-flag (unused)
	uint8_t ra		:1; //recursion available
	uint16_t qc; //question count
	uint16_t ac; //answer count
	uint16_t nc; //NS count
	uint16_t dc; //additional count, should always use 0
};

struct question_s
{
	int8_t *qname; // Доменное имя (формат 6domain3com0)
	uint16_t qtype; // Тип запроса нас интересует только A 0x0001
	uint16_t qclass; // интересует только класс IN 0x0001
};
typedef struct question_s question_t;
struct resource_record_s
{
	int8_t *name;
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
	int8_t * rdata;

};
typedef struct resource_record_s resource_record_t;

int set_dns_headerfields(struct dns_header_s *head, enum header_fields field, uint16_t val);
uint16_t get_dns_headerfields(struct dns_header_s *head, enum header_fields field);
int set_dns_headeroptions(struct dns_header_s *head, enum header_options opt, uint8_t val);
uint8_t get_dns_headeroptions(struct dns_header_s *head, enum header_options opt);
void init_dns_header(struct dns_header_s *head);
int8_t *int8ptr_postinc(int8_t **ptr, uint32_t increment);
int8_t *read_dns_header(struct dns_header_s *head, int8_t *ptr);
int8_t *read_dns_question(struct question_s *question, int8_t *ptr);
resource_record_t create_dns_TXTanswer(char * name,  char * message);
void create_dns_response(struct dns_header_s *head, struct question_s *question, resource_record_t *answer,
			void **buf, uint16_t *size);
uint8_t *convert_name(int8_t *host, uint16_t *size);


#endif /* MESSAGE_H_ */
