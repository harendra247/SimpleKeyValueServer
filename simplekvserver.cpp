#include "simplekvserver.h"

using namespace macrometa;
using namespace std;
#define WORK_DIR "home/civilmaps/kv_db"
#define LOG_DIR "home/civilmaps/kv_db/log"

/**
 * Simple Key-Value server
 * @author harendra
 */

// Logger
std::ofstream logger;
/************************Test functions************************/
void TestIndex() {

    // MAX_CACHE_SIZE 64

    SimpleKVIndex index;
    SimpleKVRecord* item1 = new SimpleKVRecord();
    SimpleKVRecord* item2 = new SimpleKVRecord();
    SimpleKVRecord* item3 = new SimpleKVRecord();
    // Set item
    item1->valid = true;
    std::string key1("alpha");
    std::string value1("alpha is testing SimpleKVSvr!");
    item1->key = (char*)malloc(key1.length() + 1);
    strcpy(item1->key, key1.c_str());
    item1->value = (char*)malloc(value1.length() + 1);
    strcpy(item1->value, value1.c_str());
    item1->offset = 0;
    item1->length = value1.length();
    item1->prev = nullptr;
    item1->next = nullptr;

    item2->valid = true;
    std::string key2("beta");
    std::string value2("beta is testing SimpleKVSvr!");
    item2->key = (char*)malloc(key2.length() + 1);
    strcpy(item2->key, key2.c_str());
    item2->value = (char*)malloc(value2.length() + 1);
    strcpy(item2->value, value2.c_str());
    item2->offset = 0;
    item2->length = value2.length();
    item2->prev = nullptr;
    item2->next = nullptr;

    item3->valid = true;
    std::string key3("gamma");
    std::string value3("gamma is testing SimpleKVSvr!");
    item3->key = (char*)malloc(key3.length() + 1);
    strcpy(item3->key, key3.c_str());
    item3->value = (char*)malloc(value3.length() + 1);
    strcpy(item3->value, value3.c_str());
    item3->offset = 0;
    item3->length = value3.length();
    item3->prev = nullptr;
    item3->next = nullptr;

    index.Set(item1);
    index.PrintCache(); // alpha

    index.Set(item2);
    index.PrintCache(); // beta, alpha

    index.Set(item3);
    index.PrintCache(); // gamma, beta

    SimpleKVRecord* item;
    if (index.Get("alpha", item) == 0) {
        if (item->value) {
            std::cout << "Get alpha: " << std::string(item->value) << std::endl;
        } else {
            std::cout << "Get alpha: alpha is in file" << std::endl;
        }
    }

    // As if read from file
    item->value = (char*)malloc(item->length + 1);
    strcpy(item->value, value1.c_str());
    std::cout << "Read from file: " << std::string(item->value) << std::endl;

    index.PrintCache(); // alpha, gamma

    index.Delete("alpha");

    index.PrintCache(); // gamma

    if (index.Get("alpha", item) == 1) {
        std::cout << "alpha does not exist!" << std::endl;
    }

    // key: 2, mem_size: 31, hit: 0, miss: 1
    std::cout << index.Stats() << std::endl;

    free(item1->key);
    free(item1->value);
    delete item1;
    free(item2->key);
    free(item2->value);
    delete item2;
    free(item3->key);
    free(item3->value);
    delete item3;

}

