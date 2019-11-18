
WORDBOOKLIB_DIR=../wordbooklib
CWD=`pwd`

OUTPUT=../bin/wordbook-lookup

all: wordbook-lookup

wordbooklib:
	cd ${WORDBOOKLIB_DIR} && make

wordbook-lookup: wordbooklib
	cd ${CWD} && gcc -g -v -Wall -std=gnu99 -o ${OUTPUT} wordbook-lookup.c \
	-L../bin/lib -I../bin/include \
	-lcurl -lwordbook -ljson-c

clean:
	rm -f *.o
	rm -f *.a