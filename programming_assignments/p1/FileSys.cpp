// CPSC 3500: File System
// Implements the file system commands via shell program

#include <cstring>
#include <iostream>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<cmath>
using namespace std;

#include "FileSys.h"
#include "BasicFileSys.h"
#include "Blocks.h"

// mounts the file system
void FileSys::mount() {
  bfs.mount();
  curr_dir = 1;
}

// unmounts the file system
void FileSys::unmount() {
  bfs.unmount();  
}

//returns true if block is directory vs inode
bool FileSys::is_dir(unsigned int magnumb){
	
	return magnumb==DIR_MAGIC_NUM;	
}
//returns true if directory has no files
bool FileSys::is_empty(int num){
	return num==0;	
}

// make a directory
void FileSys::mkdir(const char *name)
{
  if(strlen(name)>MAX_FNAME_SIZE){
	  cout<<"Filename too long."<<endl;
	  return;
  }
  struct dirblock_t current_block;
  struct dirblock_t entry_dir_block;
  bfs.read_block(curr_dir, (void*) &current_block);
  if(current_block.num_entries == MAX_DIR_ENTRIES){
	cout << "Directory is full."<<endl;
	return;
  }
  unsigned int block_index = 0;
  bool direxists = false;
  while(block_index < current_block.num_entries){
	bfs.read_block(current_block.dir_entries[block_index].block_num, (void*)&entry_dir_block);
	if (!is_dir(entry_dir_block.magic)){
		if(strcmp(current_block.dir_entries[block_index].name,name) == 0){
			direxists = true; 
			}
		}
		block_index++;
	}
   if(!direxists){
		short blocknum = bfs.get_free_block();
		if(blocknum == 0){
			cout << "Disk is Full."<<endl;
			return;
		}
		struct dirblock_t new_directory;
		new_directory.num_entries = 0;
		new_directory.magic =  DIR_MAGIC_NUM;
		for (int i = 0; i < MAX_DIR_ENTRIES; i++)
	      new_directory.dir_entries[i].block_num = 0;
	    
	    strcpy(current_block.dir_entries[current_block.num_entries].name, name);
	    current_block.dir_entries[current_block.num_entries].block_num = blocknum;
	    current_block.num_entries++;
	    bfs.write_block(blocknum,(void*)&new_directory);
	    bfs.write_block(curr_dir,(void*)&current_block);
	}
	else{
		   cout<<"File already exists."<<endl;
			return;
	}
	return;
}

// switch to a directory
void FileSys::cd(const char *name)
{
	struct dirblock_t current_block;
	struct dirblock_t target_dir;
	unsigned int block_index = 0;
	int k_entry = 0; 
	bool fileExists = false;
	bool directory = false;
	bfs.read_block(curr_dir, (void*)&current_block);
	while(block_index < current_block.num_entries){
		bfs.read_block(current_block.dir_entries[block_index].block_num,(void*)&target_dir);
		
		if (strcmp(current_block.dir_entries[block_index].name,name)==0){
			fileExists = true;
			if (is_dir(target_dir.magic)){
				k_entry = block_index;
				directory = true;
			}
		}
		block_index++;
	}
	if (fileExists == false){
		cout<<"File doesn't exist in this directory."<<endl;
		return;
	}

	if (directory == false){
		cout<<"File isn't a directory."<<endl;
		return;
	}
	else
		curr_dir = current_block.dir_entries[k_entry].block_num;
}
// switch to home directory
void FileSys::home() {
	curr_dir =1;
	return;
}

// remove a directory
void FileSys::rmdir(const char *name)
{
	struct dirblock_t delete_block;
	struct dirblock_t current_block;
	bfs.read_block(curr_dir,(void*)&current_block);
	unsigned int number_of_entries = 0;
	unsigned int k_entry = 0;
	int remove = 0;
	bool directory = false;
	while ( number_of_entries < current_block.num_entries) {
		bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*)&delete_block);
		if (strcmp(current_block.dir_entries[number_of_entries].name,name) == 0){	
			if (is_dir(delete_block.magic)){
				if(is_empty(delete_block.num_entries)){
					remove = current_block.dir_entries[number_of_entries].block_num;
					current_block.dir_entries[number_of_entries].block_num = 0;
					k_entry = number_of_entries;
					directory = true;
				} 
				else {
				cout<<"Directory is not empty"<<endl;
				return;
				}
			} 
			else{
				cout<<"File isn't a directory."<<endl;
				return;
			}
		}
		number_of_entries++;
	}
	if(directory){
		if (k_entry == current_block.num_entries)
			current_block.num_entries--; 
		else {
			for (unsigned int i = k_entry; i < current_block.num_entries; i++)
			{
				for (int j = 0; j < MAX_FNAME_SIZE + 1; j++)
					strcpy(&current_block.dir_entries[i].name[j],&current_block.dir_entries[i+1].name[j]);
				current_block.dir_entries[current_block.num_entries].block_num = 0;
				current_block.num_entries--;
			}
		}
		bfs.write_block(curr_dir,(void*)&current_block);
		bfs.reclaim_block(remove);
	}
	else
		cout<<"File doesn't exist."<<endl;
	
}

