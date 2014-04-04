#include <pcap/pcap.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <signal.h>
#include "libpq-fe.h"

struct query_queue {
	int buffer_size;
	int buffered;
	char *buffer;
};

void packet_handler(u_char *user,const struct pcap_pkthdr *h,const u_char *bytes);
void bin_dump(u_char t,char *join);
void queue_query(struct query_queue *queue,char *query);
void queue_flush(struct query_queue *queue);
void handler(int sig);

struct query_queue *pq_queue;
pcap_t *pcap_handle;

PGconn *pq_handle;

int main() {
	char errbuf[PCAP_ERRBUF_SIZE];
	if((pq_queue = malloc(sizeof(*pq_queue))) == NULL) error(1,errno,"malloc %i",__LINE__);
	pq_queue->buffer_size = 100000;
	pq_queue->buffered = 0;
	if((pq_queue->buffer = malloc(pq_queue->buffer_size)) == NULL) error(1,errno,"malloc %i",__LINE__);
	
	pq_handle = PQconnectdb("host = localhost dbname = bandwidth_monitor user = postgres");
	switch(PQstatus(pq_handle)) {
		case CONNECTION_BAD:
			printf("PQ Bad \n");
			return 1;
			break;
		case CONNECTION_OK:
			printf("PQ Okay \n");
			break;
	}
	
	if((pcap_handle = pcap_create("eth0",errbuf)) == NULL) {
		printf("%s \n",errbuf);
		return 1;
	}
	if(pcap_set_snaplen(pcap_handle,100) != 0) {
		printf("Snaplen error");
		return 1;
	}
	if(pcap_activate(pcap_handle) != 0) {
		pcap_perror(pcap_handle,"activate");
		return 1;
	}
	pcap_set_datalink(pcap_handle,228);
	signal(SIGINT,handler);
	pcap_loop(pcap_handle,-1,packet_handler,"");
	printf("Quitting\n");
	queue_flush(pq_queue);
	PQfinish(pq_handle);
	pcap_close(pcap_handle);
	return 0;
}

void packet_handler(u_char *user,const struct pcap_pkthdr *h,const u_char *bytes) {
	// unsigned int i;
	// for(i = 0;i < h->caplen;i++) bin_dump(bytes[i]," ");
	// printf("\n");
	const u_char *p = bytes+14;
	u_char protocol = p[9];
	unsigned short src_port = 0;
	unsigned short dst_port = 0;
	u_char ihl = (p[0]&15)*4;
	unsigned short length;
	char time_buffer[30];
	char query_buffer[1000];
	char src_ip[20];
	char dst_ip[20];
	struct tm localtime;
	
	
	memcpy(&length,p+2,2);
	length = ntohs(length);
	if(protocol == 6 || protocol == 17) {
		memcpy(&src_port,p+ihl,2);
		src_port = ntohs(src_port);
		memcpy(&dst_port,p+ihl+2,2);
		dst_port = ntohs(dst_port);
	}
	snprintf(src_ip,20,"%i.%i.%i.%i",p[12],p[13],p[14],p[15]);
	snprintf(dst_ip,20,"%i.%i.%i.%i",p[16],p[17],p[18],p[19]);
	
	localtime_r(&h->ts.tv_sec,&localtime);
	strftime(time_buffer,100,"%G-%m-%d %H:%M:%S",&localtime);
	// printf("prot:%i %s:%i -> %s:%i len:%i %s.%.6i\n",protocol,src_ip,src_port,dst_ip,dst_port,length,time_buffer,h->ts.tv_usec);
	// printf("%s:%i -> %s:%i\n",src_ip,src_port,dst_ip,dst_port);
	// printf("Outbound from %s, size: %i\n",src_ip,length);
	// printf("Inbound to %s, size: %i\n",dst_ip,length);
	snprintf(query_buffer,1000,"INSERT INTO traffic_log_outbound (time,ip,proto,src_port,dst_port,size) VALUES (TIMESTAMP '%s.%.6i',INET '%s',%i,%i,%i,%i);\n",time_buffer,h->ts.tv_usec,src_ip,protocol,src_port,dst_port,length);
	if(p[12] == 192 && p[13] == 168 && p[14] == 1) queue_query(pq_queue,query_buffer);
	
	snprintf(query_buffer,1000,"INSERT INTO traffic_log_inbound (time,ip,proto,src_port,dst_port,size) VALUES (TIMESTAMP '%s.%.6i',INET '%s',%i,%i,%i,%i);\n",time_buffer,h->ts.tv_usec,dst_ip,protocol,src_port,dst_port,length);
	if(p[16] == 192 && p[17] == 168 && p[18] == 1) queue_query(pq_queue,query_buffer);
	
	// snprintf(query_buffer,500,"INSERT INTO traffic_log (time,dst_ip,src_ip,proto,dst_port,src_port,size) VALUES ('%s.%.6i','%i.%i.%i.%i','%i.%i.%i.%i',%i,%i,%i,%i);",time_buffer,h->ts.tv_usec,p[16],p[17],p[18],p[19],p[12],p[13],p[14],p[15],protocol,dst_port,src_port,length);
	// printf("%s \n",query_buffer);
}

