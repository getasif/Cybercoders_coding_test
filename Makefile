CC= gcc
CFLAGS= -c -Wall -g
LDFLAGS=
SOURCES= asif_nadaf_longest.c
OBJECTS= $(SOURCES:.c=.o)
EXECUTABLE= asif_nadaf_longest

all:	$(SOURCES) $(EXECUTABLE)
clean:
	rm *o
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