// list the contents of current directory
void FileSys::ls()
{
	struct dirblock_t current_block;
	struct dirblock_t designated_block;
	string msg = "";
	bfs.read_block(curr_dir, (void*)&current_block);
	for (unsigned int i = 0; i < current_block.num_entries; i++)
		{
			bfs.read_block(current_block.dir_entries[i].block_num,(void*)&designated_block);
			msg += string(current_block.dir_entries[i].name);
			if(is_dir(designated_block.magic))
				msg += "/";
			msg += " ";
			
	}
	cout<<msg<<endl;
}

// create an empty data file
void FileSys::create(const char *name)
{
	if (strlen(name) > MAX_FNAME_SIZE){
		cout<<"Filename is too long."<<endl;
		return;
	}
	struct dirblock_t current_block;
	struct dirblock_t entry_dir_block;
	bfs.read_block(curr_dir, (void*)&current_block);
	if (current_block.num_entries == MAX_DIR_ENTRIES){
		cout<<"Directory is full."<<endl;
		return;
	}
	bool fileExists = false;
	unsigned int number_of_entries = 0;
	while(number_of_entries < current_block.num_entries){
		bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*)&entry_dir_block);
		if (!is_dir(entry_dir_block.magic)){
			if (strcmp(current_block.dir_entries[number_of_entries].name,name)==0){
				fileExists = true;
				cout<<"File already exists."<<endl;
				return;
			}
		}
		number_of_entries++;
	}
	if (!fileExists){
		short blockSpace = bfs.get_free_block();
		if (blockSpace == 0){
			cout<<"Disk is full."<<endl;
			return;
		}
		struct inode_t dFile;
		dFile.magic = INODE_MAGIC_NUM;
		dFile.size =0;
		for (int i = 0; i < MAX_DATA_BLOCKS; i++)
			dFile.blocks[i] = 0;
		strcpy(current_block.dir_entries[current_block.num_entries].name,name);
		current_block.dir_entries[current_block.num_entries].block_num = blockSpace;
		current_block.num_entries++;
		bfs.write_block(blockSpace,(void*)&dFile);
		bfs.write_block(curr_dir,(void*)&current_block);
	}
}

