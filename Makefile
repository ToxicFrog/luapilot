OBJS=init.o lpi_util.o lpi_log.o lpi_time.o lpi_process.o lpi_names.o lpi_abort.o lpi_channel.o lpi_write.o lpi_read.o

all: pilot.so lpilot

pilot.so: $(OBJS)
	mpicc -shared -Wall -g -o $@ $^ -lpilot -llua

lpilot: $(OBJS) lpilot.o
	mpicc -Wall -g -o $@ $^ -lpilot -llua

%.o: %.c
	mpicc -c -Wall -g -std=c99 -o $@ $^