void TestStore() {

    SimpleKVStore store;
    SimpleKVIndex index;

    store.Init("storefile");

    //store.Load(index);

    SimpleKVRecord* item1 = new SimpleKVRecord();
    SimpleKVRecord* item2 = new SimpleKVRecord();
    SimpleKVRecord* item3 = new SimpleKVRecord();
    // Set item
    item1->valid = true;
    std::string key1("alpha");
    std::string value1("alpha is testing SimpleKVSvr!");
    item1->key = (char*)malloc(key1.length() + 1);
    strcpy(item1->key, key1.c_str());
    item1->value = (char*)malloc(value1.length() + 1);
    strcpy(item1->value, value1.c_str());
    item1->offset = 0;
    item1->length = value1.length();
    item1->prev = nullptr;
    item1->next = nullptr;

    item2->valid = true;
    std::string key2("beta");
    std::string value2("beta is testing SimpleKVSvr!");
    item2->key = (char*)malloc(key2.length() + 1);
    strcpy(item2->key, key2.c_str());
    item2->value = (char*)malloc(value2.length() + 1);
    strcpy(item2->value, value2.c_str());
    item2->offset = 0;
    item2->length = value2.length();
    item2->prev = nullptr;
    item2->next = nullptr;

    item3->valid = true;
    std::string key3("beta");
    std::string value3("beta is not testing SimpleKVSvr!");
    item3->key = (char*)malloc(key3.length() + 1);
    strcpy(item3->key, key3.c_str());
    item3->value = (char*)malloc(value3.length() + 1);
    strcpy(item3->value, value3.c_str());
    item3->offset = 0;
    item3->length = value3.length();
    item3->prev = nullptr;
    item3->next = nullptr;

    // Test Write
    store.Write(*item1);

    store.Write(*item2);

    store.Write(*item3);

    store.WriteDelete("alpha");

    // Test Load
    //store.Load(index);

    // key_count: 1
    //std::cout << index.Stats() << std::endl;

    //SimpleKVRecord* item;
    /*if (index.Get("alpha", item) == 1) {
        std::cout << "alpha does not exist!" << std::endl;
    }

    if (index.Get("beta", item) == 0) {
        std::cout << "Get beta: " << std::string(item->value) << std::endl;
    }*/


    // Test Read
    //free(item1->value);
    //item1->value = nullptr;
    //store.Read(*item1);
    //std::cout << "Read from file: " << std::string(item1->value) << std::endl;

    store.Compact();

    // Test Stats
    std::cout << store.Stats() << std::endl;

    free(item1->key);
    free(item1->value);
    delete item1;
    free(item2->key);
    free(item2->value);
    delete item2;
    free(item3->key);
    free(item3->value);
    delete item3;
}

/************************Helper functions************************/
std::string& Trim(std::string& str) {
    const std::string delimters = " \f\n\r\t\v";
    str.erase(str.find_last_not_of(delimters) + 1);
    str.erase(0, str.find_first_not_of(delimters));
    return str;
}

/************************SimpleKVCache***************************/
SimpleKVCache::SimpleKVCache() : mem_size(0), head(nullptr), tail(nullptr) {}

SimpleKVCache::~SimpleKVCache() {}

void SimpleKVCache::AddItem(SimpleKVRecord* item) {
    // Update mem_size
    mem_size += item->length + 1;

    // Add item to LRU linked list head
    SimpleKVRecord* first = head;
    if (first == nullptr) {
        head = item;
        tail = item;
        item->prev = nullptr;
        item->next = nullptr;
    } else {
        head = item;
        item->prev = nullptr;
        item->next = first;
        first->prev = item;
    }

    // Evict if exceed cache size
    SimpleKVRecord* evictItem;
    while (mem_size > MAX_CACHE_SIZE) {
        evictItem = tail;
        RemoveItem(evictItem);
        free(evictItem->value);
        evictItem->value = nullptr;
    }
}

void SimpleKVCache::RemoveItem(SimpleKVRecord* item) {
    // Update mem_size
    mem_size -= item->length + 1;

    // Remove item from LRU linked list
    SimpleKVRecord *prev_item, *next_item;
    if (item->prev == nullptr && item->next == nullptr) {
        // Only one item in linked list
        head = nullptr;
        tail = nullptr;
    } else if (item->prev == nullptr && item->next) {
        // The first item in linked list
        next_item = item->next;
        next_item->prev = nullptr;
        head = next_item;
        item->next = nullptr;
    } else if (item->prev && item->next == nullptr) {
        // The last item in linked list
        prev_item = item->prev;
        prev_item->next = nullptr;
        tail = prev_item;
        item->prev = nullptr;
    } else {
        // In the middle of linked list
        prev_item = item->prev;
        next_item = item->next;
        prev_item->next = next_item;
        next_item->prev = prev_item;
        item->prev = nullptr;
        item->next = nullptr;
    }
}

