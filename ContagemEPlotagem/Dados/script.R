mydata <- read.csv2("C:/Users/jadis/Documents/UFAL/data-structures-project/ContagemEPlotagem/Dados/data.csv")

# 1. Plota os pontos da Árvore Desbalanceada (X = Número, Y = BST)
plot(mydata$Number.Searched, mydata$Binary.Search.Tree,
     type = "p",              # "p" significa Pontos (Points)
     pch = 19,                # Usa círculos sólidos e preenchidos
     col = "tomato",          # Cor dos pontos da BST
     main = "Desempenho de Busca: Número vs Comparações",
     xlab = "Número Buscado (Eixo X)",
     ylab = "Quantidade de Comparações (Eixo Y)")

# 2. Adiciona os pontos da AVL no exato mesmo gráfico
points(mydata$Number.Searched, mydata$AVL,
       pch = 19,
       col = "steelblue")     # Cor dos pontos da AVL

# 3. Adiciona a legenda no canto superior direito
legend("topright", 
       legend = c("BST Desbalanceada", "Árvore AVL"),
       col = c("tomato", "steelblue"), 
       pch = 19)

