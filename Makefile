warmup2: main.o my402list.o token.o packet.o server1.o signalHandler.o
	gcc -o warmup2 -g main.o my402list.o token.o packet.o server1.o signalHandler.o -lpthread -lm

main.o: main.c
	gcc -g -c -Wall main.c

signalHandler.o: signalHandler.c
	gcc -g -c -Wall signalHandler.c

server1.o: server1.c
	gcc -g -c -Wall server1.c

packet.o: packet.c
	gcc -g -c -Wall packet.c

token.o: token.c 
	gcc -g -c -Wall token.c

my402list.o: my402list.c
	gcc -g -c -Wall my402list.c

clean:
	rm -f *.o warmup2

