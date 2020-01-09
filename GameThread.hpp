#ifndef HW3WET_GAMETHREAD_HPP
#define HW3WET_GAMETHREAD_HPP

#include "Thread.hpp"
#include "Tile.hpp"
#include "PCQueue.hpp"
#include "Game.hpp"
#include "Semaphore.hpp"

struct tile_record;

class GameThread : public Thread{

public:
    GameThread(uint thread_id, pthread_mutex_t* tile_hist_lock, PCQueue<Tile>* job_queue, bool_mat* game_map, bool_mat* next_game_map, int line_width,
            int num_of_lines, vector<tile_record>* m_tile_hist, Semaphore* job_count);
    ~GameThread() override = default; //default, no need to delete anything

private:
    void thread_workload() override;
    /**
     * most fields here are pointers to the fields created in Game, as to allow communication between the producer
     * and the consumers, as well as working in parllel - for example, *next_game_map is a pointer to the next_game_map
     * created in Game, to allow multiple threads to write into it in parallel.
     */
    pthread_mutex_t* tile_hist_lock;
    PCQueue<Tile>* job_queue;
    bool_mat *game_map, *next_game_map;
    const int line_width, num_of_lines;
    vector<tile_record>* m_tile_hist;
    Semaphore* job_count;
};

#endif //HW3WET_GAMETHREAD_HPP