// append data to a data file
void FileSys::append(const char *name, const char *data)
{
	struct dirblock_t current_block;
    struct inode_t requested_file;
    struct inode_t file_append;
	unsigned int inode_block =0;
    bfs.read_block(curr_dir,(void*) &current_block);
    bool matchingFile = false;
    for(unsigned int found_inode= 0; found_inode < current_block.num_entries; found_inode++){
        bfs.read_block(current_block.dir_entries[found_inode].block_num, (void *) &requested_file);
        if(strcmp(current_block.dir_entries[found_inode].name,name)==0){
			inode_block = found_inode;
            if(is_dir(requested_file.magic)){
            	cout<<"File is a directory."<<endl;
				return;
            }
            else
                matchingFile = true;
        }
    }
    if(!matchingFile){
    	cout<<"File doesn't exist."<<endl;
		return;  
    }
    else{
	    bfs.read_block(inode_block, (void *) &file_append);
	    if((file_append.size + strlen(data))>MAX_FILE_SIZE){
	        cout<<"Appended exceeds maximum file size."<<endl;
			return;
	    }
	    else{
			int current_size = file_append.size;
			bool freeSpot = false;
			int freeSpotindex = 0;
			int block = 0;
			for(freeSpotindex = 0 ; (freeSpotindex < MAX_DATA_BLOCKS)&&(freeSpot = false) ; freeSpotindex++){
				if(file_append.blocks[freeSpotindex] == 0){
					freeSpot = true;
					block = freeSpotindex;
				}
			}  
			int appendedSoFar = 0;
			int dataToAppend = strlen(data);
			int requiredNumberOfBlocks = ceil(double(dataToAppend)/BLOCK_SIZE);
			short freeBlock;
			// If the first block is Free
			if(block == 0){
				if(requiredNumberOfBlocks ==1){
					struct datablock_t newData;
					freeBlock = bfs.get_free_block();
					if(freeBlock == 0){
						cout<<"Disk is full."<<endl;
						return;
					}
					for(int j = 0; j < strlen(data); j++){
							newData.data[j] = data[appendedSoFar];
							appendedSoFar++;
							file_append.size++;
					}
					file_append.blocks[block] = freeBlock;
					bfs.write_block(freeBlock, (void *) &newData);
					bfs.write_block(current_block.dir_entries[inode_block].block_num, (void *) &file_append);
				}
				else{
					for(int i =0; i < requiredNumberOfBlocks; i ++){
						struct datablock_t newData;
						freeBlock = bfs.get_free_block();
						if(freeBlock == 0){
							cout<<"Disk is full."<<endl;
							return;
						}
						for(int j = 0; j < BLOCK_SIZE; j++){
							if(dataToAppend>0){
								newData.data[j] = data[appendedSoFar];
								appendedSoFar++;
								cout<<newData.data[j]<<endl;
								file_append.size++;
								dataToAppend--;
							}
							
						}
						file_append.blocks[block] = freeBlock;
						bfs.write_block(freeBlock, (void *) &newData);
						block++;
						bfs.write_block(current_block.dir_entries[inode_block].block_num, (void *) &file_append);
					}
				}			
			}			
			else{ // If the first block is not Free
				bool open_space =false;
				int curr_block =0;
				int remaining_space=0;	
				block = block-1;
				open_space = true;
				curr_block =file_append.blocks[block];
				if(block !=0)
				remaining_space = BLOCK_SIZE*(block)- current_size;
				else
					remaining_space = BLOCK_SIZE-current_size;
								
				requiredNumberOfBlocks = ceil(double(dataToAppend-remaining_space)/BLOCK_SIZE);
				//Last dataBlock isn't full
				
				if(open_space){
						struct datablock_t newData;
						
						bfs.read_block(curr_block,(void*)&newData);
						for(int j = remaining_space; j < BLOCK_SIZE; j++){
							if(dataToAppend>0){
								newData.data[j] = data[appendedSoFar];
								appendedSoFar++;
								file_append.size++;
								dataToAppend--;
							}
						}
						file_append.blocks[block] = freeBlock;
						bfs.write_block(block, (void *) &newData);
						bfs.write_block(current_block.dir_entries[inode_block].block_num, (void *) &file_append);
					
				}
				//Last dataBlock is full
				else{
					for(int i =0; i < requiredNumberOfBlocks; i ++){
						struct datablock_t newData;
						freeBlock = bfs.get_free_block();
						if(freeBlock == 0){
							cout<<"Disk is full."<<endl;
							return;
						}
						for(int j = 0; j < BLOCK_SIZE; j++){
							if(dataToAppend>0){
								newData.data[j] = data[appendedSoFar];
								appendedSoFar++;
								cout<<newData.data[j]<<endl;
								file_append.size++;
								dataToAppend--;
							}
						}
						file_append.blocks[block] = freeBlock;
						bfs.write_block(block, (void *) &newData);
						bfs.write_block(current_block.dir_entries[inode_block].block_num, (void *) &file_append);
					}
				}
				
		    }
	    }
    }
    
}

