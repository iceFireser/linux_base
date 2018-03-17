all : search
.PHONY : clean

search : main.o baseAlgorithm.o breadthAlgorithm.o deepAlgorithm.o
	g++  main.o baseAlgorithm.o breadthAlgorithm.o deepAlgorithm.o -std=c++11 -o search

main.o : main.cpp
	g++ -c main.cpp -std=c++11

baseAlgorithm.o : baseAlgorithm.cpp
	g++ -c baseAlgorithm.cpp -std=c++11

breadthAlgorithm.o : breadthAlgorithm.cpp
	g++ -c breadthAlgorithm.cpp -std=c++11

deepAlgorithm.o : deepAlgorithm.cpp
	g++ -c deepAlgorithm.cpp -std=c++11

clean :
	rm search *.o