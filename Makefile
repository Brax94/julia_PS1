HEADERS = bitmap.h julia_mpi.h
OBJECTS = bitmap.o julia_mpi.o
TARGET = julia
CC = mpicc 
CFLAGS = -g -Wall

# Ignore unfortunatly named files
.PHONY: all clean default

# Don't remove compiled files in case of a crash
.PRECIOUS: $(TARGET) $(OBJECTS)

# Make a default target for better overview
default: all

# Compile sourcefiles without linking (and recompile files if header files are changed aswell)
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile target with linking
all: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o $(TARGET)

# Remove any wild object and precompiled header files and the program
clean:
	-rm -f *.o
	-rm -f *.gch
	-rm -f $(TARGET)
