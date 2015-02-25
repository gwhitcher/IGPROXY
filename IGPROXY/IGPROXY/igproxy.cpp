/* IGPROXY */

/* INCLUDES */
#include <string>
#include <stdio.h>
#include <winsock2.h>
#include "windows.h"
#include <winuser.h>
#include <winbase.h>
#include <cstdlib>
#include <iostream>
#include "igproxy.h"
#include "packetsender.h"

using namespace std;

/* DEFINES */
#define IGPROXY_VERSION "0.01"
#define PROXY_ADDRESS "127.0.0.1"
#define PROXY_PORT "9000"
#define DESINATION_ADDRESS "127.0.0.1"
#define DESINATION_PORT "9001"
#define BUF_SIZE 65530

#define ZeroMemory RtlZeroMemory

/* FUNCTION PROTOTYPES */
void debug(char *fmt, ...);
void tcp_listen(int sockfd);
int tcp_accept(int sockfd, struct sockaddr *client_addr, int *addr_len);
int tcp_sock_connect(char* dest_addr, int port);
int tcp_sock_open(struct in_addr ip, int port);
int tcp_packet_write(int sockfd, struct tcp_packet *tcp_pkt);
int tcp_packet_read(int sockfd, struct tcp_packet *tcp_pkt);
int tmp_sockfd, p_sockfd = 0;
int csa_len;
struct sockaddr_in csa;
int s_sockfd = 0, pc_sockfd = 0;
extern void packetsender_main();


int iRecv;
int iResponseLength=0;
char recvBuffer[BUF_SIZE]={0};
unsigned char packet_data[BUF_SIZE];  
unsigned int max_packet_size =  sizeof( packet_data ); 
const char snsn[]="%s\n";
string response;


/* PRINT IN LOG AND CONSOLE */
void debug(const char* text, ...)
{
	//SAVE TO LOG
	FILE* pFile = fopen("log.txt", "a");
	fprintf(pFile, "%s\n", text);
	fclose(pFile);

	//PRINT IN CONSOLE
	printf("%s\n", text);
}

/* MAKE IP INTO HEX STRING */
unsigned parseIPV4string(char* ipAddress) {
	unsigned j, j2, j3;
	unsigned ipbytes[4];
	char buf[16];

	j = j2 = j3 = 0;

	while (j2 < 4)
	{
		switch (ipAddress[j])
		{
		case 0x00:
		case 0x2E:
			buf[j3++] = 0x00;
			ipbytes[j2++] = atoi(&buf[0]);
			j3 = 0;
			if (ipAddress[j] == 0x00) j2 = 4;
			break;
		default:
			buf[j3++] = ipAddress[j];
			break;
		}
		j++;
	}
	return ipbytes[0] | (ipbytes[1] << 8) | (ipbytes[2] << 16) | (ipbytes[3] << 24);
}
struct in_addr HEX_PROXY_ADDRESS;

/* CONNECTION METHODS */
WSADATA winsock_data;
void tcp_listen(int sockfd)
{
	if (listen(sockfd, 10) < 0)
	{
		debug("Could not listen for connection");
		exit(1);
	}
}

int tcp_accept(int sockfd, struct sockaddr *client_addr, int *addr_len)
{
	int fd;

	if ((fd = accept(sockfd, client_addr, addr_len)) < 0)
	{
		debug("Could not accept connection");
		exit(1);
	}

	return (fd);
}

int tcp_sock_connect(char* dest_addr, int port)
{
	int fd;
	struct sockaddr_in sa;

	/* CLEAR */
	memset((void *)&sa, 0, sizeof(sa));
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* ERROR */
	if (fd < 0){
		debug("Could not create socket");
		exit(1);
	}

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(dest_addr);
	sa.sin_port = htons((unsigned short)port);
	if (connect(fd, (struct sockaddr*) &sa, sizeof(sa)) < 0)
	{
		debug("Could not make TCP connection");
		exit(1);
	}
	return(fd);
}

