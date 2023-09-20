# Makefile for compiling the udp receiver

CC = gcc
CFLAGS = -Wall
LIBS = -lpcap
LDFLAGS = -lssl -lcrypto

all: udp_receiver

udp_receiver: udp_receiver.c
	$(CC) -o udp_receiver udp_receiver.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f udp_receiver
