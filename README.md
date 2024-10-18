# UDP port logger

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
cd scripts && make tests
```

### or

```bash
cd ./devcontainer && docker-compose run tests
```

## Command line usage

```
-h, --help         Print usage  
-a, --address arg  Set server address, IPv4 or IPv6 (default: 0.0.0.0)  
-p, --port arg     Set server port (default: 7768)  
-f, --filter arg   Set filter string (default: *)  
-o, --output arg   Set output file name (default: ./mmdsrv.log)  
```