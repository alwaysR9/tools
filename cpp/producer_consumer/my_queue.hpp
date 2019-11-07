#ifndef __MY_QUEUE_HPP__
#define __MY_QUEUE_HPP__

#include <pthread.h>

struct Queue {
    int _size;
    void** _buff;
    int _head;
    int _tail;

    pthread_mutex_t _mu;
    pthread_cond_t _nonfull;
    pthread_cond_t _nonempty;

    Queue(){}
    Queue(int size) {
        _size = size;
        _buff = new void*[_size];
        _head = _tail = 0;
        pthread_mutex_init(&_mu, NULL);
        pthread_cond_init(&_nonfull, NULL);
        pthread_cond_init(&_nonempty, NULL);
    }
    ~Queue() {
        delete [] _buff;
        pthread_mutex_destroy(&_mu);
        pthread_cond_destroy(&_nonfull);
        pthread_cond_destroy(&_nonempty);
    }

    bool isEmpty() {
        return _tail == _head;
    }

    bool isFull() {
        return (_tail + 1) % _size == _head;
    }

    int Qsize() {
        int size = 0;
        pthread_mutex_lock(&_mu);
        if (_tail >= _head)
            size = _tail - _head;
        else
            size = _size - (_head - _tail);
        pthread_mutex_unlock(&_mu);
        return size;
    }

    void Put(void* pItem) {
        pthread_mutex_lock(&_mu);

        while(isFull()) {
            pthread_cond_wait(&_nonfull, &_mu);
        }

        _buff[_tail] = pItem;
        _tail = (_tail + 1) % _size;

        pthread_cond_signal(&_nonempty);
        pthread_mutex_unlock(&_mu);
    }

    void* Get() {
        pthread_mutex_lock(&_mu);

        while(isEmpty()) {
            pthread_cond_wait(&_nonempty, &_mu);
        }

        void* pItem = _buff[_head];
        _head = (_head + 1) % _size;

        pthread_cond_signal(&_nonfull);
        pthread_mutex_unlock(&_mu);

        return pItem;
    }
};

#endif