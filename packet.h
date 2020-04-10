#include "arptable.h"
#include "queue.h"
#include "skel.h"

void packet_send(queue q, Arp_entry * arp_entries);
void send_icmp_packet(packet m, int code, int type);



/*
	©Adrian Argint
	github.com/AdrianArgint		
								*/