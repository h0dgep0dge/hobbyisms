/**************************************************\
* tun_vpn.c - A program to tunnel IP over UDP, 2.0 *
* By Robbie Mckennie                               *
* Now with NAT support!                            *
\**************************************************/

#include <stdio.h>
#include <signal.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int run = 1;

struct addrs {
	struct in_addr master;
	struct in_addr slave;
};

void client(int sock,char *server,short port);
void r_listen(int sock,short port,char *master_addr,char *slave_addr);
int create_tun(char *if_name);
int configure_tun(const char *if_name,struct addrs addresses,int master);
void io_tun(int tun,int sock);
void usage(char *name);
void missing(char *message,char *name);

void handler() {
	printf("QUITTING \n");
	run = 0;
}

int main(int argc,char *argv[]) {
	int sock,opt;
	int listener = 0;
	
	char server_addr[20] = "";
	char port[20] = "";
	char master_tun[20] = "";
	char slave_tun[20] = "";
	
	while ((opt = getopt(argc, argv, "hsm:p:l:r:")) != -1) {
		switch(opt) {
			case 'h': // Help
				usage(argv[0]);
				break;
			case 's': // Master mode
				listener = 1;
				break;
			case 'm': // Master ip address
				strncpy(server_addr,optarg,19);
				break;
			case 'p': // Port
				strncpy(port,optarg,19);
				break;
			case 'l': // Master tun address
				strncpy(master_tun,optarg,19);
				break;
			case 'r': // Slave tun address
				strncpy(slave_tun,optarg,19);
				break;
			default: // Bad argument
				usage(argv[0]);
				break;
		}
	}
	
	if(listener == 0) {
		if(server_addr[0] == 0) missing("Master server IP missing",argv[0]);
		if(port[0] == 0) missing("Master server port missing",argv[0]);
	} else if(listener == 1) {
		if(master_tun[0] == 0) missing("Master tun IP missing",argv[0]);
		if(slave_tun[0] == 0) missing("Slave tun IP missing",argv[0]);
		if(port[0] == 0) missing("Master bind port missing",argv[0]);
	}
	
	if((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0) error(1,errno,"Socket");
	
	if(listener == 0) client(sock,server_addr,atoi(port));
	else r_listen(sock,atoi(port),master_tun,slave_tun);
	return 0;
}

void client(int sock,char *server,short port) {
	struct sockaddr_in server_addr;
	struct timeval time;
	struct addrs tun_addr;
	
	int r,tun;
	
	fd_set select_set;
	
	char buf[2000];
	char if_name[20];
	
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if(inet_aton(server,&(server_addr.sin_addr)) == 0) {
		printf("Server address invalid \n");
		exit(1);
	}
	server_addr.sin_port = htons(port);
	
	if(connect(sock,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) error(1,errno,"Connect on line %i",__LINE__);
	if(send(sock,"CONFIGURE",9,0) < 0) error(1,errno,"Send on line %i",__LINE__);
	FD_ZERO(&select_set);
	FD_SET(sock,&select_set);
	memset(&time,0,sizeof(time));
	time.tv_sec = 10;
	if((r = select(FD_SETSIZE,&select_set,NULL,NULL,&time)) < 0) error(1,errno,"Select");
	if(r == 0) {
		fprintf(stderr,"Server timeout occured \n");
		exit(1);
	}
	if((r = read(sock,buf,14)) < 0) error(1,errno,"Read on line %i",__LINE__);
	if(strncmp(buf,"CONFIGURED-1.0",r) != 0) {
		fprintf(stderr,"Protocol mismatch \n");
		exit(1);
	}
	
	recv(sock,&tun_addr,sizeof(tun_addr),0);
	if((tun = create_tun(if_name)) < 0) {
		printf("Failed to create tun \n");
		exit(1);
	}
	configure_tun(if_name,tun_addr,0);
	
	io_tun(tun,sock);
	close(sock);
	close(tun);
}

void r_listen(int sock,short port,char *master_addr,char *slave_addr) {
	int r,tun;
	int confd = 0;
	
	struct sockaddr_in bind_addr;
	struct sockaddr_in client_addr;
	struct addrs tun_addr;
	
	char buf[2000];
	char if_name[20];
	
	memset(&tun_addr,0,sizeof(tun_addr));
	if(inet_aton(master_addr,&(tun_addr.master)) == 0) {
		printf("Address invalid %i \n",__LINE__);
		exit(1);
	}
	if(inet_aton(slave_addr,&(tun_addr.slave)) == 0) {
		printf("Address invalid %i \n",__LINE__);
		exit(1);
	}
	
	memset(&bind_addr,0,sizeof(bind_addr));
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind_addr.sin_port = htons(port);
	memset(&client_addr,0,sizeof(client_addr));
	if(bind(sock,(struct sockaddr *)&bind_addr,sizeof(bind_addr)) < 0) error(1,errno,"Bind");
	
	while(confd == 0) {
		int len = sizeof(client_addr);
		if((r = recvfrom(sock,buf,9,0,(struct sockaddr *)&client_addr,&len)) < 0) error(1,errno,"Recv on line %i",__LINE__);
		if(strncmp(buf,"CONFIGURE",9) == 0) {
			if((r = connect(sock,(struct sockaddr *)&client_addr,sizeof(client_addr))) < 0) error(1,errno,"Connect on line %i",__LINE__);
			if(send(sock,"CONFIGURED-1.0",14,0) < 0) error(1,errno,"Send on line %i",__LINE__);
			confd = 1;
		}
	}
	if((tun = create_tun(if_name)) < 0) {
		printf("Failed to create tun \n");
		exit(1);
	}
	configure_tun(if_name,tun_addr,1);
	send(sock,&tun_addr,sizeof(tun_addr),0);
	
	io_tun(tun,sock);
	
	close(sock);
	close(tun);
}

int create_tun(char *if_name) {
	struct ifreq ifr;
	int fd, err;

	if((fd = open("/dev/net/tun", O_RDWR)) < 0) return -1;

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

	if((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
		close(fd);
		return err;
	}
	strcpy(if_name, ifr.ifr_name);
	return fd;
}

int configure_tun(const char *if_name,struct addrs addresses,int master) {
	char command[100];
	char remote[20];
	char local[20];
	
	if(master == 1) {
		strcpy(local,(char *)inet_ntoa(addresses.master));
		strcpy(remote,(char *)inet_ntoa(addresses.slave));
	}
	else {
		strcpy(local,(char *)inet_ntoa(addresses.slave));
		strcpy(remote,(char *)inet_ntoa(addresses.master));
	}
	sprintf(command,"ifconfig %s %s pointopoint %s",if_name,local,remote);
	system(command);
	// printf("%s \n",command);
	return 0;
}

void io_tun(int tun,int sock) {
	struct timeval time;
	fd_set select_set;
	int len,size;
	char buf[2000];
	
	while(1) {
		FD_ZERO(&select_set);
		FD_SET(tun,&select_set);
		FD_SET(sock,&select_set);
		memset(&time,0,sizeof(time));
		time.tv_sec = 1;
		if(select(FD_SETSIZE,&select_set,NULL,NULL,&time) < 0) {
			error(1,errno,"select");
		}
		if(FD_ISSET(tun,&select_set)) {
			if((len = read(tun,buf,1500)) < 0) error(1,errno,"Read on line %i",__LINE__);
			send(sock,buf,len,0);
		}
		if(FD_ISSET(sock,&select_set)) {
			if((len = recv(sock,buf,1500,0)) < 0) error(1,errno,"Recv on line %i",__LINE__);
			write(tun,buf,len);
		}
	}
}

void usage(char *name) {
	printf("%s [-hs] [-m master_address] [-p port] [-l master_tun] [-r slave_tun] \n\n",name);
	printf("	-h	Display this message \n");
	printf("	-s	Start %s in master mode \n",name);
	printf("	-m	VPN controler IP address \n");
	printf("	-p	Port to connect to, or bind to in master mode \n");
	printf("	-l	Address to assign to the master tun device (master mode only) \n");
	printf("	-r	Address to assign to the slave tun device (master mode only) \n");
	exit(1);
}

void missing(char *message,char *name) {
	printf("%s \n",message);
	usage(name);
}
