.SUFFIXES : .c .o

CC = gcc
#CC=$(CROSS_COMPILE)gcc 


TARGET = LibClient

INC = -I.
LDFLAGS = -lpthread -lrt
#CFLAGS  = -O2
#CFLAGS += -Wall
#CFLAGS += -Wpedantic -std=c90

OBJS   = LibMain.o
OBJS  += LibBoard.o
OBJS  += LibClientSocket.o
OBJS  += LibSession.o

CFLAGS += $(INC) 
SRCS = $(OBJS:.o=.c) 

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -lm -o $@ $^ $(LDFLAGS) 

.c.o:
	$(CC) -c $(CFLAGS) $<

dep :
	$(CC) -M $(INC) $(SRCS) > .depend

clean :
	rm -rf $(OBJS) $(TARGET) core
	
distclean: clean
	rm -rf .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif 
