# Abstractor: Multithreaded Scientific Search Engine

## Introduction
In this project, we have developed a multithreaded scientific search engine in C++ that queries the paper abstracts and summarizes the most relevant ones. The Abstractor is a research tool for scientific literature that focuses on delivering fast and accurate results for given queries. The program takes a query as input, scans its file collection, finds the most relevant abstracts by measuring the similarities between abstracts and the given query, then retrieves convenient sentences from the selected abstracts.

## Project Description
The Abstractor is powered by multithreading, which accelerates text processing. The program uses threads to process each abstract independently and simultaneously. Each thread logs its name and the name of the file it is processing to the output file. The threads evenly distribute the job, ensuring that no thread is idle while one thread does the job. The Abstractor uses the Jaccard Similarity metric to determine the similarity between two texts, which finds how close the two texts are to each other in terms of the number of common words that exist over total words.

## Input & Output
The final submission compiles and runs with the following commands:

```
make
./abstractor.out input_file_name.txt output_file_name.txt
```

The Abstractor.out program takes two command line arguments:

1. **input_file_name.txt**: Path of the input file
2. **output_file_name.txt**: Path of the output file

### Input
The input_file_name.txt file contains several parameters, which are read line-by-line:
1. The first line contains values of T, A, and N, which are separated by spaces. T: Number of threads (other than the main thread) that will be created for the session. A: Number of abstracts that will be scanned by the threads. N: Number of abstracts that will be returned and summarized.
2. The second line contains the words to query.
3. From lines 3 to A+2, the names of the abstract files will be listed as follows:<br>
abstract1.txt<br>
abstract2.txt<br>
...<br>
abstractA.txt


4. An input file consists of several lines of sentences that are pre-processed and separated by dots. An example abstract consisting of two sentences, all in lowercase, and separated from punctuation, is provided below.

> the covid19 pandemic caused by sarscov2 remains a significant issue for global health economics and society . a wealth of data has been generated since its emergence in december 2019 and it is vital for clinicians to keep up with this data from across virus the world at a time of uncertainty and constantly evolving guidelines and clinical practice .

### Output
The Abstractor outputs the thread operation logs, scores, and resulting sentences to the given text file.

* Each thread outputs its name and operation as "Thread X is calculating abstract Y.txt".
* After each separate operation, "###" is inserted. That is, after finishing thread jobs, listing result 1, result 2, and so on.

## Thread Creation
The program creates T number of threads to evenly distribute the job. You should name your threads alphabetically and start from A and continue with B, C, and so on. You should use POSIX threads provided by Linux-based systems or MacOS for the implementation of this project to create a multithreaded environment. To address the synchronization issues, you can use mutex and semaphores.

## Similarity Measurement
In the Abstractor program, the Jaccard Similarity metric is used to determine the similarity between two texts. This metric finds how close the two texts are to each other in terms of the number of common words that exist
