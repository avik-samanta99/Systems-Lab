/* 
Question : 2
Highway Tunnel Problem (with Mutex & Conditional Variable)
Approach : Multiple cars from same direction are allowed
Name : Stuti Priyambda, Avik Samanta
Roll No :  204101055, 204101016

Approach :-
1. Cars arrived at any end of the tunnel, will wait for their turn
2. If any car gets their turn, they will block the cars at the other end
3. But at that point if there are cars from the same direction, and the number of cars waiting at the other direction is less than N, we will allow these cars to go through the tunnel
4. But the moment number of waiting cars at the other end exceeds N, no cars from this direction will be alowed to enter the tunnel
5. Now the cars inside the tunnel will pass through, and allow the cars in the other direction to enter the tunnel, following the aboove condition
6. This process will repeat untill all the cars (to be checked) depart from their respective sides
*/

#include<bits/stdc++.h>
#include<thread>
#include<mutex>
#include<condition_variable>
using namespace std;

#define N 5 // waiting threshold
#define left 0 // 0 indicates direction left
#define right 1 // 1 indicates direction right

mutex mtx; // mutex for lock and unlock
condition_variable_any cv[2]; // conditional variable for both sides
int waiting[2]; // count of waiting cars at both ends
int turn; // which sides turn it is (0 : left, 1 : right, -1 : free)
int countVehicle; // count the number of vehicals inside the tunnel at the moment

// This function controls the arrivals of the cars at the tunnel
void arrive(int vehicleID, int direction){
    mtx.lock(); // acquire lock
    cout << "Vehicle " << vehicleID << " arrived the tunnel at the " << (direction == left ? "left ..." : "right ...") << endl;
    // if (other side has the turn) or (this side has the turn but other side has more than N cars waiting), then wait
    while((turn == 1-direction) or (turn == direction and waiting[1-direction] > N)){
		waiting[direction] ++; // increment the waiting count for this direction 
		cv[direction].wait(mtx); // wait for the condition variable to notify
		waiting[direction] --; // decrement the waiting count for this direction after waiting is done
	}
    turn = direction; // now this direction has the turn, enter the tunnel
    cout << "Vehicle " << vehicleID << " has entered the tunnel from " << (direction == left ? "left ..." : "right ...") << "\n";
    countVehicle ++; // increment the count of the cars inside the tunnel
    mtx.unlock(); // finally release the lock
}

// This function controls the departure of the cars from the tunnel
void depart(int vehicleID, int direction){
    mtx.lock(); // acquire lock, and let the car depart from the tunnel
    cout << "Vehicle " << vehicleID << " has departed from " << (direction == left ? "right ..." : "left ...") << "\n";
    countVehicle --; // decrement the count of the cars inside the tunnel
    if(countVehicle == 0){ // when there are no cars in the tunnel
        turn = -1; // free the tunnel
        cv[1-direction].notify_all(); // notify all the waiting cars at the other end
    }
    mtx.unlock(); // then release the lock
}

// driver function for controllong the arrive() and depart() function calls
void driverFunction(int vehicleID){
    int direction = (rand() % 2 ? right : left); // randomly choose the direction
    arrive(vehicleID, direction); // call the arrive() function, that a new car has arrived
    this_thread::sleep_for(chrono::microseconds(100)); // wait for the cars to pass the tunnel
    depart(vehicleID, direction); // depart from the tunnel, and notify others
}

int main(){
    int n_cars; // total no of cars to handle
    cout << "Enter the number of cars to handle : ";
    cin >> n_cars;
    // initializations
    waiting[left] = waiting[right] = 0;
    countVehicle = 0; turn = -1;
    // list of car threads
    vector<thread> threads(n_cars);
    srand(time(0));
    for(int i = 0; i < n_cars; i ++){
        threads[i] = thread(driverFunction, i+1); // create threads for each car
    }
    for(int i = 0; i < n_cars; i ++){
        threads[i].join(); // join the threads with the main flow
    }
    return 0;
}
