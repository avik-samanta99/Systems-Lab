/****        DININIG PHILOSOPHER IMPLEMENTATION           ***/
/****      Name: Stuti Priyambda, Avik Samanta            ***/
/***       Roll No: 204101055, 204101016                  ***/

/** Given fair chance to eac philosopher as mention in the Question and at least
one time each philosopher.So, After eating by each Philosopher one time I Stopped. */

#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <mutex>
#include <chrono>
#include <thread>


//Defining all the different states in which a Philosopher
//Possibly could be.
#define HUNGRY 1
#define THINKING 0
#define EATING 2

using namespace std;

unordered_map <int, int> states;
vector <string> action_vec;
//Global constant variable For No. of philosphers
const int philosoper_count =5;

int philosper_eat[philosoper_count];
int eating_counnter =0;
mutex fork_mutex[philosoper_count];   // mutexes for forks
//disp_mutx for displaying the action.
mutex mutx, disp_mutx;

ofstream f_out;


/* I am executing program till 1 times eating by each philosopher.
This boolean array will ensure fair distribution to get chance to eat
among philosopher which is given in Question statement.  */

bool done_flag[5] = {false, false, false, false, false};

// Display the Action performed by Philosopher.
void display_action(string action_str)
{
    disp_mutx.lock();
    cout <<action_str <<endl;
    f_out<<action_str <<endl;
    //action_vec.push_back(action_str);
    disp_mutx.unlock(); // Unlocking after finished printing.
}

/*Checks whether his any of the neighbor Philosopher is eating. If not
 then only This Philosopher will acquire two forks and start eating */
void check_for_neighbors(int philo_num)
{
	if (states[philo_num] == HUNGRY)
    {
        int l_index = (philo_num + 4)% philosoper_count;
        int r_index = (philo_num + 1)% philosoper_count;
        if(states[l_index] != EATING && states[r_index] != EATING)
        {
            //AS the Philosopher is Hungry and his Neighbors are not
            //eating. So, He can Eat Now.
            states[philo_num] = EATING;
            this_thread::sleep_for (chrono::seconds(1));
            display_action("Philosopher " +to_string(philo_num+1) +" has taken the fork "+to_string(l_index+1) +" and " +to_string(philo_num +1));
            display_action("Now, Philosopher " + to_string(philo_num +1) + " is Eating.");
            eating_counnter++;
            done_flag[philo_num] = true;
            fork_mutex[philo_num].unlock();

        }

	}
}

// To Pick up the Fork To Eat.
void initiate_eating(int philo_num)
{
    mutx.lock();
    states[philo_num] = HUNGRY;
    display_action("Philosopher " + to_string(philo_num +1) + " is feeling Hungry and try to Eat. ");

    int l_index = (philo_num + 4)% philosoper_count;
    int r_index = (philo_num + 1)% philosoper_count;
	// eat if neighbours are not eating
	check_for_neighbors(philo_num);
    //Unlock the mutex
    mutx.unlock();

	fork_mutex[philo_num].lock();
	this_thread::sleep_for (chrono::seconds(1));
}

// After Eating Philosoper will put down the forks
//So, that other Philosopers can EAT.
void put_down_fork(int philo_num)
{
	mutx.lock();
    states[philo_num] = THINKING;
    int l_index = (philo_num + 4)% philosoper_count;
    int r_index = (philo_num + 1)% philosoper_count;
    display_action("Now, Philosopher " +to_string(philo_num +1) +" is putting down the fork "+to_string(l_index+1) +" and " +to_string(philo_num +1));
    display_action("Now, Philosopher " + to_string(philo_num +1) + " is thinking.");
    check_for_neighbors(l_index);
	check_for_neighbors(r_index);
    mutx.unlock();
}

void start_dinning(int philospr_no)
{
    while (!done_flag[philospr_no])
    {
        display_action("Philosopher " + to_string(philospr_no +1) + " is thinking.");
        this_thread::sleep_for(chrono::milliseconds(500));      //So, that can't acquire fork to eat right away
        initiate_eating(philospr_no);
        put_down_fork(philospr_no);

    }

}


int main()
{
    f_out.open("output.txt");
    //Threads for 5 Philosophers.
    thread philosper[philosoper_count];
    //Initially All Philosophers are Eating.
    for(int i=0; i<philosoper_count; i++)
        states[i] =THINKING;
    int i=0;
    while(i< philosoper_count)
    {
        philosper[i] = thread(start_dinning, i);
        i++;
    }

    // Joining the Threads.
	for (int j = 0; j< philosoper_count; j++)
        philosper[j].join();
}
