#include "leading.h"
#include <arpa/inet.h>
#include "skel.h"
#pragma once


typedef struct arp_entry {
	uint32_t ip;
	uint8_t mac[6];
	struct arp_entry *next;
}Arp_entry;


struct arp_hdr {

	uint16_t hardware_type;
	uint16_t protocol_type;
	uint8_t hardware_address_len;
	uint8_t protocol_address_len;
	uint16_t op_code;
	uint8_t source_mac[6];
	uint32_t source_ip;
	uint8_t dest_mac[6];
	uint32_t dest_ip;
}__attribute__((packed));



void add_arp_entry(Arp_entry **arp_entries, uint32_t ip, uint8_t mac[6]);
uint8_t *search_mac(Arp_entry *arp_entries, uint32_t ip_to_find);
void send_arp_request(uint32_t ip_to_find, int interface);
void send_arp_reply(packet m, Arp_entry *arp_entries);
uint16_t ip_checksum(void* vdata,size_t length);


/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/