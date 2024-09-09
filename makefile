
default:
	gcc -std=c99 -Wpedantic \
		-iquote . \
		*.c \
			unicode/*.c

run:
	./a.out
