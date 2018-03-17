all : pthread_lock
.PHONY : clean

pthread_lock : pthread_lock.o
	gcc pthread_lock.o -o pthread_lock -D_REENTRANT -lpthread

pthread_lock.o : pthread_lock.c
	gcc -c pthread_lock.c

clean :
	rm pthread_lock *.o