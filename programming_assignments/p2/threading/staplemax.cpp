//staplemax.cpp
//Sean Wright
//cpsc 3500

const int Num_Threads = 20;
const int Max_Pens = 100;
const int Max_Paper =200;
const int Max_Toner =40;
const int Max_Laptops = 15;
int Num_pens;
int Num_paper;
int Num_toner;
int Num_laptops;
#include <cstring>
#include <iostream>
#include<string.h>
#include<stdio.h> 
#include<pthread.h> 
#include<fstream>
#include <unistd.h>
using namespace std;
struct data{
	string fname;
	int paper_reset;
	int pens_reset;
	int laptops_reset;
	int toner_reset;
};

void *function(void * parameter);
using namespace std;
pthread_mutex_t mutex;
int main(){
	//Set filename array to point to 20 text files
	string filename[Num_Threads];
	const string header ="/home/fac/lillethd/cpsc3500/projects/sales_data/sales";
	const string end = ".txt";
	Num_paper = Max_Paper;
	Num_laptops =Max_Laptops;
	Num_pens = Max_Pens;
	Num_toner = Max_Toner;
	pthread_t thread_id [Num_Threads];
	
	string list[20] = {"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20"};
	for(int i =0;i<Num_Threads;i++){
		data *dataptr = new data;
		filename[i]=header+list[i]+end;
		dataptr->fname = filename[i];
		dataptr->paper_reset = 0;
		filename[i]=header+list[i]+end;
		int error = pthread_create(&thread_id[i],NULL,function,(void*)dataptr);
		if(error){
			cout<<"Error creating pthread "<<i+1<<endl;
			return -1;
		}
	}
	sleep(1);
	data * getcount;
	int pens_reset1=0;
	int paper_reset1=0;
	int toner_reset1=0;
	int laptops_reset1=0;
	for(int i =0;i<Num_Threads;i++){
		
		int error = pthread_join(thread_id[i],(void**)&getcount);
		if(error){
			cout<<"Error joining pthread "<<(i+1)<<endl;
			return -1;
		}
		paper_reset1 += getcount->paper_reset;
		pens_reset1 += getcount-> pens_reset;
		laptops_reset1 += getcount-> laptops_reset;
		toner_reset1 += getcount-> toner_reset;
		delete getcount;
	}
	int total_sales = pens_reset1*Max_Pens+toner_reset1*Max_Toner+paper_reset1*Max_Paper+laptops_reset1*Max_Laptops;
	total_sales +=((Max_Paper-Num_paper)+(Max_Laptops-Num_laptops)+(Max_Pens-Num_pens)+(Max_Toner-Num_toner));
	cout<<"Final inventory:\nPens: "<<Num_pens<<"\nPaper: "<<Num_paper<<"\nLaptops: "<<Num_laptops<<"\nToner: "<<Num_toner<<"\nTotal sales: "<<total_sales<<endl;
	//cout<<"Pens reset "<<pens_reset1<<". Toner reset "<<toner_reset1<<". Laptops reset "<<laptops_reset1<<". Paper reset "<<paper_reset1<<endl;
	return 0;
}


void *function(void * parameter){
	data *datastruct =(data *)(parameter);
	string word = datastruct->fname;
	int num_pens_reset=0;
	int num_paper_reset=0;
	int num_toner_reset=0;
	int num_laptops_reset=0;
	ifstream filename(word);
	string name;
	int count =0;
	int errvalue;
	if(!filename.is_open()){
		cout<<"File incorrect"<<endl;
		return NULL;
	}
	else{
		errvalue = pthread_mutex_lock(&mutex);
		if(errvalue!=0){
			cout<<"Couldn't lock mutex."<<endl;
			return NULL;
		}
		while(getline(filename,name)){
			count++;
			if(name == "pen"){
				Num_pens = Num_pens-1;
				if(Num_pens==0){
					Num_pens = Max_Pens;
					num_pens_reset++;
				}
				
			}
			else if(name=="paper"){
				Num_paper = Num_paper-1;
				if(Num_paper==0){
					Num_paper = Max_Paper;			
					num_paper_reset++;			
				}				
			}
			else if (name=="laptop"){
				Num_laptops = Num_laptops-1;
				if(Num_laptops==0){
					Num_laptops = Max_Laptops;	
					num_laptops_reset++;
				}				
			}
			else if(name =="toner"){
				Num_toner = Num_toner-1;
				if(Num_toner==0){
					Num_toner = Max_Toner;
					num_toner_reset++;
				}				
			}
			else{
				cout<<"Incorrect string value"<<endl;				
			}
		}
		errvalue = pthread_mutex_unlock(&mutex);
		if(errvalue !=0){
			cout<<"Couldn't unlock mutex."<<endl;
			return NULL;
		}
		filename.close();
	}
	datastruct->paper_reset = num_paper_reset;
	datastruct-> pens_reset = num_pens_reset;
	datastruct-> laptops_reset = num_laptops_reset;
	datastruct-> toner_reset = num_toner_reset;
	cout.flush();
	pthread_exit(datastruct);
	return NULL;
}
