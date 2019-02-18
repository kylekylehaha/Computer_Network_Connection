//multithread by tcp			-->sending message
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define buffersize 256
#define queue 10
#define localip "127.0.0.1"

void * child(void *);

int sd,i,new_fd[queue],new_len,times=0,num=0,*p_num;
char buffer[buffersize];
char datatype[buffersize];
struct sockaddr_in my_addr,their_addr;
pthread_t accept_thread[queue];


int main(int argc, char* argv[]){

	p_num=&num;
	new_len=sizeof(their_addr);
	strcpy(datatype,argv[1]);

	if((sd=socket(AF_INET,SOCK_STREAM,0))<0){
		printf("ERROR create socket");
		exit(1);
	}

	bzero((char*)&my_addr,sizeof(my_addr));
	my_addr.sin_family=AF_INET;
	my_addr.sin_addr.s_addr=INADDR_ANY;
	my_addr.sin_port=htons(5678);

	if(bind(sd,(struct sockaddr*)&my_addr,sizeof(my_addr))<0){
		printf("ERROR binding");
		exit(1);
	}

	while(1){
		if(listen(sd,10)<0){
			printf("ERROR listen");
			exit(1);
		}

		printf("waiting for connection\n");

		if((new_fd[times]=accept(sd,(struct sockaddr*)&their_addr,&new_len))<0){
			printf("ERROR accepting");
			exit(1);
		}

		pthread_create(&(accept_thread[times]),NULL,child,(void *)p_num);
		printf("create %d pthread\n",times);

		sleep(2);
		num++;
		times++;
	}
//pthread_join()
	close(sd);
	return 0;
}

void * child (void * arg){
	FILE *fp;
	int sendb=0,th_num=0,filen=0,count=0,*p_th_num;
	char buf[buffersize];

	fp=fopen(datatype,"rb");
	fseek(fp,0,SEEK_END);
	filen=ftell(fp);
	fseek(fp,0,SEEK_SET);
	p_th_num=(int *) arg;
	th_num=*p_th_num;
	memset(buf,0,sizeof(buf));
	printf("file name is:%s\n",datatype);
	if(write(new_fd[th_num],datatype,sizeof(datatype))){		//send file name
		printf("success send filename : %s\n",datatype);
	}

	while(sendb=fread(buf,sizeof(char),sizeof(buf),fp)){		//send file data
		send(new_fd[th_num],buf,sendb,0);
		memset(buf,0,sizeof(buf));
		usleep(2);
	}
	printf("send file data done\n");
	send(new_fd[th_num],"end",3,0);								//send end message
	printf("send end message\n");
	memset(buf,0,sizeof(buf));
	count=(filen/buffersize)+1;
	sprintf(buf,"%d",count);
	write(new_fd[th_num],buf,sizeof(buf));
	printf("finish %d thread\n",th_num);
	fclose(fp);
	close(new_fd[th_num]);
	return(NULL);
}