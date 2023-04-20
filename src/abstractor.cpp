/**
 * @file abstractor.cpp
 * @author Salim Kemal Tirit
 * 2019400153
 *
 * @brief Multi theaded code for comparing the abstracts and the search words using Jacard similarity.
 *  
 * In this code we are trying to split the job to threads and make it parellel. This provides us faster
 * execution of the job since threads may run concurrently. The job I am referring
 * to is to calculate and compare the similarities between abstracts and the search words that are
 * provided. The similarity calculation is done by using Jaccard Similarity. Also we create summaries
 * of the abstracts using the sentences which contain the given words. I used mutex locks to provide 
 * the data safety. Finally we are expected to return
 * the best N abstracts with respect to the similarity score. Also the code tries to spread the job
 * evenly between threads. Every thread prints its ID and on which abstract it is working to the output
 * file. Input file needs to contain the number of threads, number of abstracts to be read and compared
 * and the expected number of returns in the first line as integers seperated by space. Second line
 * needs to contain the search words in lowercase seperated by space. And starting from the third line
 * name of the abstracts that needs to be checked one in a line. Abstracts need to be placed in the 
 * abstracts folder.
 * This project intends to teach the multithreaded programming with its pros. And also how to handle
 * data read-write operations without corrupting the data. This helpes us understand the producer consumer
 * problem.
 *
 * How to compile and run:
 *
  make
  ./abstractor.out <INPUT_FILE_PATH> <OUTPUT_FILE_PATH>
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <iterator>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <queue>

#include <stdio.h>
#include <unistd.h>


using namespace std;

set<string> myquery;

pthread_mutex_t jobs_lock;      // mutex lock for job queue
pthread_mutex_t lines_lock;     // mutex lock for printing to the output file
pthread_mutex_t results_lock;   // mutex lock for similarity score map
pthread_mutex_t summaries_lock; // mutex lock for summary map

multimap<double, string> results;   //key = similarity score, value = name of the abstract
multimap<string, string> summaries; //key = name of the abstract, value = summary
queue<string> jobs;                 // holds the abstracts when a thread is idle it comes and gets the next job


ofstream outfile;

//runner code for threads.
void* runner(void* threadID) {
	char id = *((char*) threadID); //ID of the thread. Uppercase letters 'A'-'Z'
	
	while(true) {
        // TODO: If you uncomment the line below the threads are better distributed
        // but i dont think this is a good practice since it slows down the execution
        // of the program. If we dont use it the scheduler decides which thread is
        // going to work. This may result in not-evenly distributed thread execution.
        // sleep(1);

		string job;
		pthread_mutex_lock(&jobs_lock);
		if(!jobs.empty()){
			job  = jobs.front(); // thread takes a new job from the queue
			jobs.pop();
		}else{
			pthread_mutex_unlock(&jobs_lock);
			break;
		}
		pthread_mutex_unlock(&jobs_lock);

        pthread_mutex_lock(&lines_lock);
        outfile << "Thread "+string(1,id)+" is calculating "+job+"\n"; // thread prints what it is doing to output file
		pthread_mutex_unlock(&lines_lock);
        ifstream inputs;
		string fileDirectory = "../abstracts/" + job; // reads the abstract from abstracts file
		inputs.open(fileDirectory);

		string word;
		vector<vector<string>> abstract;  //whole abstract sentence by sentence and word by word
    	vector<string> sentence;          // a sentence word by word 
    	set<string> set_abstract;         // set for every word in abstract
    	while (inputs.good())             // reads abstract until the end of it
    	{
        	inputs >> word;
        	sentence.push_back(word);
        	if (word == ".")
        	{
            	abstract.push_back(sentence);
            	sentence.clear();
        	}
        	set_abstract.insert(word);
    	}
    	set<string> unionOfSets;   // union of abstract set and query set.
    	set_union(set_abstract.begin(),set_abstract.end(),
        	            myquery.begin(),myquery.end(),
            	        inserter(unionOfSets,unionOfSets.begin()));
    
    	set<string> intersectionOfSets;    // intersection of abstract set and query set.
    	set_intersection(set_abstract.begin(),set_abstract.end(),
        	                myquery.begin(),myquery.end(),
            	            inserter(intersectionOfSets,intersectionOfSets.begin()));

    	double similarity = (double)intersectionOfSets.size() / unionOfSets.size(); // Jaccard Similarity score.
    
    	pthread_mutex_lock(&results_lock);
    	results.insert({similarity,job});      //inserting the score to the results map
    	pthread_mutex_unlock(&results_lock);

    	string summary = "";       // creating the summary.
    	for (int a = 0; a < abstract.size(); a++)
		{
        	for (int j = 0; j < abstract[a].size(); j++)
        	{
        		auto pos = myquery.find(abstract[a][j]);
            	if(pos != myquery.end()){
                	string summary_sentence = "";
                	for (int k = 0; k < abstract[a].size(); k++)
                	{       
                    	summary_sentence += abstract[a][k] + " ";
                	}
                	summary += summary_sentence;
                	break;                     
            	}
        	}
    	}


    	pthread_mutex_lock(&summaries_lock);
    	summaries.insert({job,summary});       //inserting the summary to the summaries map
    	pthread_mutex_unlock(&summaries_lock);

    	inputs.close();
	}

    pthread_exit(0);
}	

int main(int argc, char const *argv[])
{
    string inputFile = argv[1];
    string outputFile = argv[2];

    ifstream infile;
    infile.open(inputFile);

    int T, A, N;

    infile >> T; // Number of threads other than the main thread.
    infile >> A; // Number of abstracts that will be scanned by the threads. 
    infile >> N; // Number of abstracts that will be returned and summerized. 

    string line;
    getline(infile, line); // first line
    getline(infile, line); // second line

    stringstream check1(line);

    string intermeadiate;
    // taking the queries and storing them in a set
    while (getline(check1, intermeadiate, ' '))
    {
        myquery.insert(intermeadiate);
    }

    string temp;
    // getting abstract names
    for (int i = 0; i < A; ++i)
    {
        infile >> temp;
    	jobs.push(temp);
    }
    infile.close();

    outfile.open(outputFile);

    // I dont want to create more threads than the number of abstracts since it is unecessary. 
    int thread_number = min(T,A); 

    pthread_t threadidentifiers[thread_number]; // thread_number of thread identifiers.
    pthread_attr_t attr;
    char threadIDs[thread_number];      // thread_number of  thread ids starting with 'A' up to 'Z'
    for (int i = 0; i < thread_number; ++i)
    {
     	threadIDs[i] = 'A'+i;
    }
    setvbuf(stdout, NULL, _IONBF, 0);
    pthread_attr_init(&attr);

    for (int i = 0; i < thread_number; ++i)
    {
      	pthread_create(&threadidentifiers[i], &attr, runner, &threadIDs[i]); // creating threads
    }

    for (int i = 0; i < thread_number; ++i)
    {
    	pthread_join(threadidentifiers[i],NULL); // waiting for the threads to return.
    }

    // writing to the output file. File name, similarity score and summary. 
    outfile << "###\n";
    int r = 1;
    for(auto itr = results.rbegin(); itr != results.rend() && N >= r; ++itr,++r){
    	auto pos = summaries.find(itr->second);
    	outfile << "Result " << r << ":" << endl;
    	outfile << "File: " << itr->second << endl;
    	outfile << fixed << setprecision(4);
    	outfile << "Score: " << itr->first << endl;
    	outfile << "Summary: "  << pos->second<< endl;
    	outfile << "###" << endl;
    }

    
    return 0;
}