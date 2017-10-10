# Ändamål: Makefil för användning under inlupp 2 och för att redovisa make-målet
#
# Användning: Istället för att kompilera alla program vi använder varje gång kan man istället
#             lägga in dom här för att göra allt i en veva. Allt man nu behöver skriva är make.

# Variabler
FLAGS = gcc -Wall -Werror -ggdb

# Skapa allt, bara lager så länge
#.SILENT:
all: lager

# Programmet sparas i lager
lager: lager.c tree.o list.o db.o utils.o
	$(FLAGS) $? -o $@

# Skapa .o filer

.c.o:
	$(FLAGS) $< -c

# kolla efter minnesläckage
valgrind: lager
	@valgrind --leak-check=yes ./lager

# Rensa
.PHONY: clean
clean:
	rm -f lager
	rm *.o