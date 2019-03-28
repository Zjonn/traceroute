src = $(wildcard *.c)
obj = $(src:.c=.o)

#LDFLAGS = 
CFLAGS = -Wall -Wextra -Werror -DEBUG -g

zroute: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm  -f $(obj) zroute