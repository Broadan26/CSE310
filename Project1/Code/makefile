encode_OBJS:= encode.o
decode_OBJS:= decode.o

all: encode	decode

encode:	$(encode_OBJS)
	g++	-Wall	-std=c++11	-o	$@	$^

decode:	$(decode_OBJS)
	g++	-Wall	-std=c++11	-o	$@	$^

%.o: %.cc
	g++	-std=c++11	-c	-c $< -o $@ -lrt -fopenmp -g
	
clean:
	rm	-f	*.o
	rm	-f	encode
	rm	-f	decode
	
.PHONY:	all clean