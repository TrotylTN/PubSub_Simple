/*
    Simple udp client
*/
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string>

#define SERVER "128.101.39.191"
#define BUFLEN 512  //Max length of buffer
#define PORT 8954   //The port on which to send data

int UDP_send_packet(const char *packet_content,
                    const char *dest_IP,
                    const unsigned short dest_port) {
  struct sockaddr_in si_other;
  int s; // socket
  int slen=sizeof(si_other);
  if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
      perror("socket error");
      close(s);
      return -1;
  }
  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(dest_port);

  if (inet_aton(dest_IP , &si_other.sin_addr) == 0)
  {
      fprintf(stderr, "inet_aton() failed\n");
      close(s);
      return -1;
  }
  // send the message
  if (sendto(s, packet_content, strlen(packet_content), 0,
             (struct sockaddr *) &si_other, slen)==-1) {
    perror("sendto failed");
    close(s);
    return -1;
  }
  close(s);
  return 0;
}

void die(char *s)
{
    perror(s);
    exit(1);
}


int main(void)
{
  struct sockaddr_in si_other, si_me;
  int s, i, slen=sizeof(si_other);
  char buf[BUFLEN];
  char message[BUFLEN];

  char dest_IP[32];
  int dest_port;


    // if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    // {
    //     die("socket");
    // }
    //
    // memset((char *) &si_other, 0, sizeof(si_other));
    // si_other.sin_family = AF_INET;
    // si_other.sin_port = htons(PORT);
    //
    // if (inet_aton(SERVER , &si_other.sin_addr) == 0)
    // {
    //     fprintf(stderr, "inet_aton() failed\n");
    //     exit(1);
    // }
    // char heartbuf[128] = "heartbeat";
    // close(s);
    //
    // if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    // {
    //     die("socket");
    // }
    // memset((char *) &si_me, 0, sizeof(si_me));
    //
    // si_me.sin_family = AF_INET;
    // si_me.sin_port = htons(5105);
    // si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    //
    // //bind socket to port
    // if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    // {
    //     die("bind");
    // }
    //
    // UDP_send_packet(heartbuf, SERVER, PORT);
    // memset(buf,'\0', BUFLEN);
    // //try to receive some data, this is a blocking call
    // socklen_t socketlen = slen;
    // if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &socketlen) == -1)
    // {
    //   die("recvfrom()");
    // }
    // strncpy(dest_IP, inet_ntoa(si_other.sin_addr), 32);
    // dest_port = ntohs(si_other.sin_port);
    // printf("received \"%s\" from %s:%d\n", buf, dest_IP, dest_port);
    // puts(buf);
    // close(s);


  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
      die("socket");
  }
  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(5105);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind socket to port
  if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
  {
      die("bind");
  }
  i = 10;
  while (1) {
    memset(buf,'\0', BUFLEN);
    //try to receive some data, this is a blocking call
    socklen_t socketlen = slen;
    if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &socketlen) == -1)
    {
      die("recvfrom()");
    }
    strncpy(dest_IP, inet_ntoa(si_other.sin_addr), 32);
    dest_port = ntohs(si_other.sin_port);
    printf("received \"%s\" from %s:%d, replying...\n", buf, dest_IP, dest_port);
    // puts(buf);
    char udpbuf[128];
    strncpy(udpbuf, std::to_string(++i).c_str(),sizeof udpbuf);
    if (buf[0] == 'G')
      UDP_send_packet(udpbuf, SERVER, PORT);
  }

    return 0;
}
