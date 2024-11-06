##
# Wish - A simple Unix shell
#
# @file
# @version 0.1

# source filed
SRCS = wish.c commands.c executor.c path.c error.c

# target executable
TARG = wish

# compiler, compile flags, and needed libs
CC 	 = gcc
OPTS = -Wall -Werror
# LIBS =

# translate .c files in SRCS list to .o's
OBJS =  $(SRCS:.c=.o)

all: $(TARG)

# generate target executable
$(TARG): $(OBJS)
	$(CC) -o $(TARG) -g $(OBJS)

# generic rule for .o files
%.o: %.c
	$(CC) $(OPTS) -c $< -o $@

# perform cleanup
clean:
	rm -f $(OBJS) $(TARG)

# end
