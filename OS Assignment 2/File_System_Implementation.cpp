/***   File System Algorithm Implementation  ****/
/**    Stuti Priyambda,  Avik Samanta          **/
/**    204101055,   204101016                   */
/************************************************/

#include<bits/stdc++.h>
#include<iostream>
using namespace std;

// Structure denoting the Index Nodes
struct INodes{
    char name[8]; // name of the file
    int size; // size (no of blocks) of the file
    int blockPointer[8]; // blocks assigned to the file
    int used; // if the node is used or not
};

class disk{
    private:
        bool freeBlock[128]; // the freeblock list, 1 : free, 0 : taken
        INodes iNode[16]; // index nodes
        char dataBlock[127][1024]; // datablocks storing the file content
    public:
        // constructor, for initialization
        disk(){
            freeBlock[0] = false; // only the first block is used initially
            for(int i = 1; i < 128; i ++)
                freeBlock[i] = true;
            for(int i = 0; i < 16; i ++){
                iNode[i].used = 0;
                iNode[i].size = 0;
            }
            for(int i = 0; i < 127; i ++){
                dataBlock[i][0] = '\0';
            }
        }
        // method to check if any filename with the given name exists or not
        int checkName(char curFileName[]){
            for(int i = 0; i < 16; i ++){
                if(iNode[i].used and strcmp(iNode[i].name, curFileName) == 0)
                    return i;
            }
            return -1;
        }
        // method to create a file with the given name and size (no of blocks)
        void Create(char curFileName[], int curSize){
            if(curSize > 8){
                cout << "Requested number of blocks can not be allocated. Try again...\n\n";
                return;
            }
            if(checkName(curFileName) != -1){
                cout << "The filename already exists. Try again...\n\n";
                return;
            }
            int totalUsed = 0;
            for(int i = 0; i < 16; i ++){
            	if(iNode[i].used) totalUsed += iNode[i].size;
			}
			if(127-totalUsed < curSize){
				cout << "Not enough space available. Try again...\n\n";
			}
            bool Created = false;
            for(int i = 0; i < 16; i ++){
                if(!iNode[i].used){
                    Created = true;
                    iNode[i].used = 1;
                    strcpy(iNode[i].name, curFileName);
                    iNode[i].size = curSize;
                    int k = 0;
                    for(int j = 1; j < 128, k < curSize; j ++){
                        if(freeBlock[j]){
                            iNode[i].blockPointer[k ++] = j;
                            freeBlock[j] = false; 
                        }
                    }
                    cout << "File with the filename \'" << curFileName << "\' Created...\n\n";
                    break;
                }
            }
            if(!Created){
                cout << "No more than 16 files are allowed. Try again later ...\n\n";
                return;
            }
        }
        // method to delete the file with the filename
        void Delete(char curFileName[]){
            int iNodeIndex = checkName(curFileName);
            if(iNodeIndex != -1){
                iNode[iNodeIndex].used = 0;
                for(int i = 0; i < iNode[iNodeIndex].size; i ++){
                    dataBlock[iNode[iNodeIndex].blockPointer[i]][0] = '\0';
                    freeBlock[iNode[iNodeIndex].blockPointer[i]] = true;
                }
                iNode[iNodeIndex].size = 0;
                cout << "File with the filename \'" << curFileName << "\' deleted...\n\n";
            }
            else{
                cout << "No file with such name exists. Try again...\n\n";
            }
        }
        // method to read the file content from the specific block
        void Read(char curFileName[], int blockNumber, char buffer[]){
            buffer[0] = '\0';
            int iNodeIndex = checkName(curFileName);
            if(iNodeIndex == -1){
                cout << "No file with such name exists. Try again...\n\n";
                return;
            }
            else if(blockNumber >= iNode[iNodeIndex].size){
                cout << "Invalid block number. Try again...\n\n";
                return;
            }
            else{
                strcpy(buffer, dataBlock[iNode[iNodeIndex].blockPointer[blockNumber]-1]);
                if(buffer[0] != '\0') cout << "Content read from the block :-\n" << buffer << "\n\n";
                else cout << "There is no content in the requested block...\n\n";
            }
        }
        // method to write the content to the file, into specific block
        void Write(char curFileName[], int blockNumber, char buffer[]){
            int iNodeIndex = checkName(curFileName);
            if(iNodeIndex == -1){
                cout << "No file with such name exists. Try again...\n\n";
                return;
            }
            else if(blockNumber >= iNode[iNodeIndex].size){
                cout << "Invalid block number. Try again...\n\n";
                return;
            }
            else{
                strcpy(dataBlock[iNode[iNodeIndex].blockPointer[blockNumber]-1], buffer);
                cout << "Writing to the file \'" << curFileName << "\' was successful...\n\n";
            }
        }
        // listing out all the files in the directory
        void ls(){
            cout << "\t  --------------------------------------\n";
            cout << "\t||\tFileName\t||\tSize\t||\n";
            cout << "\t  --------------------------------------\n";
            for(int i = 0; i < 16; i ++){
                if(iNode[i].used){
                    cout << "\t||\t" << iNode[i].name << "\t\t||\t" << iNode[i].size << "\t||\n";
                    cout << "\t  --------------------------------------\n";
                }
            }
            cout << endl;
        }
};

int main(){
    int choice, fileSize, blockNumber;
    char fileName[8], ipBuffer[1024], opBuffer[1024];
    string str;
    disk ourDisk;
    while(true){
        cout << "1. Create a file,\n";
        cout << "2. Delete a file,\n";
        cout << "3. Read from a file,\n";
        cout << "4. Write into a file,\n";
        cout << "5. List out the files,\n";
        cout << "6. Exit.\n";
        cout << "\nEnter your choice : ";
        // fflush(stdin);
        cin >> choice;
        switch(choice){
            case 1:
                cout << "Enter the filename : ";
                cin >> fileName;
                cout << "Enter the file Size : ";
                cin >> fileSize;
                ourDisk.Create(fileName, fileSize);
                break;
            case 2:
                cout << "Enter the filename : ";
                cin >> fileName;
                ourDisk.Delete(fileName);
                break;
            case 3:
                cout << "Enter the filename : ";
                cin >> fileName;
                cout << "Enter the block number : ";
                cin >> blockNumber;
                ourDisk.Read(fileName, blockNumber, opBuffer);
                opBuffer[0] = '\0';
                break;
            case 4:
                cout << "Enter the filename : ";
                cin >> fileName;
                cout << "Enter the block number : ";
                cin >> blockNumber;
                cout << "Enter the content to be entered :-\n";
                cin.ignore();
                cin.getline(ipBuffer, 1024);
                ourDisk.Write(fileName, blockNumber, ipBuffer);
                break;
            case 5:
                cout << "Listing out all the files in the current Directory :-\n";
                ourDisk.ls();
                break;
            case 6:
                exit(0);
            default:
                cout << "Invalid entry. Try again...\n";
        }
    }
    return 0;
}
