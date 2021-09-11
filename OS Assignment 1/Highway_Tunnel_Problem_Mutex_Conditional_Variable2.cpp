/* 
Question : 2
Highway Tunnel Problem (with Mutex & Conditional Variable)
Approach : Only one car is allowed at any time
Name : Stuti Priyambda, Avik Samanta
Roll No :  204101055, 204101016

Approach :-
1. Cars arrived at any end of the tunnel, will wait for their turn
2. If any car gets their turn, they will block the cars at any end
3. After this car depart from the tunnel, next turn will be of the car waiting at the other end
4. If therea are no cars waiting at the other end, then car from same side will be allowed
5. This process will repeat untill all the cars (to be checked) depart from their respective sides
*/

#include<bits/stdc++.h>
#include<thread>
#include<mutex>
#include<condition_variable>
using namespace std;

#define left 0 // waiting threshold
#define right 1 // 0 indicates direction left
#define free -1 // 1 indicates direction right

mutex mtx; // mutex for lock and unlock
condition_variable_any cv[2]; // conditional variable for both sides
int waiting[2]; // count of waiting cars at both ends
int turn; // which sides turn it is (0 : left, 1 : right, -1 : free)

// This function controls the arrivals of the cars at the tunnel
void arrive(int vehicalID, int direction){
    mtx.lock(); // acquire lock
    cout << "Vehicle " << vehicalID << " arrived the tunnel at the " << (direction == left ? "left ..." : "right ...") << "\n";
    while(turn != free){ // if the tunnel is free
		waiting[direction] ++; // increment the waiting count for this direction 
		cv[direction].wait(mtx); // wait for the condition variable to notify
		waiting[direction] --; // decrement the waiting count for this direction after waiting is done
	}
    turn = direction; // now this direction has the turn, enter the tunnel
    cout << "Vehicle " << vehicalID << " has entered the tunnel from " << (direction == left ? "left ..." : "right ...") << "\n";
    mtx.unlock(); // finally release the lock
}

// This function controls the departure of the cars from the tunnel
void depart(int vehicalID, int direction){
    mtx.lock(); // acquire lock
    cout << "Vehicle " << vehicalID << " has departed from " << (direction == left ? "right ..." : "left ...") << "\n";
    turn = free; // Lat the car depart from tunnel, and free the tunnel
    if(waiting[1-direction] > 0) cv[1-direction].notify_one(); // if there are cars waiting at the other end, then notify one of them
    else cv[direction].notify_one(); // else notify one from the same direction
    mtx.unlock(); // release lock
}

// driver function for controllong the arrive() and depart() function calls
void driverFunction(int vehicleID){
    int direction = (rand() % 2 ? right : left); // randomly choose the direction
    arrive(vehicleID, direction); // call the arrive() function, that a new car has arrived
    this_thread::sleep_for(chrono::microseconds(10)); // wait for the cars to pass the tunnel
    depart(vehicleID, direction); // depart from the tunnel, and notify others
}

int main(){
    int n_cars; // total no of cars to handle
    cout << "Enter the number of cars to handle : ";
    cin >> n_cars;
    // initializations
    waiting[left] = waiting[right] = 0;
    turn = free;
    // list of car threads
    vector<thread> threads(n_cars);
    for(int i = 0; i < n_cars; i ++){
        threads[i] = thread(driverFunction, i+1); // create threads for each car
    }
    for(int i = 0; i < n_cars; i ++){
        threads[i].join(); // join the threads with the main flow
    }
    return 0;
}
