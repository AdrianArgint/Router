#include "skel.h"
#include "parse.h"
#include "leading.h"
#include "arptable.h"
#include "queue.h"
#include "packet.h"


int main(int argc, char const *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	packet m;
	int rc;

	init();
	Node rtable = read_route_table();
	Arp_entry * arp_entries = NULL;
	queue packetQueue = queue_create();
	while (1) {
		// 1. Primeste un pachet de la oricare din interfetele adiacente.
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");
		/* Students will write code here */
        struct ether_header *eth_hdr = (struct ether_header *) m.payload;
  		struct iphdr *ip_hdr = (struct iphdr *) (m.payload + sizeof(struct ether_header));

		// 2. Daca este un pachet IP destinat routerului, raspunde doar in cazul in care acesta este un
		// pachet ICMP ECHO request. Arunca pachetul original.
    	if(ip_hdr->daddr == htonl(ip(get_interface_ip(m.interface)))) {
    		if(ip_hdr->protocol == 1){
    			send_icmp_packet(m, 0, 0);
    		}
			continue;
    	}

		// 3. Dace este un pachet ARP Request catre un IP al routerului, raspunde cu ARP Reply cu
		// adresa MAC potrivita.
        if(eth_hdr->ether_type == htons(0x806)){
        	struct arp_hdr *arp_header = (struct arp_hdr *)(m.payload + sizeof(struct ether_header));
        	if(arp_header->op_code == htons(1)){
        		send_arp_reply(m, arp_entries);
			}
		// 4. Daca este un pachet ARP Reply, updateaza tabela ARP; daca exista pachete ce trebuie
		// dirijate catre acel router, transmite-le acum.
        	else{
        		add_arp_entry(&arp_entries, arp_header->source_ip, arp_header->source_mac);
	        	if(!queue_empty(packetQueue)){
	    			packet_send(packetQueue, arp_entries);
	        	}
    		}	
        	continue;
        }

        
		// 5. Daca este un pachet cu TTL <= 1, sau un pachet catre o adresa inexistenta in tabela de
		// rutare, trimite un mesaj ICMP corect sursei (vezi mai jos); arunca pachetul.
        if (ip_hdr->ttl <= 1){
        	send_icmp_packet(m, 0, 11);
        	continue;
        }


		// 6. Daca este un pachet cu checksum gresit, arunca pachetul.
        if (ip_checksum(ip_hdr, sizeof(struct iphdr))){
            continue;
        }


		// 7. Decrementeaza TTL, updateaza checksum.
        ip_hdr->ttl--;
        ip_hdr->check = 0;
        ip_hdr->check = ip_checksum(ip_hdr, sizeof(struct iphdr));


		// 8. Cauta intrarea cea mai specifica din tabela de rutare (numita f) astfel incat (iph− >
		// daddr&f.mask == f.pref ix). Odata identificata, aceasta specifica next hop pentru pachet. 
		// In cazul in care nu se gaseste o ruta, se trimite un mesaj ICMP sursei; arunca pachetul
    	Route_table_entry *best_route = get_best_route(rtable,  htonl(ip_hdr->daddr));
        if(!best_route){
        	send_icmp_packet(m, 0, 3);
        	continue;
        }
		// 9. Modifica adresele source si destination MAC. Daca adresa MAC nu este cunoscuta local,
		// genereaza un ARP request si transmite pe interfata destinatie. Salveaza pachetul in coada
		// pentru transmitere. atunci cand adresa MAC este cunoscuta (pasul 4)
		uint8_t *mac = search_mac(arp_entries, ip_hdr->daddr);
		m.interface = best_route->interface;

		if(!mac){
			send_arp_request(best_route->next_hop, best_route->interface);
			packet *pkt = (packet*)malloc(sizeof(packet));
			memcpy(pkt, &m, sizeof(packet));
			queue_enq(packetQueue, pkt);
			continue;
		}
	
		memcpy(eth_hdr->ether_dhost, mac, 6 * sizeof(char));
		// 10. Trimite pachetul mai departe folosind functia send_packet(...).
		send_packet(m.interface, &m);	

	}

	return 0;
}

/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/