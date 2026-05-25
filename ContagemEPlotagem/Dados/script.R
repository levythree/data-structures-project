mydata <- read.csv2("C:/Users/jadis/Documents/UFAL/data-structures-project/ContagemEPlotagem/Dados/data.csv")

sortedBinarySearchTree <- sort(mydata$Binary.Search.Tree)
sortedAVL <- sort(mydata$AVL)

plot(sortedBinarySearchTree,
     type = "l",
     lwd = 2,
     col = "tomato",
     main = "Big-O da Busca",
     xlab = "Tamanho do Problema",
     ylab = "Número de Comparações")

lines(sortedAVL,
      col = "steelblue",
      lwd = 2)

legend("topleft", 
       legend = c("Árvore Desbalanceada", "AVL"),
       col = c("tomato", "steelblue"), 
       lty = 1,
       lwd = 2)