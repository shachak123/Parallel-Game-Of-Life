#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H
#include "Headers.hpp" // OPT - include <pthreads.h> instead

// Synchronization Warm up 
class Semaphore {
public:
	/**
	 * 	Constructs a new semaphore with a counter of 0
	 */
	Semaphore();

	/**
	 * 	Constructs a new semaphore with a counter of val
	 */
	explicit Semaphore(unsigned val);

	/**
	 * 	Destroy the Semaphore - delete the lock and cond.
	 */
    ~Semaphore();

	/**
	 * 	 Mark: 1 Thread has left the critical section - the equivalent of post/signal
	 */
	void up();

	/**
	 * 	Block untill counter >0, and mark - One thread has entered the critical section.
	 * 	this is the equivalent of wait()
	 */
	void down();

	/**
	 * 	get the current counter value.
	 */
	int getCounter() const;

private:
	int counter;
	pthread_mutex_t lock;
	pthread_cond_t cond;
};

#endif
