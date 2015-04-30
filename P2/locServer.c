B#include <stdio.h>
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
#define SERV_PORT "35368"
#define BACKLOG 5
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

/** Recebe a a mensagem e executa a query */
void deal_with_query (int connfd){
	
	char buf[MAXBUFLEN], qryRes[MAX_QUERY_RESULT], qryTime[FLOAT_LENGTH];
	qryRes[0] = 0;
	float t;
	int bytes_read;
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
	
	/* Iniciam-se aqui operacoes epecificas de redes */
	if ((bytes_read = recv(connfd, buf, MAXBUFLEN-1, 0)) == -1){
		perror("recv");
	}
	tempo();
	int qError = 0;
	buf[bytes_read] = 0;
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
			mysql_close(con);
		}
		
	}
	t = tempo();
	FILE *fp;
	fp = fopen("tempo_serv.txt", "a");
	fprintf(fp, "%f\n", t);
	fclose(fp);
	printf("tempo de operacao: %f\n", t);
	int bytes_sent, len = strlen(qryRes);
	if ((bytes_sent = send(connfd, qryRes, len, 0)) == -1)
		perror("send");
	printf("bytes_sent = %d, len = %d\n", bytes_sent, len);
	//printf("bytes_sent: %d, len: %d\n", bytes_sent, len);
	
}

int main(int argc, char **argv) 
{
	int listenfd, connfd;
	struct addrinfo hints, *res;
	struct sockaddr_storage their_addr;
    socklen_t addr_size;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(NULL, SERV_PORT, &hints, &res);
	if ((listenfd = socket (res->ai_family, res->ai_socktype, res->ai_protocol)) < 0 ){
		perror("listener : creation");
		exit(1);
	}
	if (bind(listenfd, res->ai_addr, res->ai_addrlen) < 0){
		perror("listener : bind");
		exit(2);
	}
	if (listen(listenfd, BACKLOG) < 0){
		perror("listener : listen");
		exit(3);
	}
	
	/* Servidor esta live e aguardando conexoes */
	
	for ( ; ; ){
		addr_size = sizeof their_addr;
		if ((connfd = accept(listenfd, (struct sockaddr *)&their_addr, &addr_size)) < 0){
			perror("accept");
			exit(4);
		}
		if ((childpid = fork()) == 0){ /* child */
			close(listenfd);
			printf ("Conectou!\n");
			deal_with_query(connfd);
			exit(0);
		}
		close (connfd);
	}
} 
