data <- read.csv2("data.csv")

binarySearchTree <- data$Binary.Search.Tree
avl <- data$AVL

plot(binarySearchTree,
     type = "l",
     lwd = 2,
     col = "tomato",
     main = "Distribuição de Comparações na Busca (N = 32768)",
     xlab = "Consultas (Ordenadas do Melhor para o Pior Caso)",
     ylab = "Número de Comparações")

lines(avl,
      col = "steelblue",
      lwd = 2)

legend("topleft", 
       legend = c("Árvore Desbalanceada", "AVL"),
       col = c("tomato", "steelblue"), 
       lty = 1,
       lwd = 2)