int tcp_sock_open(struct in_addr ip, int port)
{
	int fd, turn_on_option_flag = 1, rcSockopt;
	struct sockaddr_in sa;

	/* CLEAR */
	memset((void *)&sa, 0, sizeof(sa));
	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* ERROR */
	if (fd < 0){
		debug("Could not create socket %s\n", strerror(errno));
		exit(1);
	}

	sa.sin_family = AF_INET;
	memcpy((void *)&sa.sin_addr, (void *)&ip, sizeof(struct in_addr));
	sa.sin_port = htons((unsigned short)port);

	/* REUSE PORT (ICS?) */
	rcSockopt = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&turn_on_option_flag, sizeof(turn_on_option_flag));

	/* BIND THE SOCKET TO THE INTERFACE */
	if (bind(fd, (struct sockaddr *)&sa, sizeof(struct sockaddr)) < 0){
		debug("Could not bind to port");
		exit(1);
	}

	return(fd);
}

std::string packet_info;
const char * fixed_packet_info = packet_info.c_str();
void send_packet(string packet_info) {
	int sent_bytes = send(pc_sockfd, fixed_packet_info, max_packet_size, 0);
	if ( sent_bytes != max_packet_size ) { 
		printf( "\nFailed to send packet.\n" ); 
	} else {
		printf( "\nPacket sent\n" );
	}
}


/* MAIN */
int main(int argc, char *argv[])
{	
	SetConsoleTitle("IGPROXY v"IGPROXY_VERSION"");
	debug("IGPROXY v"IGPROXY_VERSION"");
	debug("Proxy server started: IP: "PROXY_ADDRESS" PORT: "PROXY_PORT"");

	/* SETUP CONNECTION */
	debug ("Waiting for connection.");
	WSAStartup(MAKEWORD(1, 1), &winsock_data);
	memset((void*)&HEX_PROXY_ADDRESS, 0, sizeof (HEX_PROXY_ADDRESS));
	HEX_PROXY_ADDRESS.s_addr = parseIPV4string(PROXY_ADDRESS);
	p_sockfd = tcp_sock_open(HEX_PROXY_ADDRESS, atoi(PROXY_PORT));
	tcp_listen(p_sockfd);

	/* ACCEPT CONNECTION */
	debug ("Attempting to accept connection.");
	csa_len = sizeof(csa);
	pc_sockfd = tcp_accept(p_sockfd, (struct sockaddr*) &csa, &csa_len);
	debug ("Connection accepted.");

	/* SEND TO DESTINATION */
	debug ("Attempting connection to "DESINATION_ADDRESS" "DESINATION_PORT);
	s_sockfd = tcp_sock_connect(DESINATION_ADDRESS, atoi(DESINATION_PORT));
	debug ("Connection successful.");

	//LOAD PACKET SENDER WINDOW
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&packetsender_main, (LPVOID)NULL, 0, 0);

	/* DISPLAY RECEIVED PACKETS IN CONSOLE */
	unsigned char packet_data[BUF_SIZE];  
	unsigned int max_packet_size = sizeof( packet_data ); 
	while(iRecv=recv(s_sockfd, (char*) &packet_data[0], max_packet_size, 0)>0)
    {
        char hex[5];
		const char* display_hex = hex;
        string packet;
        packet.reserve(5*iRecv);
        printf(snsn,"\n");
        printf("%s","Receiving ");
        printf("%d",iRecv);
        printf(snsn," byte packet:\n");
        for(int i=0;i<iRecv;++i)
        {
            wsprintf(hex,"%02x",(unsigned char)recvBuffer[i]);
            packet.append(hex);
            //printf("0x%s ",hex);
			debug(display_hex);
        }
        response.append(recvBuffer,iRecv);
        iResponseLength+=iRecv;
        ZeroMemory(recvBuffer,BUF_SIZE);
    }

	//KEEP WINDOW OPEN
	cin.get();
    return 0;
	//system("PAUSE");
}