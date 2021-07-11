#pragma once

#include "socket.h"
#include "threadpool.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <errno.h>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

using namespace macrometa;
using namespace std;
#define MAX_CACHE_SIZE 1048576

std::string& Trim(std::string& str);

/**
 * Key-Value storage record
 */
struct SimpleKVRecord {

    /* Is valid */
    bool valid;

    /* Key */
    char* key = nullptr;        
    /* nullptr: not in memory, need to read file; NOT nullptr: cached in memory */
    char* value = nullptr;

    /* Offet of value in file */
    off_t offset; 
    /* Length of value */    
    size_t length;

    /* LRU linked list prev pointer */
    SimpleKVRecord* prev = nullptr;
    /* LRU linked list next pointer */
    SimpleKVRecord* next = nullptr;

};

/**
 * Simple KV cache
 * Cache with LRU policy implemented with linked list
 */
class SimpleKVCache {
public:

    /**
     * SimpleKVIndex is friend
     */
    friend class SimpleKVIndex;

    /**
     * Constructor
     */
    SimpleKVCache();

    /**
     * Destructor
     */
    ~SimpleKVCache();

    /**
     * Add item to LRU linked list head and update mem_size
     * @return 0 if success, -1 if fail
     */
    void AddItem(SimpleKVRecord* item);

    /**
     * Remove item from LRU linked list and update mem_size
     * @return 0 if success, -1 if fail
     */
    void RemoveItem(SimpleKVRecord* item);

private:

    int mem_size;

    // Head pointer for LRU linked list
    SimpleKVRecord* head = nullptr;

    // Tail pointer for LRU linked list
    SimpleKVRecord* tail = nullptr;
  
};

/**
 * Simple KV index
 * Maintain a hashmap and a cache for key-value pairs
 */
class SimpleKVIndex {
public:

    /**
     * SimpleKVStore is friend
     */
    friend class SimpleKVStore;

    /**
     * Constructor
     */
    SimpleKVIndex();

    /**
     * Destructor
     */
    ~SimpleKVIndex();

    /**
     * Get value and update LRU linked list
     * @return 0 if success, 1 if key does not exist, -1 if fail
     */
    int Get(const char* key, SimpleKVRecord *&item);

    /**
     * Set key-value and update LRU linked list
     * @return 0 if success, -1 if fail
     */
    int Set(SimpleKVRecord* item);

    /**
     * Delete key and update LRU linked list
     * @return 0 if success, 1 if key does not exist, -1 if fail
     */
    int Delete(const char* key);

    /**
     * Print values of LRU linked list for test
     */
    void PrintCache();

    /**
     * Print statistic string
     */
    std::string Stats();
    
private:

    SimpleKVCache cache;

    int key_count;

    int hit_count;

    int miss_count;

    // HashMap<Key, SimpleKVRecord>
    std::unordered_map<std::string, SimpleKVRecord*> hashmap;

    // Reader-writer lock for hashmap
    std::shared_mutex shared_mutex;
    std::mutex m_mutex;
};

/**
 * Simple KV store by append
 */
class SimpleKVStore {
public:

    /**
     * Constructor
     */
    SimpleKVStore();

    /**
     * Destructor
     */
    ~SimpleKVStore();

    /**
     * Initialize storage file
     * @return 0 if success, -1 if fail
     */
    int Init(const char* file);

    /**
     * Load storage file to index
     * @return 0 if success, -1 if fail
     */
    int Load(SimpleKVIndex& index);

    /**
     * Read record
     * @param record need offset and length, output value when finish
     * @return 0 if success, -1 if fail
     */
    int Read(SimpleKVRecord& record);

    /**
     * Write record
     * @param record need key, value and length, output offset when finish
     * @return 0 if success, -1 if fail
     */
    int Write(SimpleKVRecord& record);

    /**
     * Write delete record
     * @return 0 if success, -1 if fail
     */
    int WriteDelete(const char* key);

    /**
     * Remove deleted records, regenerate storage file
     * @return 0 if success, -1 if fail
     */
    int Compact();

    /**
     * Print statistic string
     */
    std::string Stats();
    
private:

    int fd;
    char* fileName = nullptr;
    std::shared_mutex shared_mutex;

};

/**
 * Simple KV business logic
 *
 * protocal
 * -: request; +: response
 *
 * 1) get
 * -get <key>\n
 * +<value>\n
 *
 * 2) set(override if exists, add if not)
 * -set <key> <value>\n
 * +OK\n
 *
 * 3) delete key-value
 * -delete <key>\n
 * +OK\n
 * 
 * 4) stats
 * -stats\n
 * +count: <key-count>, mem: <mem-size>, file: <file-size>,
 *  hits: <hit-count>, misses: <miss-count>\n
 *
 * 5) quit
 * -quit\n
 * +OK\n
 */

/**
 * Simple KV server
 */
class SimpleKVServer {
public:

    /**
     * Constructor
     */
    SimpleKVServer();

    /**
     * Destructor
     */
    ~SimpleKVServer();

    /**
     * Start server
     * @return 0 if success, -1 if fail
     */
    int Start(const char* ip, unsigned port, const char* file);

private:

    /**
     * Become daemon
     * @return 0 if success, -1 if fail
     */
    int InitDaemon();

    SimpleKVIndex index;

    SimpleKVStore store;

    ServerSocket serverSocket;
    
    ThreadPool *pool = nullptr;

};
