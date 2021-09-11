#include<iostream>
#include<vector>
#include<queue>
#include<unordered_map>
#include<algorithm>
#include<climits>
#include<string.h>
#include<fstream>
using namespace std;


vector<vector<int>> page_replacement_status_fifo, page_replacement_status_lru, page_replacement_status_lfu;

int fifo_implemntation(int no_of_frames, vector<int> page_arr)
{
    //Queue to Store the ordering in which pages have gone in Memory.
	queue<int> fifo_queue;

	//vector to store the current pages present in Memory.
	vector <int> curr_page_vec(no_of_frames, -1);

	page_replacement_status_fifo.resize(no_of_frames);

	// Start from initial page
	int no_of_page_faults = 0, idx=0;
	for (int i=0; i<page_arr.size(); i++)
	{
	    int pg1= page_arr[i];
		auto itr1 = find(curr_page_vec.begin(), curr_page_vec.end(), pg1);
        //If this page is not already present in the Memory
		if(itr1 == curr_page_vec.end())
		{
			//If no. of Frames currently in Memory is less than it's capacity to hold no. of Frames.
			if(count(curr_page_vec.begin(), curr_page_vec.end(), -1)>=1)
			{
				//Then No need to remove any pages just insert new pages.
				curr_page_vec[idx++] =page_arr[i];
				no_of_page_faults++;
				fifo_queue.push(page_arr[i]);
			}
			else
			{
				//Find the pages which is First inserted to pop first. (FIFO)
				int pg = fifo_queue.front();      // Will get the first inserted pages
				fifo_queue.pop();          // Will pop the pages which is inserted  first.
				auto it = find(curr_page_vec.begin(), curr_page_vec.end(), pg);
				int index = it - curr_page_vec.begin();
				curr_page_vec[index] = page_arr[i];     //Insert the pages at the place where the first insertion has been done after removing that page.
 				no_of_page_faults++;
				fifo_queue.push(page_arr[i]);
	    		}
		}
		for(int j = 0; j < no_of_frames; j ++) page_replacement_status_fifo[j].push_back(curr_page_vec[j]);
	}

	return no_of_page_faults;
}

int lru_implemntation(int no_of_frames, vector<int> page_arr)
{
    //unordered map to Store the Least recently used pages.
	unordered_map<int, int> umap;

	page_replacement_status_lru.resize(no_of_frames);

	//vector to store the current pages present in Memory.
	vector <int> curr_page_vec(no_of_frames, -1);

	// Start from initial page
	int no_of_page_faults = 0, idx=0;
	for (int i=0; i<page_arr.size(); i++)
	{
	    int pg1= page_arr[i];
		auto itr1 = find(curr_page_vec.begin(), curr_page_vec.end(), pg1);
        //If the page is not present in the Memory
		if(itr1 == curr_page_vec.end())
		{
			//If no. of Frames currently in Memory is less than it's capacity to hold no. of Frames.
			if(count(curr_page_vec.begin(), curr_page_vec.end(), -1)>=1)
			{
				//Then No need to remove any pages just insert new pages.
				curr_page_vec[idx++] =page_arr[i];
				no_of_page_faults++;
				umap.insert({page_arr[i], i});
			}
			else
            {
                int least_recently_idx = INT_MAX, page_indx;
                for(int k=0; k<no_of_frames; k++)
                {
                    //Checking which of the page currently in Memory is least recently used.
		            if(umap[curr_page_vec[k]] < least_recently_idx)
                    {
		                least_recently_idx =umap[curr_page_vec[k]];
                        page_indx =k;
                    }
                }

                //Erase that page from umap also.
                umap.erase(curr_page_vec[page_indx]);
				curr_page_vec[page_indx] = page_arr[i]; //Insert the page at the place where the least recently used page inserted after removing that page.
 				no_of_page_faults++;
				umap.insert({page_arr[i], i});      //Insert this new page in umap.
            }

		}
		else
        {
            //If the page is already in the Memory then update that it is recently used i.e.. update it's index.
            umap[page_arr[i]] =i;
       	}

       	for(int j = 0; j < no_of_frames; j ++) page_replacement_status_lru[j].push_back(curr_page_vec[j]);
	}
	return no_of_page_faults;
}


