storm_OBJS:=	storm.o

all:	storm

storm:	$(storm_OBJS)
	g++	-Wall	-std=c++11	-o	$@	$^

%.o: %.cc
	g++	-std=c++11	-c	-c $< -o $@ -lrt -fopenmp -g
	
clean:
	rm	-f	*.o
	rm	-f	storm
	
.PHONY:	all clean