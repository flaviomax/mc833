t1 <- read.table("test1.txt", sep = " ", col.names = c("tempoTotal", "tempoTransmissao", "check"))

boxplot(t1$tempoTotal,group = t1$tempoTotal)

m1 <- mean(t1$tempoTotal)
sd1 <- sd(t1$tempoTotal) 
g1 <- ggplot(data=t1, aes(x=1:30, y=tempoTotal)) + 
    geom_errorbar(aes(ymin = m1 - sd1, ymax = m1+sd1)) +
    geom_point()
    
    
    