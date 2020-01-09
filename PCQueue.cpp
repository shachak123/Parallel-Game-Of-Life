#include "PCQueue.hpp"
#include "Tile.hpp"

template <class T> // Add definition in PCQueue.hpp if you need this constructor
PCQueue<T>::PCQueue() : producer_exists_sem(), counter_sem(){
    pthread_mutex_init(&lock, nullptr);
}

template <class T>
PCQueue<T>::~PCQueue(){
    pthread_mutex_destroy(&lock);
}

template <class T>
T PCQueue<T>::pop(){
    while (producer_exists_sem.getCounter() > 0) //if a writer exists or the queue is empty, enter the cond
        sched_yield();
    counter_sem.down();
    pthread_mutex_lock(&lock);
    T ret = (queue.front()); //get the popped item
    queue.pop(); // and pop it
    pthread_mutex_unlock(&lock);
    return ret;
}

template <class T>
void PCQueue<T>::push(const T& item){
    producer_exists_sem.up();
    pthread_mutex_lock(&lock);
    queue.push(item);
    producer_exists_sem.down();
    counter_sem.up();
    pthread_mutex_unlock(&lock);
}

template class PCQueue<Tile>;