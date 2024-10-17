# Setting the shell so that /dev/udp device exists
SHELL:=/bin/bash

asend: # send data in ASCII
	dd if=./test_ascii.txt > /dev/udp/localhost/7768

bsend: # send data in binary
	dd if=./test_bin.gif > /dev/udp/localhost/7768

usend: # send data in UTF-8
	dd if=./test_utf8.txt > /dev/udp/localhost/7768

spam: # spam binary data with maximal intencity, ^C to stop
	echo "Spamming data; ^C to stop"
	dd if=/dev/random > /dev/udp/localhost/7768

build:
	pushd .. && \
	mkdir -p build && \
	cd build && \
	cmake .. && \
	make -j12 && \
	popd
	
server: build
	../build/mmdsrv

.PHONY: asend bsend usend spam build server