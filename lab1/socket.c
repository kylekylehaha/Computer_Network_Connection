#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define buffersize 50

int main(int argc , char * argv[]){
	FILE *fp;
	int sockfd,portnu,i,n,filen;
	char buffer[buffersize]={};
	char datatype[buffersize]={};
	portnu=atoi(argv[4]);
	
	

	if(strcmp(argv[1],"tcp")==0){			//protocol->tcp
		sockfd=socket(AF_INET,SOCK_STREAM,0);
		if(sockfd<0)
			printf("ERROR to socket");
		if(strcmp(argv[2],"recv")==0){			//server
			int newsockfd,clilen,timec,five,show,showc,count;
			struct sockaddr_in serv_addr,cli_addr;
			bzero((char*)&serv_addr,sizeof(serv_addr));

			serv_addr.sin_family=AF_INET;
			serv_addr.sin_addr.s_addr=INADDR_ANY;
			serv_addr.sin_port=htons(portnu);

			if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
				printf("ERROR binding");
			listen(sockfd,5);
			clilen=sizeof(cli_addr);
			newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);
			if(newsockfd<0)
				printf("ERROR accepting");
			recv(newsockfd,datatype,sizeof(datatype),0);
			printf("%s\n",datatype);
			printf("receive datatype\n");
			memset(buffer,0,sizeof(buffer));
			recv(newsockfd,buffer,sizeof(buffer),0);
			filen=atoi(buffer);
			printf("filesize is %d\n",filen);
			fp=fopen(datatype,"wb");
			memset(buffer,0,sizeof(buffer));
			timec=(filen/buffersize);
			five=(timec/20)+1;
			show=five;
			showc=0;
			count = 0;
			ssize_t getb;
			while(1){
					memset(buffer,0,sizeof(buffer));
					getb=recv(newsockfd,buffer,sizeof(buffer),0);
					if(strcmp(buffer,"end")==0){
						break;			//end of receive
					}
					fwrite(buffer,1,getb,fp);
					send(newsockfd,"get",3,0);
					count++;
					if(count==show){
						time_t curtime;
						time(&curtime);
						printf("Now process is %d %% ; time is %s\n",showc, ctime(&curtime));
						show+=five;
						showc+=5;
					}
					
			}
			send(newsockfd,"I got your message",18,0);
			fclose(fp);
			close(newsockfd);
			close(sockfd);
			return 0;
		}
		else{
			struct sockaddr_in info;
			char message[buffersize]={};
			bzero((char*)&info,sizeof(info));
			info.sin_family=AF_INET;
			info.sin_addr.s_addr=inet_addr(argv[3]);
			info.sin_port=htons(portnu);
			if(connect(sockfd,(struct sockaddr*)&info,sizeof(info))<0)
				printf("ERROR to connect");
			strcpy(datatype,argv[5]);
			send(sockfd,datatype,sizeof(datatype),0);
			fp=fopen(datatype,"rb");
			fseek(fp,0,SEEK_END);
			filen=ftell(fp);			//get filesize
			printf("%d\n",filen);
			sprintf(buffer,"%d",filen);
			send(sockfd,buffer,sizeof(buffer),0);
			fseek(fp,0,SEEK_SET);
			memset(message,0,sizeof(message));
			memset(buffer,0,sizeof(buffer));
			size_t sendb;
			while(sendb=fread(message,sizeof(char),buffersize,fp)){
				send(sockfd,message,sendb,0);
				memset(message,0,sizeof(message));
				memset(buffer,0,sizeof(buffer));
				recv(sockfd,buffer,sizeof(buffer),0);
				usleep(1);
			}			//end of send
			send(sockfd,"end",3,0);
			memset(buffer,0,sizeof(buffer));
			recv(sockfd,buffer,sizeof(buffer),0);
			printf("%s\n",buffer);
			fclose(fp);
			close(sockfd);
			return 0;
		}
	}
	else{			//protocl->udp
		sockfd=socket(AF_INET,SOCK_DGRAM,0);
		if(sockfd<0)
			printf("ERROR to socket");
		if(strcmp(argv[2],"recv")==0){			//server
			float udp_loss_rate,count,tcount;
			struct sockaddr_in serv_addr,cli_addr;			//create sockfd
			bzero((char*)&serv_addr,sizeof(serv_addr));
			serv_addr.sin_family=AF_INET;
			serv_addr.sin_addr.s_addr=INADDR_ANY;
			serv_addr.sin_port=htons(portnu);
			if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
				printf("ERROR binding");
			socklen_t length;
			length=sizeof(cli_addr);
			recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr*)&cli_addr,&length);
			printf("%s\n",buffer);
			printf("receive datatype\n");
			strcpy(datatype,buffer);			//receive datatype
			memset(buffer,0,sizeof(buffer));		
			fp=fopen(datatype,"wb");
			recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr*)&cli_addr,&length);
			filen=atof(buffer);
			memset(buffer,0,sizeof(buffer));
			printf("file size is %d\n",filen);
			tcount=(filen/buffersize)+1;
			count=0;
			ssize_t getb;
			while(1){
				memset(buffer,0,sizeof(buffer));
				getb=recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr*)&cli_addr,&length);
				
				if(strcmp(buffer,"end")==0){
					udp_loss_rate=(tcount-count)/tcount;
					printf("get %f ; all: %f ; rate: %f \n",count,tcount,udp_loss_rate);
					break;
				}
				count++;
				fwrite(buffer,sizeof(char),getb,fp);
			}
			fclose(fp);
			close(sockfd);	
			return 0;	
		}			
		else{
			int timec,five,show,showc,count,leave;
			struct sockaddr_in info;
			char message[buffersize]={};
			bzero((char*)&info,sizeof(info));
			info.sin_family=AF_INET;
			info.sin_addr.s_addr=inet_addr(argv[3]);
			info.sin_port=htons(portnu);
			strcpy(datatype,argv[5]);
			sendto(sockfd,datatype,sizeof(datatype),0,(struct sockaddr*)&info,sizeof(info));
			printf("send datatype\n");
			fp=fopen(datatype,"rb");
			fseek(fp,0,SEEK_END);
			filen=ftell(fp);
			fseek(fp,0,SEEK_SET);
			printf("file size is %d\n",filen);
			sprintf(buffer,"%d",filen);
			sendto(sockfd,buffer,sizeof(buffer),0,(struct sockaddr*)&info,sizeof(info));
			printf("send filesize\n");
			memset(buffer,0,sizeof(buffer));
			memset(message,0,sizeof(message));
			timec=(filen/buffersize)+1;			//times of send
			printf("time is %d ; bufferisze is %d \n",timec,buffersize);
			leave=filen%buffersize;
			five=(timec/20)+1;
			show=five;
			count=0;
			showc=0;
			size_t sendb;
			int b;
			for(b=timec;b>0;b--){
				memset(message,0,sizeof(message));
				count++;
				if(b==1){
					fread(message,sizeof(char),leave,fp);
					sendto(sockfd,message,leave,0,(struct sockaddr*)&info,sizeof(info));
				}
				else{
					sendb=fread(message,sizeof(char),sizeof(message),fp);
					sendto(sockfd,message,sendb,0,(struct sockaddr*)&info,sizeof(info));
				}
				if(count==show){
					time_t curtime;
					time(&curtime);
					show+=five;
					showc+=5;
					printf("Now process is %d %%; time is %s \n",showc,ctime(&curtime));
				}
			}
			memset(message,0,sizeof(message));
			sendto(sockfd,"end",3,0,(struct sockaddr*)&info,sizeof(info));
			fclose(fp);
			close(sockfd);
			return 0;
		}
	}
}