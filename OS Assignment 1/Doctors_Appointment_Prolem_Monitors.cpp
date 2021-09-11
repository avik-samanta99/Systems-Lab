/* 
Question : 3
Doctor-Patient problem (using Monitors)
Name : Stuti Priyambda, Avik Samanta
Roll No :  204101055, 204101016
*/

#include<bits/stdc++.h>
#include<thread>
#include<mutex>
#include<condition_variable>
using namespace std;

// Global variable for the numbner of chairs and the number of patients to be vaccinated
unsigned int seatingCapacity, noPatients;

// Monitor class for the Doctor and Patient actions
class COVIDChecking{
  private:
  	 // totalVaccinated : total number of people already vaccinated
  	 // totalLeft : total number of people left the clinic due to unavailability of seats
  	 // fullCount : number of seats which are taken by the patients in the waiting room
	 int totalVaccinated, totalLeft, fullCount;
	 condition_variable_any cv; // cv : condition variable which notifies the doctor that a patient has arrived
	 mutex mtx; // mtx : mutex lock for mutual exclusion
	 queue<int> Q; // Q : patient's waiting Queue
	  
  public:
  	 // Constructor of the monitor class, for initializing the values
  	 COVIDChecking(){
  	 	fullCount = 0;
  	 	totalVaccinated = 0;
  	 	totalLeft = 0;
  	 }
  	 
  	 // Method/Function for the patients' arrival
	 void PatientArrival(int patientID){
	 	 mtx.lock(); // Acquire the lock
		 if(fullCount == seatingCapacity){ // If all the seats of the waiting room is taken, then leave the clinic
		 	cout << "Patient " << patientID << " has left the clinic, seat not available ...\n";
		 	++ totalLeft; // increment the count for the patients leaving
		 	mtx.unlock(); // Also release the lock acqured earlier
		 	return; // leave the clinic
		 }
		 else{ // If there are seats available in the waiting room
			 cout << "Patient " << patientID << " is now waiting at the clinic for their turn ...\n";
			 Q.push(patientID); // add the patient into the queue
			 ++ fullCount; // increment the count of the seats taken
			 cv.notify_one(); // notify that a patient has arrived  
			 mtx.unlock(); // release the lock acquired
		 }
	 }
	 
	 // Method/function for the Vaccination by the doctor
	 void DoctorCheckup(){
	 	while(true){ // always keep doing something, either check patient or do your own stuff
		 	 mtx.lock(); // Acquire the lock
			 if(fullCount == 0){ // If there are no patients waiting in the waiting room
			     cout << "Doctor waiting ...\n";
			     cv.wait(mtx); // doctor wait for the condition variable (do his own stuff|)
			 }
			 // this_thread::sleep_for(chrono::nanoseconds(1));
			 cout << "Patient " << Q.front() << " is being vaccinated by the doctor ...\n";
			 Q.pop(); // check or vaccinate one of the patients
			 -- fullCount; // decrement the counter for the number of seats taken
			 ++ totalVaccinated; // increment the counter for the number of patients vaccinated
			 if(totalVaccinated == noPatients-totalLeft){ // if no patients to come hereafter
			 	cout << "Clinic closed for today ...\n";
			 	return; // close the clinic (or return from the doctor thread)
			 }
			 mtx.unlock(); // release the lock
	    }
    }
}check; // create a global object of the class

// calling function for the doctor actions
void Doctor(){
    check.DoctorCheckup();
}
// calling function for the patient actions
void Patient(int patientID){
	check.PatientArrival(patientID);
}

int main(){
	cout << "Enter the number of chairs : ";
	cin >> seatingCapacity;
	cout << "Enter the number of patients to check : ";
	cin >> noPatients;
	// threads for each patient
	vector<thread> patients(noPatients);
	// call the doctor thread
	thread doctor = thread(Doctor);
	// call the patient threads
	for(int i = 1; i <= noPatients; i ++){
	    patients[i-1] = thread(Patient, i);
	}
	// join the patient threads to the main flow
	for(int i = 1; i <= noPatients; i ++){
	    patients[i-1].join();
	}
	// join the doctor thread
	doctor.join();
	return 0;
}
