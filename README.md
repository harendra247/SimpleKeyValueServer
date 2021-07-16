# SimpleKeyValueServer
A Simple Key-Value Server Based on Thread Pool 

## Introduction
A Key-Value cache server is implemented with C++17 and it handles requests from clients by Socket connection. Server runs as daemon and creates a thread pool driven by a thread-safe work queue. One request is considered as a task added into the work queue. Memory index uses HashMap to maintain the mapping of Key to the metadata of Value, while memory uses LRU algorithm to save the most recently accessed Values as the cache for persistent storage which supports append write mode and loading data to memory index.

## Compile
make


## Usage
./simplekeyvalueserver storefile

The server will be listening on port 9999.

P.S. The default directory macro DEFAULT_FILE_DIR need to be set.


Thanks to ianhua. This code is based on https://github.com/ianhua/SimpleKVServer repository.