/************************SimpleKVIndex***************************/

SimpleKVIndex::SimpleKVIndex() : key_count(0), hit_count(0), miss_count(0) { }

SimpleKVIndex::~SimpleKVIndex() { }

int SimpleKVIndex::Get(const char* key, SimpleKVRecord *&item) {
    // Lock rdlock
    std::shared_lock<std::shared_mutex> g(shared_mutex);

    std::string key_str(key);
    auto iter = hashmap.find(key_str);

    if (iter != hashmap.end()) {
        // Key exists
        item = iter->second;

        // Lock mutex
	const std::lock_guard<std::mutex> lock(m_mutex);

        if (item->value) {
            // Value is in memory, cache hit
            hit_count++;
            cache.RemoveItem(item);
        } else {
            // Value is in file, cache miss
            miss_count++;
        }
	cache.AddItem(item);
    } else {
        // Key does not exist
        return 1;
    }

    return 0;
}

int SimpleKVIndex::Set(SimpleKVRecord* item) {
    // Lock rdlock
    std::lock_guard<std::shared_mutex> g(shared_mutex);

    std::string key_str(item->key);
    auto iter = hashmap.find(key_str);

    if (iter != hashmap.end()) {
        // Key exists
        SimpleKVRecord* oldItem;
        oldItem = iter->second; 

	const std::lock_guard<std::mutex> lock(m_mutex);

        if (oldItem->value) {
            // Value is in cache
            cache.RemoveItem(oldItem);
            free(oldItem->value);
        }
	free(oldItem->key);
	delete oldItem;
	cache.AddItem(item);

        hashmap[key_str] = item;
    } else {
        // Key does not exist
        // Lock mutex
	const std::lock_guard<std::mutex> lock(m_mutex);
        cache.AddItem(item);

        // Update key_count and hashmap
        key_count++;
        hashmap[key_str] = item;

    }
    return 0;
}

int SimpleKVIndex::Delete(const char* key) {
    // Lock rdlock
	std::lock_guard<std::shared_mutex> g(shared_mutex);

    std::string key_str(key);
    auto iter = hashmap.find(key_str);

    if (iter != hashmap.end()) {
        // Key exists
        SimpleKVRecord* oldItem;
        oldItem = iter->second;

        // Lock mutex
	const std::lock_guard<std::mutex> lock(m_mutex);

        if (oldItem->value) {
            // Value is in memory
            cache.RemoveItem(oldItem);
            free(oldItem->value);
        }
	free(oldItem->key);
	delete oldItem;

        // Delete hashmap item
        key_count--;
        hashmap.erase(key_str);
    } else {
        // Key does not exist
        return 1;
    }

    return 0;
}

void SimpleKVIndex::PrintCache() {
    SimpleKVRecord* iter = cache.head;
    while (iter) {
        std::cout << std::string(iter->value) << " ->" << std::endl;
        iter = iter->next;
    }
    std::cout << std::endl;
}

std::string SimpleKVIndex::Stats() {
    std::ostringstream response;
    response << "+count: <" << key_count << ">, mem: <" << cache.mem_size << 
    ">, hits: <" << hit_count << ">, misses: <" << miss_count << ">, ";
    return response.str();
}

/************************SimpleKVStore***************************/

SimpleKVStore::SimpleKVStore() : fd(0) { }

SimpleKVStore::~SimpleKVStore() {
    close(fd);
}

int SimpleKVStore::Init(const char* file) {
    strcpy(fileName, WORK_DIR);
    strcat(fileName, file);

    if ((fd = open(fileName, O_RDWR | O_APPEND | O_CREAT, 00644)) == -1) {
        return -1;
    }
    return 0;
}

