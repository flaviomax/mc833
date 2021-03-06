# MC833 Projeto 3
# **** Grupo: 
# * Cristina Freitas Bazzano 135368
# * Flavio Altinier Maximiano da Silva 135749

### Graficos que sintetizam o tempo de execucao de servidor locadora com um cliente em LAN
library(ggplot2)
library(reshape)

# Leitura dos tempos de cada operacao no cliente
t1 <- read.table("results/clientTime1.txt", sep = " ", col.names = c("tempoTotal"))
t2 <- read.table("results/clientTime2.txt", sep = " ", col.names = c("tempoTotal"))
t3 <- read.table("results/clientTime3.txt", sep = " ", col.names = c("tempoTotal"))
t4 <- read.table("results/clientTime4.txt", sep = " ", col.names = c("tempoTotal"))
t5 <- read.table("results/clientTime5.txt", sep = " ", col.names = c("tempoTotal"))
t6 <- read.table("results/clientTime6.txt", sep = " ", col.names = c("tempoTotal"))
t7 <- read.table("results/clientTime7.txt", sep = " ", col.names = c("tempoTotal"))

# Tipo de cada uma das 7 operacoes
op_tipo <- factor(c("OperacaoPequena", "OperacaoPequena", "OperacaoPequena", "OperacaoPequena", "OperacaoGrande", "OperacaoEscrita", "OperacaoPequena"), levels = c("OperacaoPequena", "OperacaoGrande", "OperacaoEscrita"))

# Pega o tempo total de cada operacao e calcula a media e o desvio de cada uma
tc_total <- data.frame("1" = t1$tempoTotal/1000000, "2" = t2$tempoTotal/1000000, "3" = t3$tempoTotal/1000000, "4" = t4$tempoTotal/1000000, "5"=t5$tempoTotal/1000000, "6"=t6$tempoTotal/1000000, "7"=t7$tempoTotal/1000000, check.names = FALSE)
top_total <- melt(tc_total)
t_means <- tapply(top_total$value, top_total$variable, mean)
t_sds <- tapply(top_total$value, top_total$variable, sd)*2
# Variavel final com tempo medio total por operacao, desvio padrao de cada uma e o tipo de operacao
top_mean <- data.frame(tempoMedioTotal = t_means, desvio = t_sds, tipo = op_tipo)

# plota um grafico do tempo medio total de coneccao com o servidor por operacao e salva em um png  
png(file = "resultadosTempoTotal.png", bg = "transparent", width = 960, height = 640)
g_total <- ggplot(data=top_mean, aes(x = rownames(top_mean), y = tempoMedioTotal, colour = tipo)) + 
    geom_errorbar(data = top_mean, aes(ymin = tempoMedioTotal - desvio, ymax = tempoMedioTotal + desvio), colour = "black") +
    geom_point(fill = "white", size = 5) +
    labs(title = "Tempo Medio Total de Conexao com o Servidor RMI por Operacao", x = "Operacao", y = "Tempo Medio Total (ms)") +
    theme(text = element_text(size=18))
print(g_total)
dev.off() 

#leitura dos tempos do servidor
st1 <- read.table("results/serverTime1.txt", sep = " ", col.names = "tempoTransmissao")
st2 <- read.table("results/serverTime2.txt", sep = " ", col.names = "tempoTransmissao")
st3 <- read.table("results/serverTime3.txt", sep = " ", col.names = "tempoTransmissao")
st4 <- read.table("results/serverTime4.txt", sep = " ", col.names = "tempoTransmissao")
st5 <- read.table("results/serverTime5.txt", sep = " ", col.names = "tempoTransmissao")
st6 <- read.table("results/serverTime6.txt", sep = " ", col.names = "tempoTransmissao")
st7 <- read.table("results/serverTime7.txt", sep = " ", col.names = "tempoTransmissao")

# pega o tempo de transmissao do servidor e o tempo de transmissao do cliente, subtrai os dois para obter o tempo total de transmissao
ts_transmissao <- data.frame("1" = st1$tempoTransmissao/1000000, "2" = st2$tempoTransmissao/1000000, "3" = st3$tempoTransmissao/1000000, "4" = st4$tempoTransmissao/1000000, "5"=st5$tempoTransmissao/1000000, "6"=st6$tempoTransmissao/1000000, "7"=st7$tempoTransmissao/1000000, check.names = FALSE)
ttrans_total <- (tc_total - ts_transmissao)/2

# Calcula a media e o desvio padrao da transmissao de cada operacao
ttrans_total <- melt(ttrans_total)
ttrans_means <- tapply(ttrans_total$value, ttrans_total$variable, mean)
ttrans_sds <- tapply(ttrans_total$value, ttrans_total$variable, sd)*2
# variavel final com tempo medio de transmissao por operacao, desvio padrao de cada uma e o tipo de operacao
ttrans_total_mean <- data.frame(tempoTransmissao = ttrans_means, desvio = ttrans_sds, tipo = op_tipo)

# plota um grafico do tempo medio de transmissao de dados por operacao e salva em um png  
png(file = "resultadosTempoTransmissao.png", bg = "transparent", width = 960, height = 640)
g_total <- ggplot(data=ttrans_total_mean, aes(x = rownames(ttrans_total_mean), y = tempoTransmissao, colour = tipo)) + 
    geom_errorbar(data = ttrans_total_mean, aes(ymin = tempoTransmissao - desvio, ymax = tempoTransmissao + desvio), colour = "black") +
    geom_point(fill = "white", size = 5) +
    labs(title = "Tempo Medio de Transmissao das Mensagens por Operacao", x = "Operacao", y = "Tempo Medio de Transmissao (ms)") +
    theme(text = element_text(size=18))
print(g_total)
dev.off()  
