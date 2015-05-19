#ifndef Mutex_hh
#define Mutex_hh

class Mutex {
public:
    Mutex() {pthread_mutex_init(&_mutex, 0);}
    ~Mutex() {pthread_mutex_destroy(&_mutex);}

    void lock() {pthread_mutex_lock (&_mutex);}
    void unlock() {pthread_mutex_unlock (&_mutex);}

private:
    pthread_mutex_t _mutex;
};

#endif
