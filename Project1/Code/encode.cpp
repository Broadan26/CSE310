#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <math.h>
#include <sys/time.h>

using namespace std; //Because lazy

/*Forward Declarations*/
void insertionSort(int length);
void mergeSort(int left, int right);
void merge(int left, int middle, int right);
void shift();
void lastElementString();
string IntToString(int value);

struct timespec starttime, endtime, ttime; //Assits with algorithm efficiency tracking
double timeinterval = 0; //Time result of sorting Strings

int comparisons = 0; //Tracks number of sort comparisons made
int swaps = 0; //Tracks number of sort swaps made

string line; //Current line from the text document
string* tempArray; //String array for holding the derivations of the line
string lastColumn; //String to hold the encoded last column

int size = 0; //Tracks the size of the array
int count = 0; //Counts number of elements in the arrays
int check = 0; //Checks when to create a newline

int main(int argc, char** argv)
{
	string action1 = argv[0]; //Holds the first argument passed
	string action2 = argv[1]; //Holds the second argument passed
	char commands[2][20] = {"insertion", "merge"}; //Basic Commands for main
	
	if(argc >= 2) //Make sure input parameters are correct
	{
		if(action1 == commands[0] || action2 == commands[0]) //Check for keyword Insertion Sort
		{
			//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &starttime); //Start clock for analysis purposes

			while(getline(cin, line)) //Loop to run through the file
			{
				if(check == 1)
					cout << endl;
				check = 1;
					
				shift();
				insertionSort(size); //Insertion Sort
				lastElementString();
					
				delete[] tempArray; //Clean the mess created by calling new
			}
				
			/*Record the time data */
			//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endtime);
			//ttime.tv_sec = endtime.tv_sec - starttime.tv_sec;
			//ttime.tv_nsec = endtime.tv_nsec - starttime.tv_nsec;
			//timeinterval = ttime.tv_sec * 1000000000 + ttime.tv_nsec;
				
			/*Print time data and analysis of sort*/
			//cout << "Number of comparisons made by insertion sort: " << comparisons << endl;
			//cout << "Number of swaps made by insertion sort: " << swaps << endl;
			//cout << "Time to run insertion sort: " << (double) timeinterval/1000000 << endl;
		}
		else if(action1 == commands[1] || action2 == commands[1]) //Check for keyword Merge Sort
		{
			//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &starttime); //Start clock for analysis purposes

			while(getline(cin, line)) //Loop to run through the file
			{
				if(check == 1)
						cout << endl;
				check = 1;
					
				shift();
				mergeSort(0, size -1); //Merge Sort
				lastElementString();
					
				delete[] tempArray; //Clean the mess created by calling new
			}
				
			/* Record the time data */
			//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &endtime);
			//ttime.tv_sec = endtime.tv_sec - starttime.tv_sec;
			//ttime.tv_nsec = endtime.tv_nsec - starttime.tv_nsec;
			//timeinterval = ttime.tv_sec * 1000000000 + ttime.tv_nsec;
				
			/*Print time data and analysis of sort*/
			//cout << "Number of comparisons made by insertion sort: " << comparisons << endl;
			//cout << "Number of swaps made by insertion sort: " << swaps << endl;
			//cout << "Time to run insertion sort: " << (double) timeinterval/1000000 << endl;
		}
		else //If all else fails let user know of problem
			cout << "Incorrect arguments" << endl;
	}
	else //Let user know there were not enough parameters passed
		cout << "Not enough arguments" << endl;
	
	return 0;
}

/*Helper function to convert int to String*/
string IntToString(int value)
{
	ostringstream temp;
	temp << value; //Convert int to string
	return temp.str();
}

/*Character Shift*/
/*Swaps the position of characters in the current string*/
void shift()
{
	size = 0;; //Isolate the word
	size = line.length(); //Find the size of the word
	
	if(size > 0)
	{
	tempArray = new string[size]; //Create new array to hold characters
	tempArray[0] = line;
	}
	else //Condition for just a newline char
	{
		tempArray = new string[1];
		tempArray[0] = "\n";
	}
	if(size >= 2) //Only perform shifts on something worth shifting
	{
		for(int i = 1; i < size; i++) 
		{
			tempArray[i] = tempArray[i-1]; //Use pointer to copy
			for(int j = 0; j < size; j++)
			{
				tempArray[i][j] = tempArray[i][j+1]; //Shift characters[1...n]
			}
			tempArray[i][size-1] = tempArray[i-1][0]; //Shift character[0 to n]
		}
	}
}

/*lastElementString*/
/*Prints out the encoded string*/
void lastElementString()
{
	lastColumn = "";
	int count = 1;
	int indexValue = 0;
	char key;
	
	for(int i = 0; i < size; i = i + count)
	{
		count = 1; //Reset count
		key = tempArray[i][size - 1]; //Current character
		int j = i+1; //Don't interate over what is already done

		for(j = i + 1; j < size;j++ ) //Check for clusters
		{
			if(key == tempArray[j][size - 1])
				count++; //Increase count for clusters
			else
				break;
		}
		string countS = IntToString(count); //Convert count to a string
		lastColumn = lastColumn + countS; //Add the cluster count to string
		lastColumn = lastColumn + " "; //Add an extra space between characters	
		lastColumn = lastColumn + tempArray[i][size - 1]; //Add character to string
		if(i + count < size) //Only add spaces for n-1 encrypted slots
			lastColumn = lastColumn + " "; //May need to remove extra spaces in decryption
	}
	
	//Find the index value of the sorted word
	for(int i = 0; i < size; i++)
	{
			if(tempArray[i] == line)
				indexValue = i;
	}
	cout << indexValue << endl; //Check for accuracy & import to file
	cout << lastColumn; //Check for accuracy & import to file
}

/*Insertion Sort*/
/*Sorts an array of strings by their ASCII characters*/
void insertionSort(int length)
{
	string key;
	
	if(length <= 1)
	{
		return; //Skip sorting if not worth doing
	}

	int j;
	for(int i = 1; i < length; i++)
	{
		key = tempArray[i];
		for(j = i - 1; j >= 0 && tempArray[j] > key; j--)
		{
			comparisons++;
			tempArray[j+1] = tempArray[j];
		}
		tempArray[j+1] = key;
		swaps++;
	}
}

/*Merge Sort*/
/*Sorts an array of strings by their ASCII characters*/
void mergeSort(int left, int right)
{
	int middle = 0;
	if(left >= right)
		return;

	if(left < right)
	{
		middle = floor((left + right) / 2);
		mergeSort(left, middle);
		mergeSort(middle + 1, right);
		merge(left, middle, right);
	}
}

/*Utility method that assists with mergeSort() for merging arrays*/
void merge(int left, int middle, int right)
{
	int i = 0, j = 0, k = 0;
	string temp[right+1]; //Only need as many string slots as carried over

	i = left;
	k = left;
	j = middle + 1;
	
	while(i <= middle && j <= right)
	{
		comparisons++;
		if(tempArray[i] < tempArray[j])
		{
			temp[k] = tempArray[i];
			i++;
		}
		else 
		{
			temp[k] = tempArray[j];
			j++;
		}
		k++;
	}
	if(i > middle)
	{
		for(int h = j; h <= right; h++) 
		{
			temp[k] = tempArray[h];
			k++;
		}
	}
	else
	{
		for(int h = i; h <= middle; h++) 
		{
			temp[k] = tempArray[h];
			k++;
		}
	}
	for(int i = left; i <= right; i++) //Copy from low to high
	{
		tempArray[i] = temp[i];
	}
	swaps++;
}
