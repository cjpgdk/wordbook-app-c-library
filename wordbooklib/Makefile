all: wordbooklib curlhelper growablestring
	ar ruv libwordbook.a curlhelper.o growablestring.o wordbooklib.o
	ranlib libwordbook.a

curlhelper:
	gcc -c -o curlhelper.o curlhelper.c

growablestring:
	gcc -c -o growablestring.o growablestring.c

wordbooklib:
	gcc -c -o wordbooklib.o wordbooklib.c
	
clean:
	rm -f *.o