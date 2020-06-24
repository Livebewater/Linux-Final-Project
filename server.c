#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXBUF 1024

int main(int argc, char *argv[])
{
    int pid,pid2;	
    int sockfd, new_fd;
    socklen_t len; // ?
    struct sockaddr_in my_addr, their_addr; //sockaddr_in 是用于ipv4网络通信的
    unsigned int myport, lisnum;
    char buf[MAXBUF + 1];
    int sockets[1000]; // 存放所有连接的客户端
    if (argv[2])
    {
		  myport = atoi(argv[2]); //字符串转为整数
    }
    else
    {
		  myport = 7575; //默认端口为7575
    }

    if (argv[3])
    {
		  lisnum = atoi(argv[3]); //监听队列大小
    }
  	else
    {
      lisnum = 5; //默认监听队列为5
    }
		

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  //创建socket对象
	//参数为ip协议蔟,TCP,自动选择协议
    {
		perror("socket");
		exit(EXIT_FAILURE);
    }
    
    bzero(&my_addr, sizeof(my_addr)); //将my_addr各字节置为0
    my_addr.sin_family = AF_INET; //梳理没这个成员
    my_addr.sin_port = htons(myport); // htons用于把unsigned int 转为大端
    if (argv[1])
		  my_addr.sin_addr.s_addr = inet_addr(argv[1]); // inet_addr 把点分十进制ip转为长整数型
    else
		  my_addr.sin_addr.s_addr = INADDR_ANY; // 指定为不确定地址

    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))== -1) // 绑定ip 端口 
    {
      perror("bind");
      exit(EXIT_FAILURE);
    }

    if (listen(sockfd,lisnum ) == -1) //设置监听
    {
		perror("listen");
		exit(EXIT_FAILURE);
    }
    printf("wait for connect\n");	
    len = sizeof(struct sockaddr);

    while(1)
    {
        if ((new_fd =accept(sockfd, (struct sockaddr *) &their_addr,&len)) == -1)  //设置接收,开始阻塞
        // 第二个参数为新的their_addr 用于存储客户端信息
        {
            perror("accept");
            exit(EXIT_FAILURE);
        } 
        else
            printf("server: got connection from %s, port %d, socket %d\n",inet_ntoa(their_addr.sin_addr),ntohs(their_addr.sin_port), new_fd);
        
        if(-1==(pid=fork()))	// 创建进程
        {
            // 创建进程失败
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if( pid == 0) // 子进程用于接收客户端消息
        {
            //close(sockfd);
            while(1) //父进程用于将终端输入的数据发送到客户端
            {
                bzero(buf, MAXBUF + 1);
                len = recv(new_fd, buf, MAXBUF, 0);
                if (len > 0)
                    printf("message recv successful :'%s',%dByte recv\n",buf, len);
                else if (len < 0)
                {
                    printf("recv failure!errno code is %d,errno message is '%s'\n",
                    errno, strerror(errno));
                    break;
                }
                else
                {
                    printf("the other one close quit\n");
                    break;
                }
            }
            break;

        }
        else 
	      {	
            if((pid2=fork())==-1)
            {
              perror("fork error");
            }
            else if(pid2==0)
            {
                continue;
            }
            else
            {
                bzero(buf, MAXBUF + 1);
                printf("input the message to send:");
                fgets(buf, MAXBUF, stdin); //读取输入
                if (!strncasecmp(buf, "quit", 4)) //如果前4个字符为quit,退出
                {
                    printf("i will close the connect!\n");
                    break;
                }
                len = send(new_fd, buf, strlen(buf) - 1, 0); //读取n-1字节(不包括'\0'),发送到socket
            }
            // while (1) 
            // {
            
            //  

            //     if (len < 0)
            //     {
            //         printf("message'%s' send failure!errno code is %d,errno message is '%s'\n",
            //         buf, errno, strerror(errno)); // ?
            //         break;
            //     }
            // }
        }
    }
    close(new_fd);
	  close(sockfd);
    return 0;
}
