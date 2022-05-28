//client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <linux/if.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SA struct sockaddr

//--------------------------------------------------------------
//--------------------------------------------------------------

char * get_ip_addr()
{
     //create an ifreq struct for passing data in and out of ioctl
        struct ifreq my_struct;
     	char * addr;
     	
        //declare and define the variable containing the name of the interface
        char *interface_name="eth0";   //a very frequent interface name is "eth0";
     
        //the ifreq structure should initially contains the name of the interface to be queried. Which should be copied into the ifr_name field.
        //Since this is a fixed length buffer, one should ensure that the name does not cause an overrun
        size_t interface_name_len=strlen(interface_name);
     
        if(interface_name_len<sizeof(my_struct.ifr_name))
        {
            memcpy(my_struct.ifr_name,interface_name,interface_name_len);
            my_struct.ifr_name[interface_name_len]=0;
        }
        else
        {
            perror("Copy name of interface to ifreq struct");
            printf("The name you provided for the interface is too long...\n");
        }
     
        //provide an open socket descriptor with the address family AF_INET
        /* ***************************************************************
         * All ioctl call needs a file descriptor to act on. In the case of SIOCGIFADDR this must refer to a socket file descriptor. This socket must be in the address family that you wish to obtain (AF_INET for IPv4)
         * ***************************************************************
         */
     
        int file_descriptor=socket(AF_INET, SOCK_DGRAM,0);
     
        if(file_descriptor==-1)
        {
            perror("Socket file descriptor");
            printf("The construction of the socket file descriptor was unsuccessful.\n");
            return 0;
        }
     
        //invoke ioctl() because the socket file descriptor exists and also the struct 'ifreq' exists
        int myioctl_call=ioctl(file_descriptor,SIOCGIFADDR,&my_struct);
     
        if (myioctl_call==-1)
        {
            perror("ioctl");
            printf("Ooops, error when invoking ioctl() system call.\n");
            close(file_descriptor);
            return 0;
        }
     
        close(file_descriptor);
     
        /* **********************************************************************
         * If this completes without error , then the hardware address of the interface should have been returned in the  'my_struct.ifr_addr' which is types as struct sockaddr_in.
         * ***********************************************************************/
     
      //extract the IP Address (IPv4) from the my_struct.ifr_addr which has the type 'ifreq'
     
        /* *** Cast the returned address to a struct 'sockaddr_in' *** */
        struct sockaddr_in * ipaddress= (struct sockaddr_in *)&my_struct.ifr_addr;
       /* *** Extract the 'sin_addr' field from the data type (struct) to obtain a struct 'in_addr' *** */
	
	addr = inet_ntoa(ipaddress->sin_addr);
	

return addr;
}

//--------------------------------------------------------------
//--------------------------------------------------------------

void inscrir(int sock){
	  //la recupperation de @ip
          char * ip = get_ip_addr();
          send (sock, ip, strlen(ip), 0);
          printf("\n your @ip is : %s\n", ip);
          //snprintf(sendBuff, sizeof(sendBuff), "%s\n", ip);
          //Envoyer le contenu de sendBudd avec la socket
          //write(sockfd, sendBuff, sizeof(sendBuff));
}

//--------------------------------------------------------------
//--------------------------------------------------------------

void receivefile()
{	
  int n;
  FILE *fp;
  char *filename = "recv.txt";
  char buffer[48000];

  char *ip = "127.0.0.1";
  int port = 7001;
  int e;

  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
   printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0) {
    perror("[-]Error in bind");
    exit(1);
  }
  printf("[+]Binding successfull.\n");

  if(listen(sockfd, 5) == 0){
		printf("[+]Listening....\n");
	}else{
		perror("[-]Error in listening");
    exit(1);
	}
  printf("[+]accept the connection ....\n");
  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
  printf("[+]aceepted done ....\n");
  
  fp = fopen(filename, "w");
 // while (1) {
    n = recv(new_sock, buffer, 48000, 0);
    if (n <= 0){
     printf(" false in recv  \n");
     // break;
    //  return;
    }
    
    printf("bien recu : %s", buffer);
    fprintf(fp, "%s", buffer);
    bzero(buffer, 48000);
 // }
   printf("[+]Data written in the file successfully.\n");
}

  ///#######################################################*/
 /////#######################################################*/
 /////#######################################################*/
// la fonction main//

int main(int argc, char *argv[])
{

    char *recv = "../src/recv.txt";
    // La socket client
    int sockfd = 0;
    int  n = 0;
    // Le buffer pour recevoir la réponse du serveur
    char recvBuff[1024] = {0};
    char sendBuff[1024] = {0};
    
    // La structure avec les informations du serveur
    struct sockaddr_in serv_addr = {0};
    
    /*
     * Si l'IP du serveur n'a pas été passée en argument
     * le programme se termine
     */
    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }
    
    // Création de la socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    
    serv_addr.sin_family = AF_INET;
    // Le port sur lequel écoute le serveur
    serv_addr.sin_port = htons(5000);
    
    // Copie l'adresse ip du serveur dans la structure serv_addr
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }
    
    // Connection au serveur
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    /////###############*/
    // Récupération du nom de la machine
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    // Lecture des informations envoyées par le serveur sur la socket
    
    	//Envoyer le hostname vers le server
      //  sendBuff[n] = 0;
        //copier le hostname dans notre buffer sendBuff
     //   snprintf(sendBuff, sizeof(sendBuff), "%s\n", hostname);
	//la recupperation de @ip
        //   char * ip = get_ip_addr();
         // printf("\nvotre adresse ip est : %s\n", ip);
         // snprintf(sendBuff, sizeof(sendBuff), "%s\n", ip);
          
        //################################################
          //Envoyer le contenu de sendBudd avec la socket
       // write(sockfd, sendBuff, sizeof(sendBuff));
        printf("\n call the function inscrir ...\n");
        inscrir(sockfd);
         printf("\n call the function recv ...\n");
         
         
        receivefile();
        
        
        
      //  n = read(sockfd, recvBuff, sizeof(recvBuff)-1);
        if( n > 0){
        recvBuff[n] = 0;
        // Affichage des informations recues sur la sortie standard
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
	
         
    }
        //attiendre la connexion
        printf("\n inscription done !!\n");
        
        
        close(sockfd);
    
    return 0;
}
   
