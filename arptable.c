#include "parse.h"
#include "leading.h"
#include "arptable.h"

/* Creeaza o celula de tip intrare ARP */
Arp_entry *add(uint32_t ip, uint8_t mac[6]){
	Arp_entry *aux = (Arp_entry*)malloc(sizeof(Arp_entry));
	if(!aux) return NULL;

	aux->ip = ip;
	memcpy(aux->mac, mac, 6 * sizeof(char));
	aux->next = NULL;
	return aux;
}

/* Introduce datele ARP in tabela ARP */
void add_arp_entry(Arp_entry **arp_entries, uint32_t ip, uint8_t mac[8]){
	if(*arp_entries == NULL){
		(*arp_entries) = add(ip, mac);
		return;
	}

	Arp_entry *arp = *arp_entries;
	while(arp->next != NULL){
		arp = arp->next;
	}

	arp->next = add(ip, mac);
}

/* Cauta MAC-ul corespunzator IP-ului ip_to_find in tabela si-l returneaza */
uint8_t *search_mac(Arp_entry *arp_entries, uint32_t ip_to_find){
	printf("Caut in ARP IP-ul "); 
	if(arp_entries == NULL) return NULL;
	
	while(arp_entries){
		if(arp_entries->ip == ip_to_find){
			return arp_entries->mac;
		}

		arp_entries = arp_entries->next;
	}

	return NULL;
}

/* Trimite un pachet de tip ARP REQUEST catre ip_to_find pe interfata interface */
void send_arp_request(uint32_t ip_to_find, int interface){

	//creez pachet pt a afla cine este IP_TO_FIND
	packet arp_packet;
	arp_packet.len = sizeof(struct ether_header) + sizeof(struct arp_hdr);
	struct ether_header *hdr_eth = (struct ether_header *)arp_packet.payload;
	struct arp_hdr *arp = (struct arp_hdr *)(arp_packet.payload + sizeof(struct ether_header));
	uint8_t *mac = (uint8_t *)malloc(sizeof(char) * 6);

	//ETHERNET HEADER
	memset(hdr_eth->ether_dhost, 0xff, sizeof(char) * 6);
	get_interface_mac(interface, mac);
	memcpy(hdr_eth->ether_shost, mac, sizeof(char) * 6);
	hdr_eth->ether_type= htons(0x806);

	//ARP HEADER
	arp->hardware_type = htons(0x1);
	arp->protocol_type = htons(0x800);
	arp->hardware_address_len = 6;
	arp->protocol_address_len = 4;
	arp->op_code = htons(0x1);
	memcpy(arp->source_mac, mac, sizeof(char) * 6);
	arp->source_ip = htonl(ip(get_interface_ip(interface)));
	memset(arp->dest_mac, 0x0, sizeof(char) * 6);
	arp->dest_ip = htonl(ip_to_find);

	arp_packet.interface = interface;
	send_packet(interface, &arp_packet);
}

void send_arp_reply(packet m, Arp_entry *arp_entries){
    struct ether_header *eth_hdr = (struct ether_header *) m.payload;
	uint8_t *mac = (uint8_t*)malloc(6 * sizeof(char));
	get_interface_mac(m.interface, mac);
	struct arp_hdr *arp_header = (struct arp_hdr *)(m.payload + sizeof(struct ether_header));

	packet arp_packet;
	arp_packet.len = sizeof(struct ether_header) + sizeof(struct arp_hdr);
	//printf("Creez un pachet nou cu care sa raspund\n");
	//ETHERNET HEADER
	struct ether_header *hdr_eth = (struct ether_header *)arp_packet.payload;
	memcpy(hdr_eth->ether_dhost, eth_hdr->ether_shost, sizeof(char) * 6);
	memcpy(hdr_eth->ether_shost, mac, sizeof(char) * 6);
	hdr_eth->ether_type= htons(0x806);


	//ARP HEADER
	struct arp_hdr *arp = (struct arp_hdr *)(arp_packet.payload + sizeof(struct ether_header));
	
	arp->hardware_type = htons(0x1);
	arp->protocol_type = htons(0x800);
	arp->hardware_address_len = 6;
	arp->protocol_address_len = 4;
	arp->op_code = htons(0x2);
	memcpy(arp->source_mac, mac, sizeof(char) * 6);//
	arp->source_ip = htonl(ip(get_interface_ip(m.interface)));
	memcpy(arp->dest_mac, arp_header->source_mac, sizeof(char) * 6);
	arp->dest_ip = htonl(arp_header->source_ip);

	arp_packet.interface = m.interface;
	send_packet(arp_packet.interface, &arp_packet);
	add_arp_entry(&arp_entries, arp_header->source_ip, arp_header->source_mac);
}


/*functia de checksum din laborator */
uint16_t ip_checksum(void* vdata,size_t length) {
	// Cast the data pointer to one that can be indexed.
	char* data=(char*)vdata;

	// Initialise the accumulator.
	uint64_t acc=0xffff;

	// Handle any partial block at the start of the data.
	unsigned int offset=((uintptr_t)data)&3;
	if (offset) {
		size_t count=4-offset;
		if (count>length) count=length;
		uint32_t word=0;
		memcpy(offset+(char*)&word,data,count);
		acc+=ntohl(word);
		data+=count;
		length-=count;
	}

	// Handle any complete 32-bit blocks.
	char* data_end=data+(length&~3);
	while (data!=data_end) {
		uint32_t word;
		memcpy(&word,data,4);
		acc+=ntohl(word);
		data+=4;
	}
	length&=3;

	// Handle any partial block at the end of the data.
	if (length) {
		uint32_t word=0;
		memcpy(&word,data,length);
		acc+=ntohl(word);
	}

	// Handle deferred carries.
	acc=(acc&0xffffffff)+(acc>>32);
	while (acc>>16) {
		acc=(acc&0xffff)+(acc>>16);
	}

	// If the data began at an odd byte address
	// then reverse the byte order to compensate.
	if (offset&1) {
		acc=((acc&0xff00)>>8)|((acc&0x00ff)<<8);
	}

	// Return the checksum in network byte order.
	return htons(~acc);
}



/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/