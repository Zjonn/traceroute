src = $(wildcard *.c)
obj = $(src:.c=.o)

#LDFLAGS = 
CFLAGS = -Wall -Wextra -Werror -std=gnu99

traceroute: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean distclean
clean:
	rm  -f $(obj)

distclean:
	rm  -f $(obj) traceroute