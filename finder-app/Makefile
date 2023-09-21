
cc=$(CROSS_COMPILE)gcc
CFLAGS= -I.
all: writer.c
	$(cc) -o writer $<
.PHONY: clean
clean:
	 rm -rf writer *.o	