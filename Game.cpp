#include "Game.hpp"
static const char *colors[7] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};
#define MIN(a,b) ((a)<(b)?(a):(b))
/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(nullptr);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}

void Game::_init_game() {

    //init game board
    vector<string> board_in_strings = utils::read_lines(this->filename);
    this->board = new vector<vector<int>*>;
    this->temp_board = new vector<vector<int>*>;
    for(int i=0; i<board_in_strings.size(); i++){
        vector<string> line= utils::split(board_in_strings[i], ' ');
        vector<int> *line_ints = new vector<int>;
        vector<int> *temp_line_ints = new vector<int>;
        for(int j=0; j<line.size(); j++){
            (*line_ints).push_back(std::stoi(line[j]));
            (*temp_line_ints).push_back(std::stoi(line[j]));
        }
        this->board->push_back(line_ints);
        this->temp_board->push_back(temp_line_ints);
    }
    //------------------------------------
    this->m_thread_num=MIN(m_thread_num,this->board->size());

    // Create & Start threads

    for(int i=0; i<m_thread_num;i++){
        CalcThread* thread=new CalcThread(i);
        m_threadpool.push_back(thread);
        thread->game=this;
        thread->start();
    }

	// Testing of your implementation will presume all threads are started here/
	tiles_finished=0;
}

void Game::_step(uint curr_gen) {


    //create tiles (tasks)
    int lines_in_tile;
    lines_in_tile = board->size() / m_thread_num;

    // Push jobs to queue
    vector<Tile*> tmp_queue;

    for(int i=0;i<m_thread_num;i++){
        if(i == m_thread_num-1&& m_thread_num>1){
            Tile* last_tile = new Tile(lines_in_tile*i,board->size()-1);
            last_tile->prev_tile=tmp_queue[i-1];
            last_tile->next_tile= nullptr;
            tmp_queue[i-1]->next_tile= last_tile;
            tmp_queue.push_back(last_tile);
            break;
        }
        Tile* tile = new Tile(lines_in_tile*i,lines_in_tile*(i+1)-1);
        if(i == 0 ){
            tile->prev_tile=nullptr;
        }else{
            tile->prev_tile = tmp_queue[i-1];
            tmp_queue[i-1]->next_tile=tile;
        }
        tmp_queue.push_back(tile);
    }

    tiles_finished=m_thread_num;

    for(int i=0; i<m_thread_num; i++){
        phase_one->push(tmp_queue[i]);
    }

	// Wait for the workers to finish calculating
	while(Completed_tasks->size()!=m_thread_num){
	    sched_yield();
	}

	while(Completed_tasks->size()>0){
	    delete Completed_tasks->pop();
	}


	//to finish game
	if(curr_gen == this->m_gen_num-1){
        this->tiles_finished=m_thread_num;
        for(int i=0 ; i<m_thread_num; i++){
            Tile* game_finished_tile = new Tile(-1,-1);
            game_finished_tile->gane_is_finished=true;
            this->phase_one->push(game_finished_tile);
        }
	}

    //delete tmp_matrix;
	// Swap pointers between current and next field 
	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade
}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources 
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here
	for (uint i = 0; i < m_thread_num; ++i) {
	    m_threadpool[i]->join();
    }

    for (int i = 0; i < this->board->size(); i++) {
        delete (*this->board)[i];
        delete (*this->temp_board)[i];
    }
    delete this->board;
    delete this->temp_board;
    delete this->phase_one;
    delete this->phase_two;
    while(this->Completed_tasks->size()>0){
        delete this->Completed_tasks->pop();
    }
    delete this->Completed_tasks;
    pthread_mutex_destroy(&finished_tiles_m);
    pthread_mutex_destroy(&task_mutex);
}


/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;

		p_board(this->board);

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}

Game::Game(game_params const& params) {
    this->m_gen_num = params.n_gen;
    this->interactive_on = params.interactive_on;
    this->m_thread_num = params.n_thread;
    this->print_on=params.print_on;
    this->filename=params.filename;
    this->phase_one = new PCQueue<Tile*>;
    this->phase_two = new PCQueue<Tile*>;
    this->Completed_tasks=new PCQueue<Tile*>;
    pthread_mutex_init(&this->task_mutex,NULL);
    pthread_mutex_init(&this->finished_tiles_m,NULL);
}

const vector<double> Game::gen_hist() const {
    return this->m_gen_hist;
}

const vector<double> Game::tile_hist() const {
    return this->m_tile_hist;
}




/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 
*/

void Game::p_board(std::vector<vector<int>*> *matrix){
    cout << u8"╔" << string(u8"═") * (((*(*matrix)[0]).size())) << u8"╗" << endl;
    for (uint i = 0; i < (*matrix).size(); ++i) {
        cout << u8"║";
        vector<int> curr_line = *(*matrix)[i];
        for (uint j = 0; j < curr_line.size(); ++j) {
            if (curr_line[j] > 0)
                cout << colors[curr_line[j] % 7] << u8"█" << RESET;
            else
                cout << u8"░";
        }
        cout << u8"║" << endl;
    }
    cout << u8"╚" << string(u8"═") * (((*(*matrix)[0]).size())) << u8"╝" << endl;

 }

