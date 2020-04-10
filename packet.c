#include "packet.h"
#include "queue.h"


/* Trimite pachetele care au ramas restante catre hosturile ale caror 
   MAC-uri le-a aflat intre timp. */
void packet_send(queue q, Arp_entry * arp_entries){
	queue auxQ = queue_create();
	while(!queue_empty(q)){
		packet m = *((packet *) queue_deq(q));
		struct iphdr *ip_hdr = (struct iphdr *) (m.payload + sizeof(struct ether_header));
		uint8_t *mac = search_mac(arp_entries, ip_hdr->daddr);
		if(!arp_entries){
		}
		if(!mac){
			queue_enq(auxQ, &m);
		}
		else{
			struct ether_header *eth_hdr = (struct ether_header *) m.payload;
			memcpy(eth_hdr->ether_dhost, mac, 6 * sizeof(char));
			send_packet(m.interface, &m);
		}
	}
	/* Reface coada */
	while(!queue_empty(auxQ))
		queue_enq(q, queue_deq(auxQ));
}

/* Trimite un pachet de tipul icmp cu codul si tipul date ca parametru */
void send_icmp_packet(packet m, int code, int type){
	struct ether_header *eth_hdr = (struct ether_header *)m.payload;
	struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));
	struct icmphdr *icmp_hdr = (struct icmphdr *)(m.payload + sizeof(struct ether_header) + sizeof(struct iphdr));
	


	m.len = sizeof(struct ether_header) + sizeof(struct iphdr)
		+ sizeof(struct icmphdr);

	u_char mac[6];
	memcpy(mac, eth_hdr->ether_dhost, 6 * sizeof(char));
	memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost, 6 * sizeof(char));
	memcpy(eth_hdr->ether_shost, mac, 6 * sizeof(char));


	ip_hdr->protocol = 1;
	ip_hdr->version = 4;
	ip_hdr->ihl = 5;
	ip_hdr->id = htons(getpid());
	ip_hdr->ttl = 255;
	ip_hdr->tot_len = htons(m.len - sizeof(struct ether_header));
	uint32_t ipAux = ip_hdr->daddr;
	ip_hdr->daddr = ip_hdr->saddr;
	ip_hdr->saddr = ipAux;
	ip_hdr->check = 0;
	ip_hdr->check = ip_checksum(ip_hdr, sizeof(struct iphdr));
	

	icmp_hdr->code = code;
	icmp_hdr->type = type;
	icmp_hdr->un.echo.id = htons(getpid());
	icmp_hdr->un.echo.sequence = htons(1);
	icmp_hdr->checksum = 0;
	icmp_hdr->checksum = ip_checksum(icmp_hdr, sizeof(struct icmphdr));

	send_packet(m.interface, &m);
}



/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/