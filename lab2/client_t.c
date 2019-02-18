// multithread client			-->recv message(tcp)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#define buffersize 256
#define localIP "127.0.0.1"

int main(int argc, char* argv[]){
	FILE *fp;
	int sd,i,getb;
	float getpk,allpk,rate;
	char buffer[buffersize];
	char datatype[buffersize];
	struct sockaddr_in info;

	sd=socket(AF_INET,SOCK_STREAM,0);
	if(sd<0){
		printf("ERROR create sockfd");
		exit(1);
	}

	bzero((char*)&info,sizeof(info));
	info.sin_family=AF_INET;
	info.sin_port=htons(5678);
	info.sin_addr.s_addr=inet_addr(localIP);

	if(connect(sd,(struct sockaddr*)&info,sizeof(info))<0){
		perror("ERROR connection");
		exit(1);
	}
	printf("connection ... OK\n");
	memset(buffer,0,sizeof(buffer));
	if(recv(sd,buffer,sizeof(buffer),0)>0){			//file name
		printf("open file name is : %s\n",buffer);
		strcpy(datatype,buffer);
		memset(buffer,0,sizeof(buffer));
	}
	printf("Open file");
	fp=fopen(datatype,"wb");

	getb=0;
	getpk=0;
	memset(buffer,0,sizeof(buffer));
	while(1){										//file data
		getb=read(sd,buffer,sizeof(buffer));
		if(strcmp(buffer,"end")==0){
			printf("receive end message\n");
			break;
		}
		fwrite(buffer,sizeof(char),getb,fp);
		memset(buffer,0,sizeof(buffer));
		getpk++;
	}

	memset(buffer,0,sizeof(buffer));
	if(recv(sd,buffer,sizeof(buffer),0)>0){			//number of package
		allpk=atof(buffer);
		rate=(allpk-getpk)/allpk;
		printf("get: %f ; all: %f ; rate: %f\n",getpk,allpk,rate);
	}

	fclose(fp);
	close(sd);
	return 0;
}