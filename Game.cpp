#include "Game.hpp"
#include "utils.hpp"
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
Game::Game(game_params gp) : lines(utils::read_lines(gp.filename)),line_width(utils::split(lines[0], ' ').size()),
							 num_of_lines(lines.size()),
							 game_map(num_of_lines),
							 next_game_map(num_of_lines, vector<bool>(line_width)),job_count(0), m_gen_num(gp.n_gen),
							 m_thread_num(std::min(num_of_lines, gp.n_thread)),
							 interactive_on(gp.interactive_on), print_on(gp.print_on){
    job_queue = new PCQueue<Tile>();
	pthread_mutex_init(&tile_hist_lock, nullptr);
}

const vector<double> Game::gen_hist() const {
	return m_gen_hist;
}

const vector<tile_record> Game::tile_hist() const {
	return m_tile_hist;
}

uint Game::thread_num() const {
	return m_thread_num;
}

void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back(
				(double) std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(nullptr);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}

void Game::_init_game() {
	//initialize game_map
	vector<string> cur_line;
	for (uint i = 0; i < num_of_lines; i++) {
		cur_line = utils::split(lines[i], ' ');
		for (uint j = 0; j < line_width; j++)
			game_map[i].push_back((cur_line[j]) == "1");
	}

	//initialize each thread and push it to the threadpool / Create & Start threads
	for (uint i = 0; i < m_thread_num; i++) {
		m_threadpool.push_back(new GameThread(i, &tile_hist_lock, job_queue, &game_map, &next_game_map, line_width,
											  num_of_lines, &m_tile_hist, &job_count));
		m_threadpool[i]->start();
	}

	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	//calculate number of lines each thread should run on
	uint lines_per_thread = num_of_lines / m_thread_num;
	//giving each thread 1 more line to run, instead of giving all the lines to the last one
	int remainder = num_of_lines % m_thread_num;
	// Push jobs to queue
	int run_line = 0;
	for (uint i = 0; i < m_thread_num; i++) {
		Tile job;
		job.beginning_line = run_line;
		if (remainder > 0)
			job.end_line = run_line + lines_per_thread;
		else
			job.end_line = run_line + lines_per_thread - 1;
		run_line = job.end_line + 1;
		job_queue->push(job);
		job_count.up();
		remainder--;
	}
	// Wait for the workers to finish calculating
	while (job_count.getCounter() > 0)
		sched_yield();

	// Swap pointers between current and next field
	game_map = next_game_map;

	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade 
}

void Game::_destroy_game() {
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here
	for (uint i = 0; i < m_thread_num; i++) {
		Tile t;
		t.end_line = -1;
		t.beginning_line = -1;
		job_queue->push(t);
		job_count.up(); //indicate a "destroy tile" was inserted
	}

	//wait for all threads to exit the workload function, using the "destroy tile" provided to them.
	while (job_count.getCounter() > 0)
		sched_yield();

	//join all threads, then destroy them.
	for (uint i = 0; i < m_thread_num; ++i)
		m_threadpool[i]->join();
	for (uint i = 0; i < m_thread_num; ++i)
		delete m_threadpool[i];

	pthread_mutex_destroy(&tile_hist_lock);
	delete job_queue;
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {
	if (print_on) {

		// Clear the screen, to create a running animation 
		if (interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;

		cout << u8"╔" << string(u8"═") * line_width << u8"╗" << endl;
		for (uint i = 0; i < num_of_lines; ++i) {
			cout << u8"║";
			for (uint j = 0; j < line_width; ++j) {
				cout << (game_map[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * line_width << u8"╝" << endl;

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if (interactive_on)
			usleep(GEN_SLEEP_USEC);
	}
}
