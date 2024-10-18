# Setting the shell so that /dev/udp device exists
SHELL := /bin/bash

BUILD_TESTS_OPTION := OFF

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
	pushd build && \
	cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=$(BUILD_TESTS_OPTION) .. && \
	make -j12 && \
	popd && \
	popd
	
server: BUILD_TESTS_OPTION=OFF 
server: build
	../build/Release/bin/mmdsrv

test: BUILD_TESTS_OPTION=ON 
test: build
	pushd ../build && \
	ctest -VV && \
	popd

clean: 
	rm -rf ../build

.PHONY: asend bsend usend spam build server test clean