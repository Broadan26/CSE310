seaice_OBJS:=	seaice.o

all:	seaice

seaice:	$(seaice_OBJS)
	g++	-Wall	-std=c++11	-o	$@	$^

%.o: %.cc
	g++	-std=c++11	-c	-c $< -o $@ -lrt -fopenmp -g
	
clean:
	rm	-f	*.o
	rm	-f	seaice
	
.PHONY:	all clean