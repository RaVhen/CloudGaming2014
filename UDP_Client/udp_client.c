#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>


int create_udp_client(const char *hostname, int port, struct sockaddr_in* sockname)
{
		
	struct hostent* host_address;
	int optval = 1;
	int socket_id;
	
	
	// obtention de l'adresse IP distante
	if(NULL == (host_address = gethostbyname(hostname))) {
		fprintf(stderr,"machine %s inconnue\n",hostname);
		exit(EXIT_FAILURE);
	}
	
	
	// création d'une socket
	if(-1 == (socket_id = socket(PF_INET, SOCK_DGRAM, 0))) {
		fprintf(stderr,"création socket impossible\n");
		exit(EXIT_FAILURE);
	}
	
	
	// changement paramètres pour réutilisation après fermeture
	setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	
	
	// affectation d'une adresse IP
	memset((char *)sockname, 0, sizeof(struct sockaddr_in));
	sockname->sin_family = AF_INET;
	sockname->sin_port = htons(port);
	sockname->sin_addr.s_addr = *((unsigned long *)(host_address->h_addr_list[0]));
	
	
	return socket_id;
}



int main(int argc, char **argv)
{

	int socket_id;
	char buffer[4096];
	char path[] = "toto.h264";
	int value = 11;
	char* mssg = "coucou";
	struct sockaddr_in sockname;
	int sockaddr_in_length = sizeof(struct sockaddr_in);
	int r;
	socket_id = create_udp_client("localhost", 2000, &sockname);
	
	// génération du message à envoyer
	snprintf(buffer, sizeof(buffer), "%s", mssg);
	for(r = 0; r < 4096; r++)
	  buffer[r] = r%128;
	printf("BUFFER: ");
	for(r = 0; r < 4096; r++)
	  printf("%d_",buffer[r]);
	printf("%d\n",r);
	
	printf("%d\n",sizeof(buffer));
	
	// envoi du message
	if(-1 == (r = sendto(socket_id, buffer, sizeof(buffer), 0, (struct sockaddr *) &sockname, sockaddr_in_length))) {
		fprintf(stderr, "envoi au serveur impossible\n");
		shutdown(socket_id, 2);
		close(socket_id);
		exit(EXIT_FAILURE);
	}
	printf("SEND %d\n",r);
	if(-1 == (r = recvfrom(socket_id, buffer, sizeof(buffer), 0, (struct sockaddr*) &sockname, (socklen_t *)&sockaddr_in_length))) {
		printf("problème de réception\n");
		shutdown(socket_id, 2);
		close(socket_id);
		exit(EXIT_FAILURE);
	}
	printf("RECV %d\n",r);
	buffer[r+1] = '\0';
	int j = 0;
	/*for(j = 0; j<4096;j++){
	  printf("%c ",buffer[j]);
	}
	printf("\n");*/
	//printf("BUFFER: %s %d\n", buffer,(int)strlen(buffer));
	

	FILE *fp;char new_file[]="copied.h264";
	fp=fopen(new_file,"w+");

	while(r > 3){

	//printf("BUFFER: %s %ld\n", buffer,sizeof(buffer));
	int n;
	if(n = fwrite(buffer,1,r,fp)<0)
	  {
	    printf("error writting file\n");
	    exit(1);
	  }
	printf("%d\n",n);
	
	printf("%c\n",buffer[4095]);
	if(-1 == (r = recvfrom(socket_id, buffer, sizeof(buffer), 0, (struct sockaddr*) &sockname, (socklen_t *)&sockaddr_in_length))) {
		fprintf(stderr, "problème de réception\n");
		shutdown(socket_id, 2);
		close(socket_id);
		exit(EXIT_FAILURE);
	}
	printf("RECV %d\n",r);
	printf("%c\n",buffer[4095]);
	buffer[r+1] = '\0';
	}
	fclose(fp);
	
	// attente du message de retour
	
	
	
	// affichage du résultat
	buffer[r+1] = '\0';
	printf("%s\n", buffer);
	//printf("%d * %d = %ld\n", value, value, strtol(buffer, NULL, 10));
	printf("BONJOUR PJ\n");
	shutdown(socket_id, 2);
	close(socket_id);
	
	exit(EXIT_SUCCESS);
	
}
