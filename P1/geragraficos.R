t1 <- read.table("results/test1.txt", sep = " ", col.names = c("tempoTotal", "tempoTransmissao", "check"))
t2 <- read.table("results/test2.txt", sep = " ", col.names = c("tempoTotal", "tempoTransmissao", "check"))
t3 <- read.table("results/test3.txt", sep = " ", col.names = c("tempoTotal", "tempoTransmissao", "check"))
t4 <- read.table("results/test4.txt", sep = " ", col.names = c("tempoTotal", "tempoTransmissao", "check"))
t5 <- read.table("results/test5.txt", sep = " ", col.names = c("tempoTotal", "tempoTransmissao", "check"))
t6 <- read.table("results/test6.txt", sep = " ", col.names = c("tempoTotal", "tempoTransmissao", "check"))
t7 <- read.table("results/test7.txt", sep = " ", col.names = c("tempoTotal", "tempoTransmissao", "check"))

t_total <- data.frame(t1 = t1$tempoTotal, t2 = t2$tempoTotal, t3 = t3$tempoTotal, t4 = t4$tempoTotal, t5=t5$tempoTotal, t6=t6$tempoTotal, t7=t7$tempoTotal)

t_t <- melt(t_total)
means <- rep(tapply(t_t$value, t_t$variable, mean), each = 30)
sds <- rep(tapply(t_t$value, t_t$variable, sd)*2, each = 30)
t_t <- cbind(t_t, means, sds)

g1 <- ggplot(data=t_t, aes(x=variable, y=value)) + 
    geom_errorbar(aes(ymin = means - sds, ymax = means+sds)) +
    geom_point()
    
    
    