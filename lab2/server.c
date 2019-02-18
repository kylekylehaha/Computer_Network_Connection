//multicast server
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define multicastIP "226.1.1.1"
#define localIP "127.0.0.1"
#define buffersize 256

int main(int argc , char* argv[]){
	FILE *fp;
	int sd,datalen,filen;
	struct sockaddr_in groupsock;
	struct in_addr localInterface;
	char END[]="end";
	char message[buffersize];
	char databuf[buffersize];
	char datatype[buffersize];
	strcpy(datatype,argv[1]);
	printf("Sengind data is :%s\n",datatype);
	datalen=sizeof(databuf);

	sd=socket(AF_INET,SOCK_DGRAM,0);
	if(sd<0)
		printf("ERROR opening datagram socket");
	else
		printf("Opening datagram socket ...ok\n");

	memset((char*) &groupsock,0,sizeof(groupsock));
	groupsock.sin_family=AF_INET;
	groupsock.sin_addr.s_addr=inet_addr(multicastIP);
	groupsock.sin_port=htons(5678);

	localInterface.s_addr=inet_addr(localIP);
	if(setsockopt(sd,IPPROTO_IP,IP_MULTICAST_IF,(char*) &localInterface,sizeof(localInterface))<0){
		printf("Setting localInterface error");
		exit(1);
	}
	else
		printf("Setting localInterface ...ok\n");

	fp=fopen(datatype,"rb");
	fseek(fp,0,SEEK_END);
	filen=ftell(fp);
	fseek(fp,0,SEEK_SET);
	strcpy(databuf,datatype);
	printf("datatype is : %s \n",databuf);
	if(sendto(sd,databuf,datalen,0,(struct sockaddr*) &groupsock,sizeof(groupsock))>0)
		printf("Sending filename ... OK\n");
	memset(databuf,0,sizeof(databuf));
	sprintf(databuf,"%d",filen);
	if(sendto(sd,databuf,datalen,0,(struct sockaddr*) &groupsock,sizeof(groupsock))>0)
		printf("Sending filen ... OK\n");
	memset(databuf,0,sizeof(databuf));
	size_t sendb;
	memset(message,0,sizeof(message));
	int count=0;
	while(sendb=fread(message,sizeof(char),sizeof(message),fp)){
		if(sendto(sd,message,sendb,0,(struct sockaddr*) &groupsock,sizeof(groupsock)) <= 0) {
			printf("fuck\n");
		}
		usleep(1);
		memset(message,0,sizeof(message));
	}
	memset(message,0,sizeof(message));
	if(sendto(sd,END,sizeof(END),0,(struct sockaddr*) &groupsock,sizeof(groupsock))>0)
		printf("Sending end message ... ok\n");

	memset(databuf,0,datalen);
	count=(filen/buffersize)+1;
	sprintf(databuf,"%d",count);
	if(sendto(sd,databuf,datalen,0,(struct sockaddr*) &groupsock,sizeof(groupsock))>0)
		printf("Sending number of package ... OK\n");
	printf("server complete\n");

	fclose(fp);
	close(sd);
	return 0;
}