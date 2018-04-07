object = main

object_main_c = main.c origin.c list.c string_ex.c err.c lock.c alloc.c hmdb.c algorithm.c

all : $(object)
.PHONY : clean

main : $(object_main_c)
	gcc -o main  $(object_main_c) -lpthread -g


clean :
	rm $(object) *.o