void bin_dump(u_char t,char *join) {
	int i;
	for(i = 7;i >= 0;i--) printf("%i",(t>>i)&1);
	printf("%s",join);
}

void queue_query(struct query_queue *queue,char *query) {
	if(queue->buffered+strlen(query) > queue->buffer_size) {
		PGresult *result;
		result = PQexec(pq_handle,queue->buffer);
		switch(PQresultStatus(result)) {
			case PGRES_EMPTY_QUERY:
				printf("Empty query \n");
				break;
			case PGRES_COMMAND_OK:
				// printf("Command okay \n");
				break;
			case PGRES_TUPLES_OK:
				// printf("Tuples okay \n");
				break;
			case PGRES_COPY_OUT:
				printf("Copy out \n");
				break;
			case PGRES_COPY_IN:
				printf("Copy in \n");
				break;
			case PGRES_BAD_RESPONSE:
				printf("Bad response \n");
				break;
			case PGRES_NONFATAL_ERROR:
				printf("Nonfatal error \n");
				break;
			case PGRES_FATAL_ERROR:
				fprintf(stderr,"Fatal error: %s \n",PQresultErrorMessage(result));
				exit(1);
				break;
		}
		queue->buffered = 0;
	}
	strcpy(queue->buffer+queue->buffered,query);
	queue->buffered += strlen(query);
}

void queue_flush(struct query_queue *queue) {
	if(queue->buffered == 0) return;
	PGresult *result;
	result = PQexec(pq_handle,queue->buffer);
	switch(PQresultStatus(result)) {
		case PGRES_EMPTY_QUERY:
			printf("Empty query \n");
			break;
		case PGRES_COMMAND_OK:
			// printf("Command okay \n");
			break;
		case PGRES_TUPLES_OK:
			// printf("Tuples okay \n");
			break;
		case PGRES_COPY_OUT:
			printf("Copy out \n");
			break;
		case PGRES_COPY_IN:
			printf("Copy in \n");
			break;
		case PGRES_BAD_RESPONSE:
			printf("Bad response \n");
			break;
		case PGRES_NONFATAL_ERROR:
			printf("Nonfatal error \n");
			break;
		case PGRES_FATAL_ERROR:
			fprintf(stderr,"Fatal error: %s \n",PQresultErrorMessage(result));
			exit(1);
			break;
	}
	queue->buffered = 0;
}

void handler(int sig) {
	pcap_breakloop(pcap_handle);
}
// CREATE TABLE traffic_log (
	// time		TIMESTAMP WITHOUT TIME ZONE,
	// dst_ip	INET,
	// src_ip	INET,
	// proto	INTEGER,
	// src_port	INTEGER,
	// dst_port	INTEGER,
	// size		INTEGER
// );

// CREATE TABLE traffic_log_outbound (
	// time		TIMESTAMP WITHOUT TIME ZONE,
	// ip		INET CHECK (ip << inet '192.168.1.0/24'),
	// proto	INTEGER,
	// src_port	INTEGER,
	// dst_port	INTEGER,
	// size		INTEGER
// );

// CREATE TABLE traffic_log_inbound (
	// time		TIMESTAMP WITHOUT TIME ZONE,
	// ip		INET CHECK (ip << inet '192.168.1.0/24'),
	// proto	INTEGER,
	// src_port	INTEGER,
	// dst_port	INTEGER,
	// size		INTEGER
// );

// INSERT INTO traffic_log (time,dst_ip,src_ip,proto,dst_port,src_port,size) VALUES ('2004-10-19 10:23:54.555','192.168.1.2','192.168.1.3',5,80,334,100);