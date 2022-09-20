CC = g++

CFLAGS = -std=c++11 -Wpedantic -Werror -Wextra -Wall -Wconversion -w
LFLAGS = -ljack -ljackcpp -lrtmidi -llo


SRCDIR = src
OBJDIR = obj

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(addprefix $(OBJDIR)/,$(notdir $(SRC:.cpp=.o)))

EXEC = OSCSynth

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o $@
					
obj/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $< -c $(CFLAGS) -o $@
					
obj:
	mkdir obj

clean:
	rm $(OBJ)
	rmdir $(OBJDIR)
	rm $(EXEC)
