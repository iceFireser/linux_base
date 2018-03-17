all : edit serve client
.PHONY : clean

edit : main.o
	gcc -o edit main.o
serve : serve.o
	gcc -o serve serve.o
client : client.o
	gcc -o client client.o

main.o : main.c
	gcc -c main.c
serve.o : serve.c
	gcc -c serve.c
client.o : client.c
	gcc -c client.c


clean :
	rm edit serve client *.o