PROJECTS = Huffman SATSolver SMTSolver

DIST_DIR = $(CURDIR)/dist

CFLAGS = -Wall -Wextra -g
LIBS = -lm

.PHONY: all clean $(PROJECTS)

all: create_dist $(PROJECTS)

create_dist:
	@mkdir -p $(DIST_DIR)

$(PROJECTS):
	@echo "-------------------------------------------------"
	@echo " Compilando o projeto: $@..."
	@echo "-------------------------------------------------"
	$(eval SRCS := $(wildcard $@/*.c))
	@if [ -z "$(SRCS)" ]; then \
		echo "Aviso: Nenhum arquivo .c encontrado na pasta $@/. Pulando..."; \
	else \
		gcc $(CFLAGS) $(SRCS) -o $(DIST_DIR)/$@ $(LIBS); \
		echo "Sucesso! $(DIST_DIR)/$@ gerado."; \
	fi

clean:
	@echo "Removendo a pasta de distribuição..."
	@rm -rf $(DIST_DIR)
	@echo "Limpeza concluída!"