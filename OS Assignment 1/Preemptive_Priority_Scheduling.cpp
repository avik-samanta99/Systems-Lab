/****        PREMPTIVE PRIORITY SCHEDULING                ***/
/****      Name: Stuti Priyambda, Avik Samanta            ***/
/***       Roll No: 204101055, 204101016                  ***/

/** ASSUMED PRIORITY with Higher number has Higher PRIORITY */



#include <cstdlib>
#include <iostream>
#include <queue>
#include <ctime>
#include <climits>
#include <vector>
#include <cstring>
#include <sstream>


#define P_COUNT 9

using namespace std;

//Class To define Process and it's Attributes.
class Process {
	public:
	    int p_id;
	    time_t t_arrvl, initial_t_arrvl;
	    time_t t_burst, rem_tburst;
	    time_t t_comp, t_turn_around, t_waiting;
        int priority;

    void initial_arrival()
	{
		initial_t_arrvl = t_arrvl;
		rem_tburst = t_burst;
	}
};

class compare
{
    public:
        bool operator() (const Process &p1, const Process &p2)
        {
            return p1.t_arrvl > p2.t_arrvl;
        }
};

// Ready Queue, Completion Queue.
priority_queue<Process, vector<Process>, compare> ready_queue, completion_queue;

Process get_process(priority_queue<Process, vector<Process>, compare>* ready_q, int index)
{
    priority_queue <Process, vector<Process>, compare> prio_q;
    int i;
    Process p;

    if(index > 0)
    {
        for (i = 0; i < index; i++)
        {
            Process temp_p = (*ready_q).top();
            prio_q.push(temp_p);
            (*ready_q).pop();
        }
        p = (*ready_q).top();
        (*ready_q).pop();

        while (!(*ready_q).empty())
        {
            Process temp_p = (*ready_q).top();
            prio_q.push(temp_p);
            (*ready_q).pop();
        }
        (*ready_q) = prio_q;
    }
    else
    {
        p = (*ready_q).top();
        (*ready_q).pop();
    }

    return p;
}

vector<int> get_high_priority_process_info(int time_bound, priority_queue<Process,vector<Process>, compare> temp_queue)
{
    vector<int> vec;
    int i=0, idx =0,maxm = INT_MIN;
    // Getting the process having highest priority among all Arrived Processes.
    while (!temp_queue.empty() && temp_queue.top().t_arrvl <= time_bound)
    {
        if (temp_queue.top().priority > maxm)
        {
            maxm = temp_queue.top().priority;
            idx = i;
        }
        temp_queue.pop();
        i++;
    }
    vec.push_back(idx);
    vec.push_back(maxm);
    return vec;
}


/* Implementing Pre-emptive priority scheduling  Assuming, Higher Number will have Higher Priority */
void Premptive_Priority_sch(queue<Process>& gant_q)
{
    time_t curr_tym = 0;
    Process p;
    while (!ready_queue.empty())
    {
        bool flag =0;

        while (curr_tym < ready_queue.top().t_arrvl)
        {
            flag =1;
            curr_tym +=1 ;
        }

        if(flag !=0)
        {
            p.p_id = INT_MIN;
            p.t_comp = curr_tym;
            gant_q.push(p);
            flag =0;
        }
        vector<int> high_priority_vec (2);
        high_priority_vec= get_high_priority_process_info(curr_tym, ready_queue);
        int index = high_priority_vec[0], cur_max_priority =high_priority_vec[1];
        p= get_process(&ready_queue, index);

        while (p.rem_tburst !=0  && (ready_queue.empty() || curr_tym < ready_queue.top().t_arrvl || p.priority >= cur_max_priority))
        {
            p.rem_tburst--;
            curr_tym++;
            high_priority_vec= get_high_priority_process_info(curr_tym, ready_queue);
            cur_max_priority =high_priority_vec[1];
        }
        if (p.rem_tburst > 0)
        {
            /////////////////////
            p.t_comp = curr_tym;
            p.t_arrvl = curr_tym;
            gant_q.push(p);
            ready_queue.push(p);
        }
        else
        {
            p.t_comp = curr_tym;
            p.t_turn_around = p.t_comp - p.initial_t_arrvl;
            p.t_waiting = p.t_turn_around - p.t_burst;
            gant_q.push(p);
            completion_queue.push(p);
        }
    }
}

