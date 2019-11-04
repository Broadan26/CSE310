#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <math.h>
#include <sys/time.h>

using namespace std; //Because lazy

/*Forward Declarations*/
void parseLine();
void insertionSort(int length);
void mergeSort(int left, int right);
void merge(int left, int middle, int right);
void findNext();
void createLine();

struct timespec starttime, endtime, ttime; //Assits with algorithm efficiency tracking
double timeinterval = 0; //Time result of sorting Strings

int swaps = 0; //Tracks number of sort swaps made
int comparisons = 0; //Tracks number of sort comparisons made

int fileIndex; //Holds the index from the file
int size = 0; //Holds the size of the string being analyzed
int* next; //Holds the values needed to decode the string
string line; //Holds the input from the redirected text file
string encodedLine; //Holds the encrypted string from the file
int check = 0; //Checks for newline condition

string lastColumn = ""; //Holds the sorted encoded line
string firstColumn = ""; //Holds the parsed encoded line
string originalLine = ""; //Holds the decoded line

/*Driver code*/
int main(int argc, char** argv)
{
	string action1; //Holds the first argument passed
	string action2; //Holds the second argument passed
	char commands[2][20] = {"insertion", "merge"}; //Basic Commands for main
	
	if(argc >= 2) //Check for apropriate conditions
	{
		action1 = argv[0]; //Holds the first argument passed
		action2 = argv[1]; //Holds the second argument passed
		
		if(	action1 == commands[0] || action2 == commands[0]) //Check keyword for Insertion Sort
		{
			while(getline(cin, line)) //While the text file has lines to read
			{
				/* Basic information needed to perform decryption*/
				fileIndex = atoi(line.c_str()); //Convert the string to an integer
				getline(cin, encodedLine); //Get the encrypted line from the file
				
				size = encodedLine.size(); //Size of the encoded line
				
				if(size > 1)
				{
					if(check != 0)
						cout << endl;
					parseLine(); //Parse the encoded line
					insertionSort(size); //Insertion Sort
					findNext(); //Create the Next integer array
					createLine(); //Create the decoded line & print it
					check = 1; //Condition for extra line

					delete next; //Clean up the memory mess
				}
				else //For newlines
				{
					check = 1;
					cout << endl;
				}
				
				lastColumn = ""; //Clear the last parsed last column
				firstColumn = ""; //Clear the last parsed first column
			}
		}
		else if(action1 == commands[1] || action2 == commands[1]) //Check keyword for Merge Sort
		{
			while(getline(cin, line)) //While the text file has lines to read
			{
				/* Basic information needed to perform decryption*/
				fileIndex = atoi(line.c_str()); //Convert the string to an integer
				getline(cin, encodedLine); //Get the encrypted line from the file
				
				size = encodedLine.size(); //Size of the encoded line
				
				if(size > 1)
				{
					if(check != 0)
						cout << endl;
					parseLine(); //Parse the encoded line
					firstColumn = lastColumn; //Copy the string
					mergeSort(0, size - 1); //MergeSort
					findNext(); //Create the Next integer array
					createLine(); //Create the decoded line & print it
					check = 1;
					
					delete next; //Clean up the memory mess
				}
				else //For newlines
				{
					check = 1;
					cout << endl;
				}
				
				lastColumn = ""; //Clear the last parsed last column
				firstColumn = ""; //Clear the last parsed first column
			}
		}
	}
	else //Let user know about problem with arguments
		cout << "Incorrect arguments" << endl;
	
	return 0;
}

/*CreateLine*/
/*Creates the decoded line using the Next array and both column strings*/
void createLine()
{
	char nextChar; //Holds the character in lastColumn
	int nextVal; //Holds the value from array Next
	nextVal = next[fileIndex]; //Extract value from Next array
	
	for(int i = 0; i < size; i++)
	{
		nextChar = lastColumn[nextVal]; //Use value to locate char in lastColumn
		originalLine = originalLine + nextChar; //Concatenate the char on to the string
		nextVal = next[nextVal]; //Keep iterating Next array
	}
	
	cout << originalLine; //Print to file
	originalLine = ""; //Clear the line
}

