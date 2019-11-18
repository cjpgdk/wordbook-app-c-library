all: wordbook-lookup

curlhelper:
	gcc -c -o curlhelper.o ../wordbooklib/curlhelper.c

growablestring:
	gcc -c -o growablestring.o ../wordbooklib/growablestring.c

wordbooklib: curlhelper growablestring
	gcc -c -o wordbooklib.o ../wordbooklib/wordbooklib.c

wordbook-lookup: wordbooklib
	gcc -g -v -Wall -std=gnu99 -o wordbook-lookup -I../wordbooklib/ wordbook-lookup.c wordbooklib.o growablestring.o curlhelper.o \
	-ljson-c -lcurl

clean:
	rm -f *.o