CC = gcc
CFLAGS = -Wall -pedantic -lcurl -lcjson

all: watt_a_shine

clean:
	rm -f watt_a_shine *.o*

run: watt_a_shine
	./watt_a_shine

watt_a_shine: watt_a_shine.o curl.o
	$(CC) $(CFLAGS) -o watt_a_shine watt_a_shine.o curl.o

watt_a_shine.o: watt_a_shine.c curl.h
	$(CC) $(CFLAGS) -c watt_a_shine.c

curl.o: curl.c curl.h
	$(CC) $(CFLAGS) -c curl.c