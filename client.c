#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXBUF 1024

int main(int argc, char **argv)
{
    int sockfd, len;
    struct sockaddr_in dest;
    char buffer[MAXBUF + 1];
    if (argc != 3) 
    {
		printf(" error format,it must be:\n\t\t%s IP port\n",argv[0]);
		exit(EXIT_FAILURE);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { //创建socket
		perror("Socket");
		exit(errno);
    }
    printf("socket created\n");


    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));
    if (inet_aton(argv[1], (struct in_addr *) &dest.sin_addr.s_addr) == 0) // 把ip转为二进制网络字节序,并存储在dest中
    {
		perror(argv[1]);	exit(errno);
    }
    if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest))==-1) //连接
    {
		perror("Connect ");
		exit(errno);
    }
    printf("server connected\n");

	pid_t pid;
	if(-1==(pid=fork()))
	{
		perror("fork");exit(EXIT_FAILURE);
	}
	else if (pid==0) //子进程用于接收并打印服务器发来的数据
	{
		while (1) 
		{
			bzero(buffer, MAXBUF + 1);
			len = recv(sockfd, buffer, MAXBUF, 0);
			if (len > 0)
				printf("recv successful:'%s',%d byte recv\n",buffer, len);
			else if(len < 0)
			{
				perror("recv");
				break;
			}
			else
			{
				printf("the other one close ,quit\n");
				break;
			}
		}	
	} 
	else // 父进程用于将客户端的终端的输入数据发送给服务器
	{
		while (1) 
		{
			bzero(buffer, MAXBUF + 1);
			printf("pls send message to send:");
			fgets(buffer, MAXBUF, stdin);
			if (!strncasecmp(buffer, "quit", 4)) 
			{
				printf(" i will quit!\n");
				break;
			}
			len = send(sockfd, buffer, strlen(buffer) - 1, 0); // 这为什么是-1?
			if (len < 0) 
			{
				perror("send");
				break;
			}
		}
	}
    close(sockfd);
    return 0;
}
