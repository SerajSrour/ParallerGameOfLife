#ifndef __GAMERUN_H
#define __GAMERUN_H

#include "Headers.hpp"
#include "Thread.hpp"
#include "utils.hpp"
#include "PCQueue.hpp"

/*--------------------------------------------------------------------------------
								  Species colors
--------------------------------------------------------------------------------*/
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black - 7 */
#define RED     "\033[31m"      /* Red - 1*/
#define GREEN   "\033[32m"      /* Green - 2*/
#define YELLOW  "\033[33m"      /* Yellow - 3*/
#define BLUE    "\033[34m"      /* Blue - 4*/
#define MAGENTA "\033[35m"      /* Magenta - 5*/
#define CYAN    "\033[36m"      /* Cyan - 6*/


/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/
struct game_params {
	// All here are derived from ARGV, the program's input parameters. 
	uint n_gen;
	uint n_thread;
	string filename;
	bool interactive_on; 
	bool print_on; 
};


/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/

class Game {
public:

	explicit Game(game_params const & params);
	~Game()=default;
	void run(); // Runs the game
	const vector<double> gen_hist() const; // Returns the generation timing histogram
	const vector<double> tile_hist() const; // Returns the tile timing histogram
	uint thread_num() const; //Returns the effective number of running threads = min(thread_num, field_height)
	class Tile {
    public:
        int begin;
        int end;
        Tile* next_tile;
        Tile* prev_tile;
        bool end_phase2;
        bool end_phase1;
        bool end_write_ph_1;
        bool gane_is_finished;
        Tile(int begin,int end): begin(begin), end(end),next_tile(nullptr),prev_tile(nullptr),end_phase2(false),end_phase1(false),end_write_ph_1(false),gane_is_finished(false){}
        ~Tile()=default;
    };
    class CalcThread : public Thread{
    public:
        explicit CalcThread(int thread_id):Thread(thread_id),finished(false){}
        ~CalcThread() override =default;
        virtual void thread_workload() override;
        Game* game;
        bool finished;
    };


protected: // All members here are protected, instead of private for testing purposes

	// See Game.cpp for details on these three functions
	void _init_game(); 
	void _step(uint curr_gen); 
	void _destroy_game();
    void p_board(std::vector<vector<int>*> *matrix);
	inline void print_board(const char* header);
	uint num_of_neighors(int line, int column,int* dom_species_sum,int* argmax_dom_array, vector<vector<int>*>* matrix_neibors);

	uint m_gen_num; 			 // The number of generations to run
	uint m_thread_num; 			 // Effective number of threads = min(thread_num, field_height)
	vector<double> m_tile_hist; 	 // Shared Timing history for tiles: First (2 * m_gen_num) cells are the calculation durations for tiles in generation 1 and so on. 
							   	 // Note: In your implementation, all m_thread_num threads must write to this structure.
	vector<double> m_gen_hist;  	 // Timing history for generations: x=m_gen_hist[t] iff generation t was calculated in x microseconds
	vector<Thread*> m_threadpool; // A storage container for your threads. This acts as the threadpool.

	bool interactive_on; // Controls interactive mode - that means, prints the board as an animation instead of a simple dump to STDOUT 
	bool print_on; // Allows the printing of the board. Turn this off when you are checking performance (Dry 3, last question)
	
	// TODO: Add in your variables and synchronization primitives  
    string filename;
    vector<vector<int>*> *board;
    vector<vector<int>*> *temp_board;
    PCQueue<Tile*>* phase_one;
    PCQueue<Tile*>* phase_two;
    PCQueue<Tile*>* Completed_tasks;
    pthread_mutex_t task_mutex;
    int tiles_finished;
    pthread_mutex_t finished_tiles_m;
};
#endif
