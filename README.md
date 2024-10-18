# UDP port logger

[![test](https://github.com/ieroglyph/test-mmd/actions/workflows/test.yml/badge.svg)](https://github.com/ieroglyph/test-mmd/actions/workflows/test.yml)

## Description

Logs data from UDP port to a file.

## Requirements

### either

* A Linux-based OS, WSL will also do fine
* Compiler with C++20 support (or just concepts if not building tests)
* CMake v3.14 or higher

### or 

* docker with docker-compose

## Building

### either

```bash
cd scripts && make build
```

### or

```bash
cd ./devcontainer && docker-compose run build
```

## Running server on default port 7768

### either

```bash
cd scripts && make server
```

### or

```bash
cd ./devcontainer && docker-compose run server
```

## Running tests

### either

```bash
cd scripts && make test
```

### or

```bash
cd ./devcontainer && docker-compose run test
```

## Command line usage

```
-h, --help         Print usage  
-a, --address arg  Set server address, IPv4 or IPv6 (default: 0.0.0.0)  
-p, --port arg     Set server port (default: 7768)  
-f, --filter arg   Set filter string (default: *)  
-o, --output arg   Set output file name (default: ./mmdsrv.log)  
```

## Manual testing

To test the running server, some test data and scripts are provided.

### Data logging testing

The sample test data is located in the ./scripts, 
and, if the server is running on the default port,
sample scripts can be run using make:

```bash
cd scripts
make asend # Sends data from test_ascii.exe, an ASCII encoded text file 
make usend # Sends data from test_utf8.exe, an UTF-8 encoded text file 
make bsend # Sends data from test_bin.gif, a binary file 
```

### Naive stress testing

For an imitation of a stess testing,
also an script is prepared.
It will brutforce the random data to a port "file".
Works only with `bash`, though.

```bash
cd scripts
make spam 
```
