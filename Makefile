src = $(wildcard *.c)
obj = $(src:.c=.o)

#LDFLAGS = 
CFLAGS = -Wall -Wextra -Werror  -g

zroute: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean distclean
clean:
	rm  -f $(obj) zroute

distclean:
	rm zroute