// display the contents of a data file
void FileSys::cat(const char *name)
{
	struct inode_t correct_File;
	struct inode_t checkFile;
	struct dirblock_t current_block;
	unsigned int blockNum = 0;
	unsigned int number_of_entries = 0;
	bool fileExists = false;
	bool directory = true;
	string namefile = "";
	bfs.read_block(curr_dir, (void*)&current_block);
	while(number_of_entries < current_block.num_entries){
		bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*)&checkFile);
		if (strcmp(current_block.dir_entries[number_of_entries].name,name) == 0){
			namefile = current_block.dir_entries[number_of_entries].name;
			fileExists = true;
			if (!is_dir(checkFile.magic)){
				directory = false;
				blockNum = current_block.dir_entries[number_of_entries].block_num;
				bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*) &correct_File);
				
			}
		}
		number_of_entries++;
	}

	if (fileExists == false){
		cout<<"File doesn't exist."<<endl;
		return;
	}

	else if (directory){
		cout<<"File is a directory."<<endl;
		return;
	}
	else{
		bfs.read_block(blockNum, (void*) &correct_File);
		char outputmsg[correct_File.size];
		int total_bits = correct_File.size;
		if (correct_File.size != 0){
			int remainder_data = correct_File.size %BLOCK_SIZE;
			int cell = ceil(double(correct_File.size)/BLOCK_SIZE);
			struct datablock_t data_block;
			int total_data_chars =0;
			if(cell ==1){
				bfs.read_block(correct_File.blocks[0],(void*)&data_block);
				for (int j = 0; j < remainder_data; j++)
					outputmsg[j] = data_block.data[j];				
			}
			else{
				for (int i = 0; i < cell; i++){
					bfs.read_block(correct_File.blocks[i],(void*)&data_block);				
					if(i == cell-1){
						for (int j = 0; j < remainder_data; j++){
							outputmsg[total_data_chars] = data_block.data[j];
							total_data_chars++;
						}
					}
					else{
						for(int j=0;j<BLOCK_SIZE;j++){
							outputmsg[total_data_chars] = data_block.data[j];
							total_data_chars++;
						}
					}
				}
			}
			for(int t = 0; t< total_bits;t++)
				cout<<outputmsg[t];
			cout<<endl;
		}
		else
			cout<<"File has size of 0.";
	}
}

// display the last N bytes of the file
void FileSys::tail(const char *name, unsigned int n)
{
	struct inode_t correct_File;
	struct inode_t checkFile;
	struct dirblock_t current_block;
	unsigned int number_of_entries = 0;
	bool fileExists = false;
	int correctblock;
	bool directory = true;
	bfs.read_block(curr_dir, (void*)&current_block);
	while(number_of_entries < current_block.num_entries){
		bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*)&checkFile);
		if (strcmp(current_block.dir_entries[number_of_entries].name,name) == 0){
			fileExists = true;
			if (!is_dir(checkFile.magic)){
				directory = false;
				bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*) &correct_File);
				correctblock = current_block.dir_entries[number_of_entries].block_num;
	
			}
		}
		number_of_entries++;
	}

	if (fileExists == false){
		cout<<"File doesn't exist in current directory."<<endl;
		return;
	}

	if (directory){
		cout<<"File is a directory."<<endl;
		return;
	}
	if(!directory && fileExists){
		if (correct_File.size != 0){
			struct datablock_t dataBlock;
			int total_data = correct_File.size;
			char outputmsg[total_data];
			int total_blocks = ceil(double(total_data)/BLOCK_SIZE);
			int last_block = total_blocks-1;
			int value = (int)n;
			int remainder = total_data%BLOCK_SIZE;
			//Requesting more data than currnetly in file
			if(value >total_data){
				cat(name);
				return;
			}
			//Only print N bits of starting at final data block
			else if(value<BLOCK_SIZE){
				//Only print out contents on final block
				if(value<remainder){
					bfs.read_block(correct_File.blocks[last_block],(void*)&dataBlock);
					for(int i = BLOCK_SIZE-n-1; i< BLOCK_SIZE;i++)
						//msg += dataBlock.data[i];
					continue;
				}
				else{
					int dataonlastblock = total_data-total_blocks*BLOCK_SIZE;
					int data_from_other_block =  n-dataonlastblock-1;
					bfs.read_block(correct_File.blocks[last_block-1],(void*)&dataBlock);
					for(int i = BLOCK_SIZE-data_from_other_block;i<BLOCK_SIZE;i++)
						//msg += dataBlock.data[i];
					continue;
					bfs.read_block(correct_File.blocks[last_block],(void*)&dataBlock);
					for(int i = 0;i<dataonlastblock;i++)
						//msg += dataBlock.data[i];	
						continue;
				}
			}
			//Must read multiple blocks
			else{
				int numberofblocks = ceil(double(value)/ BLOCK_SIZE);	
				int start_block = total_blocks-numberofblocks;
				for(int i = start_block;i<total_blocks-1;i++){
					bfs.read_block(correct_File.blocks[i],(void*)&dataBlock);
						for (int j = 0; j < BLOCK_SIZE; j++)
							//msg += dataBlock.data[j];
						continue;
				}
				bfs.read_block(correct_File.blocks[total_blocks],(void*)&dataBlock);
						for (int j = 0; j < remainder; j++)
							//msg += dataBlock.data[j];
						continue;
			}
		for(int w = 0; w<total_data;w++)
			cout<<outputmsg[w]<<endl;
		}
	}	
}

