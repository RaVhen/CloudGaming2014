#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


int create_udp_server(int port)
{
  int socket_id;
  struct sockaddr_in sockname;
  int optval;
	
	
  // création d'une socket
  if (-1 == (socket_id = socket(PF_INET, SOCK_DGRAM, 0))) {
    fprintf(stderr, "création de socket impossible\n");
    exit(EXIT_FAILURE);
  }
	
  // chargement d'un paramètre de la socket pour la réutiliser après sa fermeture
  optval = 1;
  setsockopt(socket_id,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int));
	
	
  // affectation d'une adresse IP
  memset((char *) &sockname, 0, sizeof(struct sockaddr_in));
  sockname.sin_family = AF_INET;
  sockname.sin_port = htons(port); // ????
  //sockname.sin_addr.s_addr = (htonl(INADDR_ANY));
  sockname.sin_addr.s_addr = (inet_addr("127.0.0.1"));		// localhost
	
  if(0 > bind(socket_id, (struct sockaddr *) &sockname, sizeof(struct sockaddr_in))) {
    fprintf(stderr, "problème avec bind\n");
    exit(EXIT_FAILURE);	
  }


  return socket_id;

}


void print_debug(char* c){
  int i = 0;
  while(c[i] != '\0'){
    printf("%d:%c ",i,c[i]);
    i++;
  }
}

int main(int argc, char **argv)
{
	
  int socket_id;
  char buffer[4096];
  char path[] = "../test.h264";
  int sockaddr_in_length = sizeof(struct sockaddr_in);
  struct sockaddr_in sockname;
  char* mssg;
  int r,numbytes;
  int value, MAXBUFSIZE = 4096;


  FILE* fp;
  if ((fp = fopen(path, "rb")) == NULL) {
    printf("Error opening the file: %s\n", path);
    exit(1);
  }

  socket_id = create_udp_server(2000);
	
  // attente d'un message
  if(-1 == (numbytes = recvfrom(socket_id, buffer, sizeof(buffer) , 0, (struct sockaddr*) &sockname, (socklen_t *)&sockaddr_in_length))) {
    fprintf(stderr,"problème avec recvfrom\n");
    exit(EXIT_FAILURE);
  }
	
  printf("BUFFER: ");
  for(r = 0; r < 4096; r++)
    printf("%d_",buffer[r]);
  printf("%d\n",r);
  printf("RECV %d\n",numbytes);
  
   printf("DEBUT TRANSMISSION\n");
  do {
    int n;
    memset(&buffer[0], 0, sizeof(buffer));
    n = fread(buffer, 1, MAXBUFSIZE , fp);
    printf("%d \n",n);
    //buffer[n] = '\0';
    numbytes = sendto(socket_id, buffer, n, 0,
		      (struct sockaddr *)&sockname, sockaddr_in_length);
    printf("%c\n",buffer[4095]);
    //for(n = 0; n < sizeof(buffer);n++)
    //printf("%d-",buffer[n]);
    printf("server sent %d bytes to client\n", numbytes);

  } while (!feof(fp));
  fclose(fp);

  strcpy(buffer,"end");
  
  // envoi du message
  r = sendto(socket_id, buffer, strlen(buffer), 0, (struct sockaddr*) &sockname, sockaddr_in_length);
  printf("client sent %d bytes to server\n", r);
  

  // génération du message à renvoyer
  /*buffer[r+1] = '\0';
	
  //print_debug(buffer);
  printf("INPUT: %s\n",buffer);
  mssg = buffer;	//value = strtol(buffer, NULL, 10);
  if(!strcmp(mssg,"coucou")){
    mssg = "tu veux voir ma bite";
    printf("MSSG CHANGED: %s\n", mssg);
  }
  strcpy(buffer,mssg);
  //snprintf(buffer, sizeof(buffer), "%d", value* value);
	
	
  // envoi du message
  r = sendto(socket_id, buffer, strlen(buffer), 0, (struct sockaddr*) &sockname, sockaddr_in_length);*/
  shutdown(socket_id, 2);
  close(socket_id);
  exit(EXIT_SUCCESS);	
		
			
}	 

