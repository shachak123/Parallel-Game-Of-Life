#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
#include "Semaphore.hpp"

// Single Producer - Multiple Consumer queue
template <typename T>class PCQueue //OPT - we may know the Type stored in PCQueue, so we can move the functions to .cpp
{
public:
	/**
     * initializes the lock and the cond, as well as the Semaphores.
     */
	PCQueue();

	/**
	 * destroy lock and cond.
	 */
	~PCQueue();

	/** Blocks while queue is empty. When queue holds items, allows for a single
	 * thread to enter and remove an item from the front of the queue and return it.
	 * Assumes multiple consumers.
	 */
	T pop();

	/** Allows for producer to enter with *minimal delay* and push items to back of the queue.
	 * Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.
	 * Assumes single producer
	 */
	void push(const T& item);


private:
	/**
	 * in order to allow for minimal delay for producers, we use a semaphore indicating wether a producer is currently
	 * producing items to the queue. if a producer
	 */
	std::queue<T> queue;
	pthread_mutex_t lock;
	Semaphore producer_exists_sem;
	Semaphore counter_sem;
};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif