#include "GameThread.hpp"
#include "Game.hpp"

#define NEED_DESTROY -1

GameThread::GameThread(uint thread_id, pthread_mutex_t* tile_hist_lock, PCQueue<Tile>* job_queue, bool_mat* game_map,
                                    bool_mat* next_game_map,
                                    int line_width, int num_of_lines, vector<tile_record>* m_tile_hist,
                                    Semaphore* job_count) :
                                    Thread(thread_id),tile_hist_lock(tile_hist_lock), job_queue(job_queue), game_map(game_map),
                                    next_game_map(next_game_map), line_width(line_width), num_of_lines(num_of_lines),
                                    m_tile_hist(m_tile_hist), job_count(job_count){

}

void GameThread::thread_workload() {
    while (true) { // run this thread's workload until said otherwise - until _destroy_game is called
        Tile tile = job_queue->pop(); //get the next job
        // this will happen when _destroy_game is called, indicating the thread should stop running
        if (tile.beginning_line == NEED_DESTROY) {
            job_count->down(); //"destroy tile" was taken care of
            break;
        }
        else {
            auto start_time = std::chrono::system_clock::now();
            for (int i = tile.beginning_line; i <= tile.end_line; i++) {
                for (int j = 0; j < line_width; j++) {
                    int neighbors = 0;
                    for (int curRow = i - 1; curRow <= i + 1; curRow++) {
                        for (int curCol = j - 1; curCol <= j + 1; curCol++) {
                            if (curCol == j && curRow == i)
                                continue;
                            if (curCol < 0 || curCol >= line_width || curRow < 0 || curRow >= num_of_lines) //corner cases
                                continue;
                            ((*game_map)[curRow][curCol]) ? neighbors++ : 1;
                        }
                    }
                    (*next_game_map)[i][j] = (neighbors == 3 || (neighbors == 2 && (*game_map)[i][j]));
                }
            }
            auto end_time = std::chrono::system_clock::now();
            tile_record tr;
            tr.thread_id = m_thread_id;
            tr.tile_end_time = end_time;
            tr.tile_start_time = start_time;
            tr.tile_compute_time = (double)
                    std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
            pthread_mutex_lock(tile_hist_lock); //lock, because threads can push to tile_hist in parallel.
            m_tile_hist->push_back(tr);
            pthread_mutex_unlock(tile_hist_lock);
            job_count->down();
        }
    }
}