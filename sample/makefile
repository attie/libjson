all: sample

clean:
	rm -rf sample

run: all
	LD_LIBRARY_PATH=../lib/ ./sample

new: clean
	${MAKE} --no-print-directory all

sample: sample.c ../lib/libjson.so
	gcc -I../ -L../lib/ $(filter %.c,$^) -ljson -g -o $@

.PHONY: always
../lib/libjson.so: always
	make -C ..