int lfu_implemntation(int no_of_frames, vector<int> page_arr)
{
    //unordered map to Store the frequency of the  page currently in Memory.
	unordered_map<int, int> count_map;

	//vector to store the current pages present in Memory.
	vector <int> curr_page_vec(no_of_frames, -1);

	page_replacement_status_lfu.resize(no_of_frames);

    //Queue to Store the ordering in which pages have gone in Memory.
	queue<int> ordr_queue;

	// Start from initial page
	int no_of_page_faults = 0, idx=0;

	for (int i=0; i<page_arr.size(); i++)
	{
	    int pg1= page_arr[i];
		auto itr1 = find(curr_page_vec.begin(), curr_page_vec.end(), pg1);
        //If this page is not present in the Memory
		if(itr1 == curr_page_vec.end())
		{
			//If no. of Frames currently in Memory is less than it's capacity to hold no. of Frames.
			if(count(curr_page_vec.begin(), curr_page_vec.end(), -1)>=1)
			{
				//Then No need to remove any pages just insert new pages.
				curr_page_vec[idx++] =page_arr[i];
				no_of_page_faults++;
				count_map.insert({page_arr[i], 1});
				(ordr_queue).push(page_arr[i]);

			}
			else
            {
           		bool flag =0;
           		int least_freq = INT_MAX, page_indx;
                vector<int> same_leastfreq_page_vec;
            	for(int k=0; k<no_of_frames; k++)
                {
                    //Checking which of the page currently in Memory is least frequently used.
                    if(count_map[curr_page_vec[k]] < least_freq)
            		{
              			flag =0;
                        same_leastfreq_page_vec.clear();
                       	least_freq =count_map[curr_page_vec[k]];
                       	page_indx =k;
                        //same_leastfreq_page_vec.push_back(curr_page_vec[k]);
                    }
                    if(least_freq == count_map[curr_page_vec[k]] )
           			{
                        //If more than one page has highest frequency.then keep all pages in a vector.
                       	//Because In that case We need to apply FIFO for removing the page and Inserting new one.
               			flag =1;
               			same_leastfreq_page_vec.push_back(curr_page_vec[k]);
          			}
           		}

                //If flag is set that means We need to apply FIFO
                if(flag)
                {

	               /////////////////////////////
                    queue<int> temp_queue;
        			int temp_idx=0;
                    for(int j=0; j<no_of_frames; j++)
          			{
                        int temp_pg = (ordr_queue).front();
			            (ordr_queue).pop();
                        //cout<<"pagefault is "<<no_of_page_faults<<endl;
			            bool flag2 =0;
                       	for(int p=0; p<same_leastfreq_page_vec.size(); p++)
			            {
                        	if( temp_pg == same_leastfreq_page_vec[p])
                            {
                                //so, We got temp_pg is inserted first,Apply FIFO
			                    auto it = find(curr_page_vec.begin(), curr_page_vec.end(), temp_pg);
			                    int index = it - curr_page_vec.begin();
			                    //Insert the pages at the place where the first insertion has been done after removing that page.
			                    curr_page_vec[index] = page_arr[i];
			                    no_of_page_faults++;
	 		         	        count_map.erase(temp_pg);
                        		count_map.insert({page_arr[i], 1});
                                flag2=1;

                            }
                        }
                        if(flag2)
                            break;
                        else
                        {
                            temp_queue.push(temp_pg);
                        }

                    }
		            while(!((ordr_queue).empty()))
                	{
			            int temp_pg = (ordr_queue).front();
                        (ordr_queue).pop();
			            temp_queue.push(temp_pg);
                    }
		            temp_queue.push(page_arr[i]);
                    (ordr_queue) =temp_queue;

                }
                //Otherwise only one page has least frequency. So, replace that least frequently used.
                else
                {
                    //Remove that page from Queue.
                    queue<int> temp_queue;
                    int t_pg = (ordr_queue).front();
                    (ordr_queue).pop();
                    while(t_pg != curr_page_vec[page_indx])
                    {
                        temp_queue.push(t_pg);
                        t_pg = (ordr_queue).front();
                        (ordr_queue).pop();

                    }
                    while(!((ordr_queue).empty()))
                    {
                        int temp_pg = (ordr_queue).front();
                        (ordr_queue).pop();
                        temp_queue.push(temp_pg);
                    }
                    //Pushing the current arrived page in Ordering Queue
                    temp_queue.push(page_arr[i]);

                    (ordr_queue) =temp_queue;
                    //Erase that page from count_map also.
                    count_map.erase(curr_page_vec[page_indx]);
                    curr_page_vec[page_indx] = page_arr[i];
                    no_of_page_faults++;
                    count_map.insert({page_arr[i], 1});

                }

            }
		}
		else
    	{
        	count_map[page_arr[i]]++;
    	}

    	for(int j = 0; j < no_of_frames; j ++) page_replacement_status_lfu[j].push_back(curr_page_vec[j]);

	}
	return no_of_page_faults;
}