// delete a data file
void FileSys::rm(const char *name)
{
	struct inode_t block_to_remove;
	struct dirblock_t current_block;
	bfs.read_block(curr_dir, (void*)&current_block);
	unsigned int number_of_entries = 0;
	bool fileExists = false;
	bool directory = true;
	unsigned int k_entry = 0;
	int removeindex = 0;
	int removeData = 0;
	while (number_of_entries < current_block.num_entries) {
		bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*)&block_to_remove);
		if (strcmp(current_block.dir_entries[number_of_entries].name,name) == 0){
			fileExists = true;
			if (!is_dir(block_to_remove.magic)){
				directory = false;
				removeindex = current_block.dir_entries[number_of_entries].block_num;
				k_entry = number_of_entries;
			}
		}
		number_of_entries++;
	}

	if (!fileExists)
	{
		cout<<"File doesn't exist."<<endl;
		return;
	}
	else{

		if (directory){
			cout<<"File is a directory."<<endl;
			return;
		}
        else{
			if (k_entry == current_block.num_entries){
				bfs.read_block(current_block.dir_entries[k_entry].block_num,(void*)&block_to_remove);
				for (int i = 0; i < MAX_DATA_BLOCKS; i++){
					if (block_to_remove.blocks[i] != 0){
						removeData = block_to_remove.blocks[i];
						block_to_remove.blocks[i] = 0;
						bfs.reclaim_block(removeData);
					}
				}
				bfs.write_block(current_block.dir_entries[k_entry].block_num,(void*)&block_to_remove);
				current_block.dir_entries[k_entry].block_num = 0;
				current_block.num_entries--;
			} 
			else{
				bfs.read_block(current_block.dir_entries[k_entry].block_num,(void*)&block_to_remove);
				for (int i = 0; i < MAX_DATA_BLOCKS; i++){
					if(block_to_remove.blocks[i] != 0 ){
						removeData = block_to_remove.blocks[i];
						block_to_remove.blocks[i] = 0;
						bfs.reclaim_block(removeData);
					}
				}
					
				bfs.write_block(current_block.dir_entries[k_entry].block_num,(void*)&block_to_remove);

				for (unsigned int i = k_entry; i < current_block.num_entries; i++){
					for (int j = 0; j < MAX_FNAME_SIZE + 1; j++)
						strcpy(&current_block.dir_entries[i].name[j],&current_block.dir_entries[i+1].name[j]);
					current_block.dir_entries[i].block_num = current_block.dir_entries[i+1].block_num;
				}
				current_block.dir_entries[current_block.num_entries].block_num = 0;
				current_block.num_entries--;
			}
        }
		bfs.write_block(curr_dir,(void*)&current_block);
		bfs.reclaim_block(removeindex);
	}
}

// display stats about file or directory
void FileSys::stat(const char *name)
{
	struct inode_t appendFile;
	struct inode_t checkFile;
	struct dirblock_t current_block;
	//int blockNum = 0;
	unsigned int number_of_entries = 0;
	bool fileExist = false;
	bfs.read_block(curr_dir, (void*)&current_block);
	while(number_of_entries < current_block.num_entries){
		bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*)&checkFile);
		if (strcmp(current_block.dir_entries[number_of_entries].name,name) == 0){
			fileExist = true;
			if (is_dir(checkFile.magic)){
				cout << "Directory name: " << current_block.dir_entries[number_of_entries].name <<"/"<< endl;
				cout << "Directory block number: " <<current_block.dir_entries[number_of_entries].block_num << endl;
			}

			else {
				int number_of_blocks = 0;
				bfs.read_block(current_block.dir_entries[number_of_entries].block_num,(void*)&appendFile);
				int startblock =appendFile.blocks[0];
				for (int i = 0; i < MAX_DATA_BLOCKS; i++){
					if (appendFile.blocks[i] != 0)
						number_of_blocks++;
				}
				cout << "Inode block number: " << current_block.dir_entries[number_of_entries].block_num << endl;
				cout << "Bytes in file: " << appendFile.size << endl;
				cout << "Data block counts: " << number_of_blocks<< endl;
				cout<<"First data block number: "<<startblock<<endl;
			}
		}
		number_of_entries++;
	}
	if (!fileExist)
		cout<<"File doesn't exist."<<endl;
}
