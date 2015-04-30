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
#define SERV_PORT "35368"
#define MAXBUFLEN 1000000
#define FLOAT_LENGTH 9
#define MAX_QUERY_SIZE 150

/** Funcao que especifica o erro mySQL */
void finish_with_error(MYSQL *con){
	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);    
	exit(1);  
}

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

/** Pergunta ao usuario que tipo de consulta quer fazer, e retorna
 * a query mySQL correspondente. Retorna por parametro a opcao escolhida*/
char* select_query(char * option){
	static char query[MAX_QUERY_SIZE], adendo[20];
	memset (query, '\0', sizeof(query));
	
	printf ("\n*** Escolha que tipo de operacao deseja realizar: ***\n\n");
	printf ("1. Listar todos os titulos e anos de lancamento\n");
	printf ("2. Listar todos os titulos e anos de lancamento de um genero\n");
	printf ("3. Dado o identificador de um filme, consultar a sinopse\n");
	printf ("4. Dado o identificador de um filme, consultar tudo sobre ele\n");
	printf ("5. Listar tudo de todos os filmes\n");
	printf ("6. Alterar o numero de exemplares em estoque\n");
	printf ("7. Dado o identificador de um filme, retornar o numero de exemplares em estoque\n");
	printf ("\nOperacao: ");
	int op;
	scanf("%d", &op);
	while ( getchar() != '\n' ); // limpar buffer de entrada
	*option = (char)(((int)'0')+op);
	
	char senha[5]; /* uma identificacao simples pra locadora.
	* decidimos por fazer esse identificacao no lado do software cliente
	* mesmo, deixando a rede apenas para as trocas de mensagens especificadas
	* no projeto */
	switch (op){
		case 1:
			strcpy(query, "SELECT titulo, lancamento FROM Locadora");
			break;
			
		case 2:
			strcpy(query, "SELECT titulo, lancamento FROM Locadora WHERE genero=\"");
			printf("Entre com o genero: ");
			scanf("%s", adendo);
			strcat(query, adendo);
			strcat(query, "\"");
			break;
			
		case 3:
			strcpy(query, "SELECT sinopse FROM Locadora WHERE id=");
			printf("Entre com o identificador: ");
			scanf("%s", adendo);
			strcat(query, adendo);
			break;
			
		case 4:
			strcpy(query, "SELECT * FROM Locadora WHERE id=");
			printf("Entre com o identificador: ");
			scanf("%s", adendo);
			strcat(query, adendo);
			break;
			
		case 5:
			strcpy(query, "SELECT * FROM Locadora");
			break;
			
		case 6:
			printf ("Senha: ");
			scanf ("%s", senha);
			while ( getchar() != '\n' );
			if (strcmp(senha, "1234") == 0){
				strcpy(query, "UPDATE Locadora SET exemplares=3 WHERE id=1");
				/* Esta opcao eh hard-coded para que nao haja inconsistencia
				 * entre o BD local e o remoto */
			}
			else{
				printf ("Senha incorreta!\n");
				exit(1);
			}
			break;
			
		case 7:
			strcpy(query, "SELECT exemplares FROM Locadora WHERE id=");
			printf("Entre com o identificador: ");
			scanf("%s", adendo);
			strcat(query, adendo);
			break;
			
		case 8:
			printf("Erro proposital! Caso de testes.\n");
			strcpy(query, "SELECT colunaErrada FROM Locadora");
			break;
			
		default:
			printf ("Opcao nao disponivel!\n");
			exit(1);	
	}
	
	return query;	
}

/** Funcao que envia e recebe mensagens do servidor.
 * Os mensagens enviadas sao na forma de queries SQL, enquanto
 * as respostas sao os resultados dessas queries. sockfd guarda o socket
 * usado na conexao; t1 eh o intervalo de tempo total de conexao, entre o envio
 * da mensagem e recebimento da resposta do servidor.
 * bytes_read retorna por parametro o numero de bytes lidos na mensagem.
 * A funcao retorna o resultado da query com header de sucesso.*/
char* queryToServer (int sockfd, float *t1, char* query, int *bytes_read){
	int len, bytes_sent, total_read = 0;
	len = strlen(query);
	*bytes_read = 0;
    static char buf[MAXBUFLEN];
    static char result[MAXBUFLEN];
    result[0] = 0;
    struct timeval twait; // tempo de espera de resposta
    twait.tv_sec = 3;
    twait.tv_usec = 50000;
    
    tempo(); // inicia o relogio antes de enviar a msg
    
	if ((bytes_sent = send(sockfd, query, len, 0)) == -1)
		perror("send");
		
    // printf("bytes_sent: %d, len: %d\n", bytes_sent, len);
    if ((*bytes_read = recv(sockfd, buf, MAXBUFLEN-1, 0)) < 0)
		perror ("recv");
	
	*t1 = tempo(); // tempo apos receber a resposta do servidor
	buf[*bytes_read] = 0;
	strcat(result, buf);
	total_read = *bytes_read;	
		
	printf("total_read: %d\n", total_read);
	
	//printf("\n\nresult: %s\n\n", result);
		
	return result;
}