void display(vector<vector<int>> &status){
	for(int i = 0; i < status[0].size(); i ++){
		for(int j = 0; j < status.size(); j ++){
			if(status[j][i] != -1) cout << status[j][i] << "\t";
			else cout << "-\t";
		}
		cout << endl;
	}
}


int main(int argc, char** argv)
{

	int no_of_pages, no_of_frames, pges;
	vector<int> page_arr;
	ifstream frd;
	string filenm =argv[1];
	frd.open(filenm);
	if(!frd)
	{
		cout<<"Input  file can't be opened\n";
		return 0;
	}
	frd >> no_of_pages;
    frd >> no_of_frames;
	while(!frd.eof())				
	{
		frd>>pges;
		page_arr.push_back(pges);
	}

	if(argc ==2)
	{
		cout<<"In FIFO Implementation page Fault is  ";
    	cout<<fifo_implemntation(no_of_frames, page_arr)<<endl;
    	cout<<"The status of FIFO implementation :-\n";
    	display(page_replacement_status_fifo);
    	cout<<"In LFU Implementation page Fault is  ";
    	cout<<lfu_implemntation(no_of_frames, page_arr)<<endl;
    	cout<<"The status of LFU implementation :-\n";
    	display(page_replacement_status_lfu);
    	cout<<"In LRU Implementation page Fault is  ";
    	cout<<lru_implemntation(no_of_frames, page_arr)<<endl;
    	cout<<"The status of LRU implementation :-\n";
    	display(page_replacement_status_lru);
		return 0;
	}
	else
	{
		for(int i=2; i<argc; i++)
        {
	        if(!(strcmp(argv[i] ,"FF")))
        	{
                	cout<<"In FIFO Implementation page Fault is  ";
                	cout<<fifo_implemntation(no_of_frames, page_arr)<<endl;
                	cout<<"The status of FIFO implementation :-\n";
    				display(page_replacement_status_fifo);
        	}
        	else if(!(strcmp(argv[i] ,"LF")))
        	{
                	cout<<"In LFU Implementation page Fault is  ";
                	cout<<lfu_implemntation(no_of_frames, page_arr)<<endl;
                	cout<<"The status of LFU implementation :-\n";
    				display(page_replacement_status_lfu);
        	}
        	else if(!(strcmp(argv[i] ,"LR")))
        	{
                	cout<<"In LRU Implementation page Fault is  ";
                	cout<<lru_implemntation(no_of_frames, page_arr)<<endl;
                	cout<<"The status of LRU implementation :-\n";
    				display(page_replacement_status_lru);
        	}

        }

	}


	return 0;

}