int SimpleKVStore::Load(SimpleKVIndex& index) {
    // Clear hashmap
   index.hashmap.clear();

    // Start from file head
    if (lseek(fd, 0, SEEK_SET) == -1) {
        return -1;
    }
    
    // Keep read until EOF
    while (true) {

        int res;
        SimpleKVRecord* item = new SimpleKVRecord();

        // valid
        bool valid;
        if ((res = read(fd, &valid, sizeof(bool))) == -1) {
            return -1;
        } else if (res == 0) {
            // EOF
            break;
        }
        item->valid = valid;

        // key
        int key_length;
        if (read(fd, &key_length, sizeof(int)) == -1) {
            return -1;
        }
        item->key = (char*)malloc(key_length + 1);
        if (read(fd, item->key, key_length + 1) == -1) {
            return -1;
        }

        // Erase key in hashmap if deleted
        if (valid == false) {
            index.Delete(item->key);
            continue;
        }

        // length
        int value_length;
        if (read(fd, &value_length, sizeof(int)) == -1) {
            return -1;
        }
        item->length = value_length;

        // offset
        item->offset = lseek(fd, 0, SEEK_CUR);

        // value
        item->value = (char*)malloc(value_length + 1);
        if (read(fd, item->value, value_length + 1) == -1) {
            return -1;
        }

        // Set new item
        index.Set(item);
    }

    return 0;
}

int SimpleKVStore::Read(SimpleKVRecord& record) {
    // Lock rdlock
    std::shared_lock<std::shared_mutex> g(shared_mutex);

    // Set offset to value
    int pos = record.offset;
    if (lseek(fd, pos, SEEK_SET) == -1) {
        return -1;
    }

    // Read from file to heap
    record.value = (char*)malloc(record.length + 1);
    if (read(fd, record.value, record.length + 1) < 0) {
	    return -1;
    }

    return 0;
}

int SimpleKVStore::Write(SimpleKVRecord& record) {

    // Lock wrlock
    std::lock_guard<std::shared_mutex> g(shared_mutex);

    // Set offset to EOF
    if (lseek(fd, 0, SEEK_END) == -1) {
        return -1;
    }

    bool valid = record.valid;
    int key_length = strlen(record.key);
    int value_length = record.length;
    
    // Write five fields separately
    if (write(fd, &valid, sizeof(bool)) == -1) {
        return -1;
    }
    if (write(fd, &key_length, sizeof(int)) == -1) {
        return -1;
    }
    if (write(fd, record.key, key_length + 1) == -1) {
        return -1;
    }
    if (write(fd, &value_length, sizeof(int)) == -1) {
        return -1;
    }
    // Set offset of record
    off_t pos;
    if ((pos = lseek(fd, 0, SEEK_CUR)) == -1) {
        return -1;
    }
    record.offset = pos;

    if (write(fd, record.value, value_length + 1) == -1) {
        return -1;
    }

    return 0;
}

int SimpleKVStore::WriteDelete(const char* key) {
    // Lock wrlock
    std::lock_guard<std::shared_mutex> g(shared_mutex);

    // Set offset to EOF
    if (lseek(fd, 0, SEEK_END) == -1) {
        return -1;
    }

    bool valid = false;
    int key_length = strlen(key);

    // Write three fields separately
    if (write(fd, &valid, sizeof(bool)) == -1) {
        return -1;
    }
    if (write(fd, &key_length, sizeof(int)) == -1) {
        return -1;
    }
    if (write(fd, key, key_length + 1) == -1) {
        return -1;
    }

    return 0;
}

int SimpleKVStore::Compact() {
    // Load old file to index
    SimpleKVIndex index;
    if (Load(index) == -1) {
        return -1;
    }

    // Close current fd
    close(fd);

    // Open new fd to overwrite the file
    fd = open(fileName, O_RDWR | O_TRUNC, 00644);

    // Traverse hashmap to write file
    auto iter = index.hashmap.begin();
    while (iter != index.hashmap.end()) {
        Write(*(iter->second));
        iter++;
    }

    return 0;
}

std::string SimpleKVStore::Stats() {
    // Lock rdlock
    std::shared_lock<std::shared_mutex> g(shared_mutex);

    struct stat buf;
    int file_size;

    if (fstat(fd, &buf) == -1) {
        file_size = -1;
    } else {
        file_size = buf.st_size;
    }

    std::ostringstream response;
    response << "file: <" << file_size << ">\n";
    return response.str();
}


