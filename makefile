CC = gcc
CFLAGS = -MMD -MP
OBJS := OpenBL.o b_lib.o mesh.o
NAME := OpenBL

TESTNAME := test
TESTOBJS := test_driver.o mesh.o

# Link
$(NAME): $(OBJS)
	$(CC) -o $@ $^ -lm

# pull in dependency info
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test
$(TESTNAME): $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $^

# remove compilation products
clean:
	rm -f $(TESTNAME) $(TESTOBJS) $(TESTOBJS:.o=.d) $(NAME) $(OBJS) $(OBJS:.o=.d)
