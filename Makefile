CC=g++
CFLAGS=-c -Wall -Wextra
LDFLAGS=-fsanitize=address

SRCS=main.cpp src/custom_alphabet.cpp src/text_processing.cpp src/ucode_char.cpp
OBJS=$(SRCS:.cpp=.o)
EXEC=a.out

all: $(SRCS) $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clear:
	rm -r *.o $(EXEC)