/************************SimpleKVServer***************************/
SimpleKVServer::SimpleKVServer() {}

SimpleKVServer::~SimpleKVServer() {}

int SimpleKVServer::Start(const char* ip, unsigned port, const char* file) {
    // Become daemon
    if (InitDaemon() != 0) {
        std::cout << "Become daemon error!" << std::endl;
        return -1;
    }

    // Open log file and overwrite it for every start
    logger.open(LOG_DIR, std::ofstream::out | std::ofstream::trunc);

    logger << "Server start!" << std::endl;

    // Listen port
    if (serverSocket.Listen(ip, port) != 0) {
        logger << "Listen error!" << std::endl;
        return -1;
    }

    logger << "Listening on " << ip << ":" << port << std::endl;
    
    // Initialize store
    if (store.Init(file) != 0) {
        logger << "Initialize store error!" << std::endl;
        return -1;
    }
    
    // Initialize index
    if (store.Load(index) != 0) {
        logger << "Initialize index error!" << std::endl;
        return -1;
    }
    
    // Start thread pool
    pool = new ThreadPool(4);
    int i = -1;
    // Let thread pool to handle works from clients
    while (true) {

        // Accept connection from client
        ClientSocket* serverConnector = serverSocket.Accept();
        if (serverConnector == nullptr) {
            logger << "Accept error: " << std::endl;
            return -1;
        }
        
        // Add to work queue
	i++;
	pool->enqueue([i](SimpleKVIndex &index, SimpleKVStore& store, ClientSocket*& serverConnector){
		    std::string request, response = "Quit!\n";

		    // Keep doing work for the client until quit
		    while (true) {

			// Read request from client
			if (serverConnector->ReadLine(request) == -1) {
			    response = "+Read request error!\n";
			    serverConnector->WriteLine(response);
			    continue;
			}

			// Trim request
			if (!request.empty()) {
			    request = Trim(request);
			}

			if (request.empty()) {
			    response = "+Empty string!\n";
			    serverConnector->WriteLine(response);
			    continue;
			}

			// Resolve request
			if (request[0] != '-') {
			    response = "+Request format error!\n";
			    serverConnector->WriteLine(response);
			    continue;
			}

			auto cmdpos = request.find_first_of(" ", 1);
			size_t start, end;
			auto len = request.length();
			std::string cmd, key, value;

			if (cmdpos != std::string::npos) {
			    cmd = request.substr(1, cmdpos - 1);
			    if (cmd == "get") {

				// Resolve key
				start = request.find_first_of("<", cmdpos + 1);
				if (start == std::string::npos) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}
				end = request.find_first_of(">", start + 1);
				if (end == std::string::npos) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}
				key = request.substr(start + 1, end - start - 1);
				if (end + 1 != len) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}

				// Get item from hashmap and update cache
				SimpleKVRecord* item;
				int res;
				if ((res = index.Get(key.c_str(), item)) == 0) {
				    // Key exists
				    if (item->value) {
					// Value is in memory
					value = std::string(item->value);
				    } else {
					// Value is in file
					if (store.Read(*item) != 0) {
					    response = "Read item error!\n";
					    serverConnector->WriteLine(response);
					    continue;
					}
					value = std::string(item->value);
				    }
				    response = "+<" + value + ">\n";
				} else if (res == 1) {
				    // key does not exist
				    response = "+<" + key + "> does not exist!\n";
				} else {
				    response = "+get fail!\n";
				}

			    } else if (cmd == "set") {

				// Resolve key and value
				start = request.find_first_of("<", cmdpos + 1);
				if (start == std::string::npos) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}
				end = request.find_first_of(">", start + 1);
				if (end == std::string::npos) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}
				key = request.substr(start + 1, end - start - 1);
				start = request.find_first_of("<", end + 1);
				if (start == std::string::npos) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}
				end = request.find_first_of(">", start + 1);
				if (end == std::string::npos) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}
				value = request.substr(start + 1, end - start - 1);
				if (end + 1 != len) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}

				// Set item members
				SimpleKVRecord* item = new SimpleKVRecord();
				item->valid = true;
				item->length = value.length();
				item->prev = nullptr;
				item->next = nullptr;
				// Allocate item key
				item->key = (char*)malloc(key.length() + 1);
				strcpy(item->key, key.c_str());
				// Allocate item value
				item->value = (char*)malloc(value.length() + 1);
				strcpy(item->value, value.c_str());

				// Append to store first and get offset
				if (store.Write(*item) != 0) {
				    response = "+Write item error!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}

				// Set item to hashmap and update cache
				if (index.Set(item) != 0) {
				    response = "+set fail!\n";
				} else {
				    response = "+OK\n";
				}

			    } else if (cmd == "delete") {

				// Resolve key
				start = request.find_first_of("<", cmdpos + 1);
				if (start == std::string::npos) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}
				end = request.find_first_of(">", start + 1);
				if (end == std::string::npos) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				}
				key = request.substr(start + 1, end - start - 1);
				if (end + 1 != len) {
				    response = "+Unsupported command!\n";
				    serverConnector->WriteLine(response);
				    continue;
				} 

				// Delete key
				int res;
				if ((res = index.Delete(key.c_str())) == 0) {
				    // Append delete record to store
				    if (store.WriteDelete(key.c_str()) != 0) {
					response = "+Write item error!\n";
					serverConnector->WriteLine(response);
					continue;
				    }
				    response = "+OK\n";
				} else if (res == 1) {
				    response = "+<" + key + "> does not exist!\n";
				} else {
				    response = "+delete fail!\n";
				}

			    } else {
				response = "+Unsupported command!\n";
			    }
			} else {
			    if (request.substr(1) == "stats") {

				// stats
				response = index.Stats() + store.Stats();

			    } else if (request.substr(1) == "quit") {

				// quit
				response = "+OK\n";
				serverConnector->WriteLine(response);
				// End while loop
				break;

			    } else {
				response = "+Unsupported command!\n";
			    }
			}

			// Write response to client
			serverConnector->WriteLine(response);
		    }
		}, std::ref(index), std::ref(store), std::ref(serverConnector));

	    return 0;
	}

    store.Compact();

    logger << "Server shutdown!" << std::endl;
    delete pool;
    logger.close();

    return 0;
}

