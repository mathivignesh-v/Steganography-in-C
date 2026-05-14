# Executable name
stego: main.o encode.o decode.o
	gcc -o stego main.o encode.o decode.o

# Compile main file
main.o: main.c
	gcc -c main.c

# Compile encode file
encode.o: encode.c
	gcc -c encode.c

# Compile decode file
decode.o: decode.c
	gcc -c decode.c

# Clean command
clean:
	rm -f *.o stegoax