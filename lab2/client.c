//multicast client
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define buffersize 256
#define localIP "127.0.0.1"
#define multicastIP "226.1.1.1"

int main( int argc, char * argv[]){
	FILE *fp;
	int sd,datalen,filen,getb;
	float allpk,getpk,rate;
	char datatype[buffersize];			//for datatype
	char databuf[buffersize];			//for info about data
	char message[buffersize];			//for message in the file
	struct sockaddr_in localsock;
	struct ip_mreq group;

	sd=socket(AF_INET,SOCK_DGRAM,0);
	if(sd<0){
		perror("Opening datagram sock ...ERROR");
		exit(1);
	}
	else
		printf("Oening datagram sock ... OK\n");

	int reuse=1;
	if(setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(char*) &reuse,sizeof(reuse))<0){
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR ... OK\n");

	memset((char*) &localsock , 0 , sizeof(localsock));
	localsock.sin_family=AF_INET;
	localsock.sin_port=htons(5678);
	localsock.sin_addr.s_addr=INADDR_ANY;
	if(bind(sd,(struct sockaddr*) &localsock , sizeof(localsock))<0){
		perror("Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("Binding datagram socket ... OK\n");

	group.imr_multiaddr.s_addr=inet_addr(multicastIP);
	group.imr_interface.s_addr=inet_addr(localIP);
	if(setsockopt(sd,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*) &group,sizeof(group))<0){
		perror("Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("Adding multicast group ... OK\n");
	
	datalen=sizeof(databuf);
	if(read(sd,databuf,datalen)>0)
		printf("receive datatype is : %s\n",databuf);
	strcpy(datatype,databuf);
	memset(databuf,0,datalen);
	if(read(sd,databuf,datalen)>0)
		printf("receive filen is : %s\n",databuf);
	filen=atoi(databuf);
	memset(databuf,0,datalen);

	fp=fopen(datatype,"wb");
	getb=0;
	memset(message,0,sizeof(message));
	while(1){
		getb=read(sd,message,sizeof(message));
		if(strcmp(message,"end")==0){
			printf("receive end message\n");
			break;
		}
		fwrite(message,sizeof(char),sizeof(message),fp);
		memset(message,0,sizeof(message));
		getpk++;
	}

	memset(databuf,0,datalen);
	if(read(sd,databuf,datalen)>0){
		allpk=atof(databuf);
		rate=(allpk-getpk)/allpk;
		printf("get :%f ; all :%f ; rate :%f\n", getpk,allpk,rate);
	}
	fclose(fp);
	close(sd);
	return 0;
}
