# Ändamål: Makefil för användning under inlupp 2 och för att redovisa make-målet
#
# Användning: Istället för att kompilera alla program vi använder varje gång kan man istället
#             lägga in dom här för att göra allt i en veva. Allt man nu behöver skriva är make.

all:
	gcc db.c tree.c list.c utils.c -g -Wall
