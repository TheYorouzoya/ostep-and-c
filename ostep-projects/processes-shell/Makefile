##
# Wish - A simple Unix shell
#
# @file
# @version 0.1

# source filed
SRCS = commands.c path.c error.c tokens.c wish.c

# target executable
TARG = wish

# compiler, compile flags, and needed libs
CC 	 = gcc
OPTS = -g -Wall -Werror
# LIBS =

# translate .c files in SRCS list to .o's
OBJS =  $(SRCS:.c=.o)

all: $(TARG)

# generate target executable
$(TARG): $(OBJS)
	$(CC) -o $(TARG) $(OBJS)

# generic rule for .o files
%.o: %.c
	$(CC) $(OPTS) -c $< -o $@

# perform cleanup
clean:
	rm -f $(OBJS) $(TARG)

# end
