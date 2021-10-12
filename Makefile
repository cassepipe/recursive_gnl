CC =		gcc

#CFLAGS = -O2	

#LDFLAGS = -fsanitize=address 


all:	gnl 

gnl:	gnl.o 
		${CC} ${LDFLAGS}  $< -o $@

clean:	
		rm -rf *.o gnl

re :	clean all




