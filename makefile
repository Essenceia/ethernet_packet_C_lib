DOC_DIR=doc
RELEASE=release
INC=inc
LIB_NAME=lib_eth_packet.a

ifndef debug
debug:=
endif

FLAGS = -std=gnu99 -Wall -Wextra -Wconversion -Wshadow -Wundef -fno-common  -Wno-unused-parameter -Wno-type-limits -fpic
CC = cc $(if $(debug),-DDEBUG -g)
LD = cc

test : test.o eth_packet_s.o
	$(LD) -o test -g $^

test.o : test.c
	$(CC) -c test.c $(FLAGS)

eth_packet_s.o: eth_defs.h eth_packet_s.h eth_packet_s.c
	$(CC) -c eth_packet_s.c $(FLAGS)

lib: eth_packet_s.o
	ar rcs $(LIB_NAME) $^ 

release: lib
	mkdir -p $(RELEASE)/$(INC)
	cp $(LIB_NAME) $(RELEASE)/.
	cp *.h $(RELEASE)/$(INC)

clean:
	-rm -f *.o	
	-rm -f *.a	
	-rm -f test
	-rm -fr $(RELEASE)