uint Game::num_of_neighors(int line, int column, int* dom_species_sum, int* argmax_dom_arr, vector<vector<int>*>* matrix_neighbors) {
    int cnt=0;
    int max_dom[7]={0,0,0,0,0,0,0};

    for(int i=-1; i<2; i++){
        if((line+i)<0 || (line+i) > this->board->size()-1){continue;}
        for(int j=-1; j<2; j++){
            if((column+j)<0 || (column+j)>(*board)[0]->size()-1){continue;}
            if(i==0 && j==0){
                (*dom_species_sum)+=(*(*matrix_neighbors)[line])[column];
                continue;
            }
            if((*(*matrix_neighbors)[i+line])[column+j]>0){
                (*dom_species_sum)+=(*(*matrix_neighbors)[line+i])[column+j];
                max_dom[((*(*matrix_neighbors)[line +i])[column + j]) % 7]++;
                cnt++;
            }
        }
    }

    int tmp_max = max_dom[0]*7;
    int tmp_max_indx = 0;
    for(int i=1; i<7; i++){
        if(tmp_max<max_dom[i]*(i)){
            tmp_max=max_dom[i]*(i);
            tmp_max_indx=i;
        }
    }
    if(tmp_max_indx==0){
        (*argmax_dom_arr)=7;
    }else{
        (*argmax_dom_arr)=tmp_max_indx;
    }

    return cnt;
}

uint Game::thread_num() const {
    return this->m_thread_num;
}


void Game::CalcThread::thread_workload() {

    while(1){
        auto gen_start = std::chrono::system_clock::now();
        while(this->game->phase_one->size()==0){
            sched_yield();
        }
        vector<vector<int>*>* thread_board;
        vector<vector<int>*>* thread_temp_board;
        while(this->game->tiles_finished) {
            thread_board = this->game->board;
            thread_temp_board = this->game->temp_board;

            Tile *tile = this->game->phase_one->pop();

            if (tile->gane_is_finished) {
                return;
            }

            int dom_species_sum = 0;
            int argMax_dom_species = 0;
            int dom_species = 0;

            //phase 1 --------------------------------------------
            for (int i = tile->end; i >= tile->begin; i--) {
                for (int j = 0; j < (*(*this->game->board)[i]).size(); j++) {
                    int n_neighbors = this->game->num_of_neighors(i, j, &dom_species_sum, &argMax_dom_species,
                                                                  thread_board);
                    if ((*(*thread_board)[i])[j] == 0) {
                        //calculate color
                        if (n_neighbors == 3) {
                            (*(*thread_temp_board)[i])[j] = argMax_dom_species; //update arg_max_species to be correct
                        }else{
                            (*(*thread_temp_board)[i])[j] = 0;
                        }
                    } else if ((*(*thread_board)[i])[j] > 0) {
                        if (n_neighbors < 2 || n_neighbors > 3) {
                            (*(*thread_temp_board)[i])[j] = 0;
                        }else{
                            (*(*thread_temp_board)[i])[j] = (*(*thread_board)[i])[j];
                        }
                    }
                }
            }
            // end of phase 1 --------------------------------------

            this->game->phase_two->push(tile);
            pthread_mutex_lock(&this->game->finished_tiles_m);
            this->game->tiles_finished--;
            pthread_mutex_unlock(&this->game->finished_tiles_m);
        }

        vector<vector<int>*>* temp_for_swap=thread_board;
        thread_board=thread_temp_board;
        thread_temp_board=temp_for_swap;


        while(this->game->phase_two->size()) {
            Tile *tile=this->game->phase_two->pop();
            int dom_species_sum = 0;
            int argMax_dom_species = 0;
            int dom_species = 0;
            for(int i = tile->end; i>=tile->begin; i--){
                for(int j = 0; j < (*(*this->game->board)[i]).size(); j++){
                    if((*(*thread_board)[i])[j] > 0){
                        dom_species_sum=0;
                        int n_neighbors = this->game->num_of_neighors(i, j, &dom_species_sum, &argMax_dom_species,thread_board);
                        dom_species = std::round((double)dom_species_sum /(n_neighbors+1));
                        (*(*thread_temp_board)[i])[j]=dom_species;
                    }else{
                        (*(*thread_temp_board)[i])[j] = 0;
                    }
                }
            }

            this->game->Completed_tasks->push(tile);
        }

        auto gen_end = std::chrono::system_clock::now();
        pthread_mutex_lock(&this->game->task_mutex);
        this->game->m_tile_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
        pthread_mutex_unlock(&this->game->task_mutex);
    }

}
