#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <signal.h>
#include <stddef.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <my_global.h>
#include <mysql.h>
#include <unistd.h>
#define SERV_PORT "35368"
#define MAXBUFLEN 1000000
#define MAX_QUERY_RESULT 1000000
#define FLOAT_LENGTH 9


/** Contador de tempo. A primeira chamada retorna um valor arbitrario;
 * a partir da segunda chamada, ela retorna o tempo decorrido entre
 * esta chamada e a ultima. */
float tempo() {
    static struct timeval ti;
    struct timeval tf;
    float t;
    gettimeofday(&tf, NULL);
    t = tf.tv_sec - ti.tv_sec + (float)(tf.tv_usec - ti.tv_usec)/1E6;
    ti = tf;
    return t;
}

/** Funcao que especifica o erro mySQL */
void finish_with_error(MYSQL *con){
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);      
}

/** get sockaddr, IPv4 or IPv6: */
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

/** Recebe a a mensagem e executa a query */
void deal_with_query (int sockfd){
	MYSQL *con = mysql_init(NULL); // conector mysql
	
	if (con == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}
	
	if (mysql_real_connect(con, "localhost", "root", "crislinda", "MC833", 0, NULL, 0) == NULL) 
	{
		finish_with_error(con);
	}
	/* Neste ponto o conector sql esta pronto e aguardando queries */
	
	
	
	/* Iniciam-se aqui operacoes especificas de redes */
	while( 1 ){
		char buf[MAXBUFLEN], qryRes[MAX_QUERY_RESULT];
		qryRes[0] = 0;
		int numbytes;
		struct sockaddr_storage their_addr;
		socklen_t addr_len;
		float t;
		
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
		printf("Passou do recv\n");
		tempo();
		int qError = 0;
		buf[numbytes] = 0;
		if (mysql_query(con, buf)) { // realizar query
			finish_with_error(con);
			strcat(qryRes, "Server: Erro na query.\n");
			qError = 1;
		}
		else{
			strcat(qryRes, "Operacao realizada com sucesso!\n\n");
		}
		
		if (!qError){
			char operation[7];
			strncpy(operation, buf, 6);
			operation[6] = 0;
		
			if (strcmp(operation, "SELECT") == 0){ // se a query foi um SELECT, deve responder com o resultado
				MYSQL_RES *result = mysql_store_result(con);
				if (result == NULL) 
				{
					finish_with_error(con);
				}
			
				int num_fields = mysql_num_fields(result);

				MYSQL_ROW row;
				MYSQL_FIELD *field;
				
				while ((row = mysql_fetch_row(result))) 
				{ 
					for(int i = 0; i < num_fields; i++) 
					{ 
						if (i == 0) 
						{              
							 while(field = mysql_fetch_field(result)) 
							 {
								strcat(qryRes, field->name);
								strcat(qryRes, " - ");
							 }
							 strcat(qryRes, "\n");           
						  }
						  strcat(qryRes, row[i] ? row[i] : "NULL");
						  strcat(qryRes, " - "); 
					  } 
				  }
			  
				strcat(qryRes, "\n"); // neste ponto qryRes ja guarda o resultado da query SELECT
				mysql_free_result(result);
			}
			
		}
		t = tempo();
		FILE *fp;
		fp = fopen("tempo_serv.txt", "a");
		fprintf(fp, "%f\n", t);
		fclose(fp);
		printf("tempo de operacao: %f\n", t);
		int bytes_sent, len = strlen(qryRes);
		if ((bytes_sent = sendto(sockfd, qryRes, len, 0, (const struct sockaddr *)&their_addr, addr_len)) == -1)
			perror("send");
		printf("bytes_sent = %d, len = %d\n", bytes_sent, len);
	}
	
}

int main(int argc, char **argv) 
{
	int sockfd, rv;
	struct addrinfo hints, *servinfo, *p;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	if ((rv = getaddrinfo(NULL, SERV_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);
	
	/* Servidor esta live e aguardando mensagens */
	printf("Aguardando mensagens!\n");
	
	deal_with_query(sockfd);
	
} 
