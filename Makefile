all: location.cgi

OBJS=location.o dbcommand.o
LIBS=-lfcgi -lmysqlclient
CFLAGS=-o2
location.cgi:$(OBJS)
	gcc -o location.cgi $(OBJS) $(LIBS)
.c.o:
.PHONY:clean

clean:
	rm $(OBJS) *~
