# Ändamål: Makefil för användning under inlupp 2 och för att redovisa make-målet
#
# Användning: Istället för att kompilera alla program vi använder varje gång kan man istället
#             lägga in dom här för att göra allt i en veva. Allt man nu behöver skriva är make.

# Variabler
FLAGS = gcc -Wall -Werror

# Skapa allt, bara lager så länge
all: lager

# Programmet sparas i lager
lager: lager.c tree.o list.o db.o utils.o
	$(FLAGS) lager.c tree.o list.o db.o utils.o -o lager

# Skapa .o filer

tree.o: tree.c tree.h
	$(FLAGS) tree.c -c

list.o: list.c list.h
	$(FLAGS) list.c -c

db.o:	db.c db.h
	$(FLAGS) db.c -c

utils.o: utils.c utils.h
	 $(FLAGS) utils.c -c

# kolla efter minnesläckage
valgrind: lager
	@valgrind --leak-check=yes ./lager

# Rensa
clean:
	rm -f lager
	rm *.o
