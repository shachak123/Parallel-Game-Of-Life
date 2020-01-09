#include "Semaphore.hpp"

Semaphore::Semaphore() : counter(0) {
    pthread_mutex_init(&lock, nullptr);
    pthread_cond_init(&cond, nullptr);
}

Semaphore::Semaphore(const unsigned val) : counter(val) {
    pthread_mutex_init(&lock, nullptr);
    pthread_cond_init(&cond, nullptr);
}

Semaphore::~Semaphore(){
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
}

void Semaphore::up(){
    pthread_mutex_lock(&lock);
    counter++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
}

void Semaphore::down(){
    pthread_mutex_lock(&lock);
    while (counter == 0)
        pthread_cond_wait(&cond, &lock);
    counter--;
    pthread_mutex_unlock(&lock);
}

int Semaphore::getCounter() const{
    return counter;
}

