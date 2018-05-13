object = main

object_main_c = $(wildcard *.c)

all : $(object)
.PHONY : clean

main : $(object_main_c)
	gcc -o main  $(object_main_c) -lpthread -g -rdynamic


clean :
	rm $(object) *.o
