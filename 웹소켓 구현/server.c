/* 
   A simple server in the internet domain using TCP
   Usage:./server port (E.g. ./server 10000 )
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>  // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h> // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h> // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <netdb.h>      // domain name과 IP address를 변환하는 것 ex)www.google.com <--> 72.14.213.147
#include <arpa/inet.h>  // 인터넷 주소 조작 함수인데.. netinet에 종속되어 있다고는 하나 그래도 넣기
#include <strings.h>
#include <sys/stat.h> // 파일 정보 알고있는 헤더파일
#include <fcntl.h>    // 파일 제어 헤더파일

#define BUF_SIZE 1024

void error(char *msg)
{
   perror(msg);
   exit(1);
}

int main(int argc, char *argv[])
{
   int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
   int portno;            // port number
   socklen_t clilen;

   char buffer[BUF_SIZE];

   /*sockaddr_in: Structure Containing an Internet Address*/
   struct sockaddr_in serv_addr, cli_addr;

   int n;
   if (argc < 2)
   {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(1);
   }

   /*소켓 생성
       AF_INET: Address Domain is Internet 
       SOCK_STREAM: Socket Type is STREAM Socket */
   sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sockfd < 0)
      error("ERROR opening socket"); // 실패 시 에러 반환

   bzero((char *)&serv_addr, sizeof(serv_addr)); // 메모리 공간을 채우는 비표준함수..!
   portno = atoi(argv[1]);                       //atoi converts from String to Integer
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //for the server the IP address is always the address that the server is running on
   serv_addr.sin_port = htons(portno);            //convert from host to network byte order

   /* 주소 정보에 해당하는 IP, port 번호를 socket에 할당 */
   if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
      error("ERROR on binding"); //실패 시 에러 반환

   /* 연결요청 대기상태로 진입 */
   if (listen(sockfd, 5)) == -1) // Listen for socket connections. Backlog queue (connections to wait) is 5
      error("ERROR on listening"); //실패 시 에러 반환

   while (1) // listen까지 성공하고 accept를 위해 무한루프
   {
      clilen = sizeof(cli_addr); // cli_addr 사이즈를 clilen에 저장함으로 accept가 client의 크기를 알게 한다

      /*accept function: 
       1) Block until a new connection is established
       2) the new socket descriptor will be used for subsequent communication with the newly connected client.
     */
      /* 연결을 허용 (대기하고 있다가 수락하기) */
      newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
      if (newsockfd < 0)
         error("ERROR on accept"); // 살패 시 에러 반환

      if (!fork())
      { /* 자식 프로세스 */
         close(sockfd);
         memset(buffer, 0, BUF_SIZE);
         read(newsockfd, buffer, BUF_SIZE - 1);

         printf("%s\n", buffer);

         if (!strncmp(buffer, "GET /favicon.ico, 16"))
         {
            media = open("favicon.ico", O_RDONLY);
            sendfile(newsockfd, media, NULL, 4000);
            close(media);
         };
         if (!strncmp(buffer, "GET /favicon.ico, 16"))
         {
            media = open("favicon.ico", O_RDONLY);
            sendfile(newsockfd, media, NULL, 4000);
            close(media);
         };
         else
            write(newsockfd, 웹페이지, 웹페이지 사이즈 - 1);
      }

      /* 잘가렴 기본예제
      bzero(buffer, 256);
      n = read(newsockfd, buffer, 255); //Read is a block function. It will read at most 255 bytes
      if (n < 0)
         error("ERROR reading from socket");
      printf("Here is the message: %s\n", buffer);

      n = write(newsockfd, "I got your message", 18); //NOTE: write function returns the number of bytes actually sent out �> this might be less than the number you told it to send
      if (n < 0)
         error("ERROR writing to socket");
      */
      /* 부모 프로세스 */
      close(newsockfd);
   }

   return 0;
}
