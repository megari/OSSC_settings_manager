EXES := main

all: $(EXES)

main: main.o menu.o
	$(CXX) -Wall -std=c++11 -O2 -g main.o menu.o -o main

main.o: main.cpp menu.h
	$(CXX) -Wall -std=c++11 -O2 -c -g main.cpp -o main.o

menu.o: menu.cpp menu.h
	$(CXX) -Wall -std=c++11 -O2 -c -g menu.cpp -o menu.o

.PHONY: clean
clean:
	rm -f *.o $(EXES)