int SimpleKVServer::InitDaemon() {
    pid_t pid;
    int fd0, fd1, fd2;
    struct rlimit rl;
    struct sigaction sa;

    // Clear file mode creation mask
    umask(0);

    // Get maximum number of file descriptors
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        return -1;
    }

    // Fork child process
    if ((pid = fork()) < 0) {
        return -1;
    } else if (pid != 0) {
        exit(0);
    }

    // Become a session leader to lose controlling TTY
    setsid();

    // Ensure future opens won't allocate controlling TTYs
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, nullptr) < 0) {
        return -1;
    }
    if ((pid = fork()) < 0) {
        return -1;
    } else if (pid != 0) {
        exit(0);
    }

    // Change current working directory to the root
    if (chdir("/") < 0) {
        return -1;
    }

    // Close all open file descriptors
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (size_t i = 0; i < rl.rlim_max; i++) {
        close(i);
    }
    
    // Attach file descriptors 0, 1 and 2 to /dev/null
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    
    return 0;
}

int main(int argc, char** argv) {
    /*if (argc < 4) {
        std::cout << "usage: " << argv[0] << " <ip> <port> <file-path>" << std::endl;
        return 1;
    }*/

    //return server.Start(argv[1], (unsigned short)atoi(argv[2]), argv[3]);

    //TestIndex();
    //TestStore();

    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " <file-path>" << std::endl;
        return 1;
    }

    SimpleKVServer server;
    
    const char* ip = "127.0.0.1";

    unsigned short port = 9999;

    if (server.Start(ip, port, argv[1]) != 0) {
        logger << "Simple KV server start fail!" << std::endl;
        return -1;
    }

    return 0;
}