/** Aqui o cliente escolhe o modo de acao do programa:
 * 0. 30 vezes é o modo especificado no projeto, executa a comunicação 30 vezes
 * entre cliente e servidor e imprime na saida duas colunas:
 * a primeira o tempo total de comunicação (connect, troca de mensagens, close)
 * e a segunda o tempo aproximado de transmissao. Cada linha da saida eh um experimento.
 * 1. Teste: recebe a mensagem-resposta do servidor e a imprime uma vez,
 * mostrando que a execução está correta. */
int select_modo(){
	int modo = printf("\nEscolha o modo:\n0. 30 vezes\n1. Teste\nModo: ");
    scanf("%d", &modo);
	
	return modo;
	}

/** Este cliente, por razao de teste de corretude, tem acesso a um banco
 * de dados exatamente igual ao do servidor. Quando o cliente entra com
 * uma opção, a resposta "correta" é gerada no cliente mesmo, e depois
 * comparada com a resposta fornecida pelo servidor.
 * Dada a query que sera enviada ao servidor, esta funcao retorna a resposta
 * correta esperada. */
char* generate_correct_answer(char* query){
	MYSQL *con = mysql_init(NULL); // conector mysql
	static char correct_answer[MAXBUFLEN];
	int qError;
	
	if (con == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}
	if (mysql_real_connect(con, "localhost", "root", "crislinda", "MC833", 0, NULL, 0) == NULL) 
	{
		finish_with_error(con);
	}
	if (mysql_query(con, query)) { // realizar query
		finish_with_error(con);
		printf("ATENCAO: Erro na query.\n");
		qError = 1;
	}
	else{
		strcat(correct_answer, "Operacao realizada com sucesso!\n\n");
	}
	
	if (!qError){
		char operation[7];
		strncpy(operation, query, 6);
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
							strcat(correct_answer, field->name);
							strcat(correct_answer, " - ");
						 }
						 strcat(correct_answer, "\n");           
					  }
					  strcat(correct_answer, row[i] ? row[i] : "NULL");
					  strcat(correct_answer, " - "); 
				  } 
			  }
		  
			strcat(correct_answer, "\n"); // neste ponto correct_answer ja guarda o resultado da query SELECT
			mysql_free_result(result);
			mysql_close(con);
		}
		
	}
	
	return correct_answer;

}

int main (int argc, char** argv) {

	struct addrinfo hints, *res;
    int status;
    int sockfd;

    if (argc != 2) {
        fprintf(stderr,"usage: show ip hostname\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_DGRAM;

    if ((status = getaddrinfo(argv[1], SERV_PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }
    
    /* As mensagens enviadas ao servidor sao no formato de queries mySQL */
    char option;
    char* query = select_query(&option);
    char* correct_answer = generate_correct_answer(query);
    
    int modo = select_modo(), repeticoes, k;
    FILE *fp;
    if (modo == 1)
		repeticoes = 1;
	else{ // se for para fazer 30 testes, salva o resultado num arquivo testX.txt, onde X eh o nuemro do teste
		repeticoes = 30;
		char filename[12];
		strcpy(filename, "test");
		strncat(filename, &option, 1);
		strcat(filename, ".txt");
		fp = fopen(filename, "w");
		printf("Salvando resultado em %s...\n", filename);
	}
	
	// realizar os testes
    for (k = 0; k < repeticoes; k++){
		if ( (sockfd = socket (res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
			perror("talker: socket");
            continue;
		}
		
		float t1, tQuery; // serao as contagens de tempo
		int bytes_read;
		
		// conecta com o servidor apenas para armazenar seus dados
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0)
			perror("connect");
		else if (modo == 1)
			printf("Dados do servidor obtidos com sucesso.\n");
			
		char* buf = queryToServer (sockfd, &t1, query, &bytes_read);
		close (sockfd);
		
		if (modo == 1)
			printf("%s\n", buf); // imprime o retorno do servidor
		
		/* Aqui o tempo esta estrutrado do seguinte modo:
		 * t1: o intervalo de tempo entre o inicio da contagem do relogio (antes do "send")
		 * e imediatamente apos o "recv" (apos a resposta do servidor)
		 */
		 
		/* Dessa forma, t1 eh o tempo total da conexao UDP, desde antes do send() ate depois do recv().
		 */
		
		if (modo == 1)
			printf("\ntempo total de conversa (entre send e recv): %f\n", t1);
		else{
			fprintf(fp, "%f %d\n", t1, strcmp(buf, correct_answer));
		}
			
		/* Essa saida para arquivo eh da seguinte forma: a primeira coluna da o tempo total, desde o send()
		 * A terceira da a corretude da resposta recebida pelo servidor: se eh 0, a resposta esta correta.
		 * Qualquer outro valor significa resposta errada (algo da mensagem se alterou durante a transmissao) */
	}
	
	if (modo==0)
		fclose(fp);
    return 0;
    
}
