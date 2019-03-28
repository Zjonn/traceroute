src = $(wildcard *.c)
obj = $(src:.c=.o)

#LDFLAGS = 
CFLAGS = -Wall -Wextra -Werror

traceroute: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean distclean
clean:
	rm  -f $(obj) traceroute

distclean:
	rm traceroute