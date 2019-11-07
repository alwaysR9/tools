#include <string>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#include "my_queue.hpp"

struct ThreadParam {
    int id;
    pthread_mutex_t* mu;
    Queue* queue;
    ThreadParam() {}
    ThreadParam(const int id, pthread_mutex_t* mu, Queue* queue) {
        this->id = id;
        this->mu = mu;
        this->queue = queue;
    }
};

struct Pack {
    char* key;
    size_t key_size;
    char* value;
    size_t value_size;
    Pack() {
        key = value = NULL;
        key_size = value_size = 0;
    }
    ~Pack() {
        if (key)
            delete [] key;
        if (value)
            delete [] value;
    }
    void set_key(const void* key, const size_t key_size) {
        this->key = new char[key_size + 5];
        memcpy(this->key, key, key_size);
        this->key_size = key_size;
    }
    void set_value(const void* value, const size_t value_size) {
        this->value = new char[value_size + 5];
        memcpy(this->value, value, value_size);
        this->value_size = value_size;
    }
};

void* produce(void* argv) {
    ThreadParam* param = (ThreadParam*) argv;
    int tid = param->id;
    Queue* queue = param->queue;

    // do produce

    // send fin
    queue->Put(NULL);
}

void* consume(void* argv) {
    ThreadParam* param = (ThreadParam*) argv;
    int tid = param->id;
    pthread_mutex_t* mu = param->mu;
    Queue* queue = param->queue;

    // recv msg from queue
    while (1) {
        Pack* pack = reinterpret_cast<Pack*>(queue->Get());
        if (pack == NULL) {
            break;
        }
	// do task
        delete pack;
    }
    // send fin
    queue->Put(NULL);
}

void create_producer(pthread_t* pid, ThreadParam* param) {
    pthread_create(pid, NULL, produce, param);
}

void create_consumer(pthread_t** pid, ThreadParam* param, const int n_consumer) {
    for (int i = 0; i < n_consumer; ++ i) {
        pthread_create(pid[i], NULL, consume, &(param[i]));
    }
}

void work(int argc, char** argv) {
    int n_consumer = std::stoi(argv[1]);

    // alloc
    Queue* queue = new Queue(1024);
    // mutex for output lqdb
    pthread_mutex_t mu;
    pthread_mutex_init(&mu, NULL);

    // create thread
    pthread_t t_producer;
    pthread_t** pid = new pthread_t*[n_consumer];
    for (int i = 0; i < n_consumer; ++ i)
        pid[i] = new pthread_t;

    ThreadParam p_producer(0, NULL, queue);
    ThreadParam* param = new ThreadParam[n_consumer];
    for (int i = 0; i < n_consumer; ++ i) {
        param[i].id = i;
        param[i].mu = &mu;
        param[i].queue = queue;
    }

    create_producer(&t_producer, &p_producer);
    create_consumer(pid, param, n_consumer);
    _INFO("create threads finish");

    // wait thread
    pthread_join(t_producer, NULL);
    for (int i = 0; i < n_consumer; ++ i)
        pthread_join(*(pid[i]), NULL);

    // delete
    delete [] param;
    for (int i = 0; i < n_consumer; ++ i)
        delete pid[i];
    delete [] pid;
    delete queue;
    // close output lqdb
    pthread_mutex_destroy(&mu);
}

int main(int argc, char** argv) {
    if (argc != 5) {
        _INFO("usage: ./scan_use_single\n\t[input_lqdb path]\n\t[output_lqdb path]\n\t[lqdb_hash_size]\n\t[consumer_num]");
        return 1;
    }

    work(argc, argv);
    return 0;
}