/*FindNext*/
/*Iterates thru the first column string and compares chars to the last column*/
/*Stores the integer index value of the next character in an integer array*/
void findNext()
{
	next = new int[size]; //Integer array to hold next values
	char currentChar; //Current char being searched for
	
	for(int i = 0; i < size; i++) //Avoid comparing 0 location by accident
		next[i] = -1;
	
	for(int i = 0; i < size; i++) //Iterates thru sorted column
	{
		currentChar = firstColumn[i]; //Store the current char being looked at
		
		for(int j = 0; j < size; j++) //Iterates thru last column
		{
			char stop = 'n';
			
			if(currentChar == lastColumn[j]) //If matched determine location
			{
				for(int k = 0; k <= i; k++) //Check next for repeats
				{	
					if(next[k] == j) //Break if identical value found
					{
						k = i; //Break
						stop = 'n'; //Break
					}
					else //Otherwise keep the value
					{ 
						stop = 'y'; //Keep
					}
				}
			}
			
			if(stop == 'y') //Check to see if loop should end
			{
				next[i] = j; //Assign to next
				j = size; //Break out of loop
			}
		}
	}
}

/*ParseLine*/
/*Undoes the encryption of the final column*/
void parseLine()
{
	int location = 0; //Location in the line being looked at
	string numberS = ""; //Number of elements to add from encoded line
	int number = 0; //Number of elements to add
	char currentChar; //Current char being added to lastColumn
	
	size = encodedLine.size();
	//cout << "Line size is: " << size << endl;
	
	while(location < size) //Iterate the entire encrypted string
	{
		while(encodedLine[location] != ' ') //Spaces as delimiters
		{
			numberS = numberS + encodedLine[location]; //Parse the number of characters
			location++; //Move the location down to continue the loop
		}
		location++; //Move 1 spot further in the line for delimiter
		currentChar = encodedLine[location]; //Store the char being looked at
		//cout << "Current char is: " << currentChar << endl; //Check to make sure current char is correct
		
		location = location + 2; //Move to the next counting number
		
		number = atoi(numberS.c_str()); //Convert the string to an integer
		numberS = ""; //Clear the string
		
		//cout << "Number of current char is: " << number << endl; //Check to make sure the number is correct
		
		for(int i = 0; i < number; i++)
			lastColumn = lastColumn + currentChar; //Concatenate the string array by number of chars
		
		//cout << lastColumn << endl; //Check to make sure it's working correctly
	}
	
	size = lastColumn.size(); //Change the size properly
	
	//cout << lastColumn << endl; //Check to make sure it's working correctly
}

/*Insertion Sort*/
/*Sorts an array of strings by their ASCII characters*/
void insertionSort(int length)
{
	char key;
	firstColumn = lastColumn;
	
	if(length <= 1)
	{
		return; //Skip sorting if not worth doing
	}

	int j;
	for(int i = 1; i < length; i++)
	{
		key = firstColumn[i];
		for(j = i - 1; j >= 0 && firstColumn[j] > key; j--)
		{
			comparisons++;
			firstColumn[j+1] = firstColumn[j];
		}
		firstColumn[j+1] = key;
		swaps++;
	}
}

/*Merge Sort*/
/*Sorts an array of strings by their ASCII characters*/
void mergeSort(int left, int right)
{
	int middle; 

	if(left < right)
	{
		middle = floor((left + right) / 2);
		mergeSort(left, middle);
		mergeSort(middle + 1, right);
		merge(left, middle, right);
	}
}

/*Merge*/
/*Utility method that assists with mergeSort() for merging arrays*/
void merge(int left, int middle, int right)
{
	int i = 0, j = 0, k = 0;
	char temp[right + 1]; //Only need as many char slots as carried over

	i = left;
	k = left;
	j = middle + 1;
	
	while(i <= middle && j <= right)
	{
		comparisons++;
		if(firstColumn[i] <= firstColumn[j])
		{
			temp[k] = firstColumn[i];
			i++;
		}
		else 
		{
			temp[k] = firstColumn[j];
			j++;
		}
		k++;
	}
	
	while(i <= middle)
	{
			temp[k] = firstColumn[i];
			k++;
			i++;
	}
	
	while(j <= right)
	{
			temp[k] = firstColumn[j];
			k++;
			j++;
	}
	
	for(i = left; i < k; i++) //Copy from low to high
	{
		firstColumn[i] = temp[i];
	}
	swaps++;
}
