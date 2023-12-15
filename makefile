DOC_DIR=doc
RELEASE=release
INC=inc
LIB_NAME=libeth.a

ifndef debug
debug:=
endif

FLAGS = -std=gnu99 -Wall -Wextra -Wconversion -Wshadow -Wundef -fno-common  -Wno-unused-parameter -Wno-type-limits -fpic
CC = cc $(if $(debug),-DDEBUG -g)
LD = cc

test : test.o eth.o mac.o ipv4.o udp.o tcp.o dump.o
	$(LD) -o test -g $^

test.o : test.c
	$(CC) -c test.c $(FLAGS)

eth.o: eth_defs.h eth.h eth.c
	$(CC) -c eth.c $(FLAGS)

mac.o: mac.h mac.c
	$(CC) -c mac.c $(FLAGS)

ipv4.o: ipv4.h ipv4.c
	$(CC) -c ipv4.c $(FLAGS)

udp.o: udp.c udp.h
	$(CC) -c udp.c $(FLAGS)

tcp.o: tcp.c tcp.h
	$(CC) -c tcp.c $(FLAGS)

dump.o: dump.c dump.h
	$(CC) -c dump.c $(FLAGS)

lib: eth.o mac.o ipv4.o udp.o tcp.o dump.o
	ar rcs $(LIB_NAME) $^ 

valgrind: test
	valgrind --leak-check=full --track-origins=yes ./test	

release: lib
	mkdir -p $(RELEASE)/$(INC)
	cp $(LIB_NAME) $(RELEASE)/.
	cp *.h $(RELEASE)/$(INC)

# rule to update eth lib in network 
# stack tb 
VPI_INC_DIR := ~/rtl/hft/ethernet/tb/vpi/inc
update: release
	cp $(RELEASE)/$(LIB_NAME) $(VPI_INC_DIR)/.
	cp $(RELEASE)/$(INC)/* $(VPI_INC_DIR)/.

clean:
	-rm -f *.o	
	-rm -f *.a	
	-rm -f vgcore.*	
	-rm -f test
	-rm -fr $(RELEASE)
