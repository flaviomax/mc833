# MC833 Projeto 2

Comunicação UDP entre cliente e servidor sobre um banco de dados de uma locadora filmes.

Os programas guardam os dados em um BD mySQL, portanto suas dependências devem estar instaladas nas máquinas.

Requisitos:

1 - libmysqlclient-dev


- O banco de dados deve ser populado com as queries no arquivo dump.

COMO COMPILAR:

gcc locServer.c -o locServer  `mysql_config --cflags --libs` -std=gnu99

gcc locClient.c -o locClient  `mysql_config --cflags --libs` -std=gnu99

