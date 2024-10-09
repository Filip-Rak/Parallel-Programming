# Zmienne
# Wszystkie z nich dzialaja jak string w c++
# Sa one czesciami polecenia gcc
CCOMP = gcc
LOADER = gcc
OPT = -O3	# Stopien optymalizacji
INC = -I../pomiar_czasu	# Adres do katalogu plikow naglowkowych
LIB = -L../pomiar_czasu -lpomiar_czasu -lm	# Adres do biblioteki


# ---------- Naglowek ----------
# nazwa_naglowka: zaleznosci
# 	polecenie
# Po dwokropku podajesz pliki lub naglowki zalezne.
# Oznacza to, ze jak ten naglowek bedzie sie wykonywac.
# To sprawdzi czy pliki zalezne zostaly zmienione od ostatniej
# kompilacji.
# Jezeli podasz naglowek jako zaleznosc to wtedy make do niego przejdzie
# Jak w konsoli wpiszesz "make" to wtedy wykona sie pierwszy naglowek,
# w tym wypadku "all"
# Mozesz tez kompilowac specyficzny naglowek np. "make fork"
# ------------------------------
all: fork clone

# W tym specyficznym Makefile kompilujemy dwa niezalezne programy,
# fork oraz clone.
# Gdy make bedzie pracowac nad tym naglowkiem to spojrzy sobie na 
# naglowek fork.o
# Patrzac na naglowek fork.o. sprawdzi czy w wskazanym pliku .c lub .h
# zaszly zmiany.
# Jezeli wystapily zmiany wykona polecenie podane linijke niżej
# "$(CCOMP) -c $(OPT) fork.c $(INC)"
# $(CCOMP) -> oznacza uzycie zmiennej CCOMP, ktora jest rowna gcc
# Zamieniajac zmienne na tekst:
# "gcc -c -O3 fork.c -I../pomiar_czasu"
# Jest to standardowe polecenie w gcc i efektem bedzie utworzenie
# pliku .o o nazwie takiej samej jak plik wejsciowy, tj. fork.o
fork: fork.o 
	$(LOADER) $(OPT) fork.o -o fork $(LIB)

# Naglowek for.o jest tutaj ->
fork.o: fork.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) fork.c $(INC) 

clone: clone.o 
	$(LOADER) $(OPT) clone.o -o clone $(LIB)

clone.o: clone.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) clone.c $(INC) 


# Naglowek clean jest do czyszczenia po programie.
# Musisz go wywolac manualnie przez "make clean".
# Nazwa naglowka tj. "clean" jaka tu zastosujesz jest dowolna,
# ale taka jest konwencja
clean:
	rm -f *.o fork clone

# Gdy w konsoli wpiszesz "make", wykonany zostanie pierwszy naglowek.
# W tym pliku jest to "all", ktory dodalem juz sam.
# W wersji od Banasia pierwszy byl fork, ktory kompilowal tylko jeden z programow,
# dlatego dodalem "all"
# Jezeli chcialabys przekompilowac program z inna optymalizacja, bedziesz musiala uzyc:
# "make clean", a pozniej "make", samo uzycie "make" nic nie da jezeli kod sie nie zmieni
# wtedy program powie, ze wszystko jest aktualne


# Ogolnie to Makefile jest do tego, zeby nie trzeba bylo wszystkiego kompilowac za kazdym razem
# Jak masz kilkanascie plikow i zmiany wprowadzisz w jednym z nich to nie ma sensu kompilowac wszystkiego
# Jezeli chodzi o konwencje nazewnictwa to nazywasz tak jak nazwya sie plik jaki polecenie tworzy
# lub opisujesz co robi jak "clean"
# Same polecenia to to samo co jest w konsoli