//Initially updating all the Process Attributes.
void update_process_attr(int roll_num_arr[], int len)
{
    for(int i=0; i<len; i++)
    {
        Process p;
        p.p_id = i+1;
        p.t_arrvl =roll_num_arr[len-i-1];
        p.priority = roll_num_arr[i];

        int t = int(time_t(0));
        int ith_process_bt = rand()%8;

        while(!(ith_process_bt >=2 && ith_process_bt<=7))
            ith_process_bt =rand()%8;
        p.t_burst = ith_process_bt;
        p.initial_arrival();
        ready_queue.push(p);
    }
}

//To display the Time Descriptions and gantt Chart.
void time_and_gantt_display(priority_queue<Process,vector<Process>, compare> q, queue<Process> gant, int len)
{
    Process p1;
    cout<<"Process  "<<"Arrival_t       "<<"priority       "<<"Burst_t       "<<"Completion_t       "<<"Turn_Around_t       "<<"Waiting_t       "<<endl;
    double total_waiting_t =0, avg_waiting_t;
    while(!q.empty())
	{
	    p1 =q.top();

        cout<<"p"<<p1.p_id<<"         ";
        cout<<p1.initial_t_arrvl<< "              ";
        cout<<p1.priority<< "              ";
        cout<<p1.t_burst<< "              ";
        cout<<p1.t_comp<< "                 ";
        cout<<p1.t_turn_around<< "                   ";
        cout<<p1.t_waiting<<endl;
        total_waiting_t += p1.t_waiting;
        q.pop();
	}
	avg_waiting_t =total_waiting_t/len;

	cout<<endl<<endl<<"Average Waiting time For Scheduling processes is "<<avg_waiting_t<<endl<<endl;
    cout<<"Gnatt Chart :-\n\n";
    cout<<"NL is showing, There is no process to execute this time. i.e.. cpu is idle for that duration. \n";
	queue<Process> g1 ;
	g1 =gant;
    time_t old_tym=0 ;
	while (!g1.empty())
	{
        cout << string(((g1.front().t_comp)  -old_tym)*3 , '.');
        old_tym =g1.front().t_comp ;
		g1.pop();
	}
	g1 =gant;
	cout << "\n|";
    old_tym=0 ;
    while (!g1.empty())
    {
        int lenth1 = (((g1.front().t_comp)  -old_tym)*3 );
        int lenth2 = lenth1 -(lenth1/2) -3;
        cout<< string( lenth1/2 , ' ');
		if (g1.front().p_id == INT_MIN)
			cout << "NL" ;
		else
			cout << "P" << g1.front().p_id;
		if(lenth2>0)
            cout<< string( lenth2, ' ');
        cout<<'|';
        old_tym =g1.front().t_comp ;
		g1.pop();
	}
	g1 =gant;
	old_tym=0 ;
	cout<<endl;
	while (!g1.empty())
	{
        cout << string(((g1.front().t_comp)  -old_tym)*3 , '.');
        old_tym =g1.front().t_comp ;
		g1.pop();
	}
    cout<<endl<<0;
    g1 =gant;
	old_tym=0 ;

	while (!g1.empty())
	{
	    int lenth1 =((g1.front().t_comp)  -old_tym)*3;
	    if(lenth1/10)
            cout << string(lenth1 -2, ' ');
        else
            cout<< string(lenth1 -1, ' ');
        cout<<g1.front().t_comp;
        old_tym =g1.front().t_comp ;
		g1.pop();
	}
    cout<<endl<<endl;
}




int main()
{
    string roll_str;
	int roll_no, roll_no_arr[9];
    cout<<"Enter Roll No to get priority and Arrival time \n";
	cin>>roll_str;
	if(roll_str.length() !=9)
    {
        //If roll no is not of 9 digits. It's Invalid Input.
        cout<<"Invalid Input. please Enter NO. with 9 digits.\n ";
        return 0;
    }
	stringstream strobj(roll_str);
	strobj >>roll_no;

	for(int i=P_COUNT; i>=1; i--)
    {
        roll_no_arr[i-1] = 	roll_no% 10;
        roll_no /=10;
    }


    update_process_attr(roll_no_arr, P_COUNT);

    queue<Process> gantt;
    cout<<"\nNOTE :- Assumed priority with Higher no. has high Priority\n\n";
    Premptive_Priority_sch(gantt);

	time_and_gantt_display(completion_queue, gantt, P_COUNT);
    return 0;
}
