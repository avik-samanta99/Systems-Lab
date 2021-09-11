/*** Second Chance Algorithm Implementation  ****/
/** Stuti Priyambda,  Avik Samanta             **/
/** 204101055,   204101016                      */
/************************************************/


#include<iostream>
#include<vector>
#include<queue>
#include<algorithm>

using namespace std;

//Vector to store List of Replaced Box.
vector<int> replaced_box_vec;

vector<vector<pair<int, int>>> status;

int clock_algo_implementation(int no_of_slots, vector<int> box_vec)
{
	status.resize(no_of_slots);
    queue<pair<int, bool> > ordrng_que;
    int victim_pointr =0, index =0;
    //vector to store the current box present in Memory.
	vector <int> curr_box_vectr(no_of_slots, -1);

	int no_of_replacmnts =0;
	for (int i=0; i<box_vec.size(); i++)
	{
	    int cur_box= box_vec[i];
		auto itrtr = find(curr_box_vectr.begin(), curr_box_vectr.end(), cur_box);
        //If this box is  already  NOT present in the Slot.
		if(itrtr == curr_box_vectr.end())
		{
			//If no. of boxes currently present are less than it's no. of slots.
			if(count(curr_box_vectr.begin(), curr_box_vectr.end(), -1)>=1)
			{

				//Then No need to remove any pages just insert new pages.
				curr_box_vectr[index] =box_vec[i];
				ordrng_que.push({box_vec[i], 0});
				index++;

			}
			else
            {
                pair<int, bool> cur_top;
                cur_top = ordrng_que.front();
                ordrng_que.pop();
                while(cur_top.second ==1)
                {
                    ordrng_que.push({cur_top.first, 0});
                    cur_top = ordrng_que.front();
                    ordrng_que.pop();
                }
                auto itr1 = find(curr_box_vectr.begin(), curr_box_vectr.end(), cur_top.first);
                int box_index_to_rplc = itr1 - curr_box_vectr.begin();
                replaced_box_vec.push_back(curr_box_vectr[box_index_to_rplc]);
                curr_box_vectr[box_index_to_rplc] = box_vec[i];
                ordrng_que.push({box_vec[i], 0});
                no_of_replacmnts++;
            }
		}
		else
        {
            //Since this Box is already present in Slots, So, just make it's reference bit 1.
            queue<pair<int, bool> > temp_que;
            pair<int, bool> curr_top;
            //Pop all elements from the ordrng_que and make it's refernece bit 1 if it is that box
            //Otherwise if the popped box is not matching then just pop and push in temp_que.
            while(!(ordrng_que.empty()))
            {
                curr_top =ordrng_que.front();
                ordrng_que.pop();
                if(curr_top.first != box_vec[i])
                    temp_que.push(curr_top);
                else
                    temp_que.push({curr_top.first, 1});
            }
            ordrng_que =temp_que;
        }
        
        int ref_bit;
        for(int j = 0; j < no_of_slots; j ++){
        	queue<pair<int, bool>> temp = ordrng_que;
        	while(!temp.empty()){
        		if(temp.front().first == curr_box_vectr[j]){
        			ref_bit = (temp.front().second ? 1 : 0);
        			break;
				}
				else temp.pop();
			}
        	status[j].push_back(make_pair(curr_box_vectr[j], ref_bit));
		}

	}

	return no_of_replacmnts;
}

void display(){
	for(int i = 0; i < status[0].size(); i ++){
		for(int j = 0; j < status.size(); j ++){
			if(status[j][i].first != -1) cout << status[j][i].first << "(" << status[j][i].second << ")\t";
			else cout << "-(-)\t";
		} cout << endl;
	} cout << endl;
}


int main()
{
    vector<int> pg_vectr;
    int no_of_boxes, no_of_slots, page_rplcmnt;
    cout<<"Enter no. of boxes : ";
    cin>>no_of_boxes;
    cout<<"Enter these "<<no_of_boxes<<" boxes identification number :-\n";
    for(int i=0; i<no_of_boxes; i++)
    {
        int box;
        cin>>box;
        pg_vectr.push_back(box);
    }
    cout<<"Enter no. of slots : ";
    cin>>no_of_slots;
    int no_of_rplcmnt =clock_algo_implementation(no_of_slots, pg_vectr);
    cout<<"\n *** Assuming very initial all slots are empty. Othrewise (No of fault = No of Replacements) ***\n\n";
    cout << "The page replacement status :-\n";
    display();
    cout<<"No. of Replacements are : ";
    cout<<no_of_rplcmnt;
    cout<<"\nNo. of page faults are : ";
    cout<<no_of_rplcmnt + no_of_slots<<endl;
    cout<<"List Of Replaced blocks is :-\n";
    for(int i=0; i<replaced_box_vec.size(); i++)
        cout<<replaced_box_vec[i]<<" ";
    cout<<endl;


}
