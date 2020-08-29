#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

using namespace std; //Because lazy

/*Struct Definitions */
#define STATE_LEN 25
#define MONTH_LEN 10
#define EVENT_LEN 26
#define COUNTY_LEN 110
#define TOR_LEN 4
#define DATE_LEN 20
#define FAT_LOC 25

/*Annual Storms*/
/*Holds the year of the storms and points to storm events*/
struct annual_storms
{
	int year; // Year of storm events
	int array_size; //Array size
	struct storm_event *events;  // Pointer to array of storm events for the given year
};

/*Storm_Event*/
/*Holds the details for the storm*/
struct storm_event
{
	int event_id; // Event id
	char state[ STATE_LEN ]; // State name
	int year; // Four digit year of event
	char month_name[ MONTH_LEN ]; // Month of event
	char event_type[ EVENT_LEN ]; // Event type
	char cz_type; // Where event happened, C, Z, or M
	char cz_name[ COUNTY_LEN ]; // Name of county/zone
	int injuries_direct; // Number of direct injuries
	int injuries_indirect; // Number of indirect injuries
	int deaths_direct; // Number of direct deaths
	int deaths_indirect; // Number of indirect deaths
	int damage_property; // Amount of property damage; convert to integer
	int damage_crops; // Amount of crop damage; convert to integer
	char tor_f_scale[ TOR_LEN ]; // Strength of tornado on Fujita scale
    struct fatality_event *f; // Linked list of fatalities associated with this storm event
};

/*Fatality Event*/
/*Holds the details for the fatalities based on storm*/
struct fatality_event
{
    int fatality_id; // Identifier of the fatality
    int event_id; // Identifier of the storm event
    char fatality_type; // D or I
    char fatality_date[ DATE_LEN ]; // Date and time of fatality
    int fatality_age; // Age of fatality
    char fatality_sex; // Gender of fatality
    char fatality_location[ FAT_LOC ]; // Location of fatality
    struct fatality_event *next; // Pointer to next fatality event
};

/*Hash Table*/
/*Helps with hashing function*/
struct hash_table_entry
{
    int event_id; // Event id -- key used to hash on
    int year; // Year of storm event
    int event_index; // For the given year, the index into array of storm events
    struct hash_table_entry *next; // Pointer to next entry in case of collisions
};

/*Binary Search Tree*/
/*Used to help order the structs for ease of searching*/
struct bst
{ 
    string s; // String corresponding to either a state or a month_name
    int event_id; // Identifier of storm event
    struct bst *left;  // Pointer to the left subtree
    struct bst *right;  // Pointer to the right subtree
};

/*Forward Declarations*/
void readDetails(string current_year); //File Functions
void readFatality(string current_year);
void findEvent(int event_id);
void calculateLines(string current_year);
int convertToInt(string value);
int hashNode(int location, int count); //Hash Table Functions
int hash(int event_id);
void insertHash(int eventID, int year, int count, int hashLocation);
bool testForPrime(int value);
int calculatePrime(int size);
void printHashTable();
void printHashLoad();
void buildBinarySearchTree(string year, string field_name, string start, string end); //BST Functions
struct bst* insert(struct bst* node, string type, int event_id);
void deleteTree(bst* root);
struct bst* newNode(string type, int event_id);
int bstTotalNodes(bst* root);
int bstLeftDepth(bst* root);
int bstRightDepth(bst* root);
void inOrderPrint(struct bst* root); 
void buildMaxHeap(int num_of_storms, string specific_year, string damage_type); //Max Heap Functions
void heapifyProperty(storm_event **root, int n, int i);
void heapifyCrops(storm_event **root, int n, int i);
void heapifyFatality(storm_event **root, int* fatalNum, int n, int i);
void deleteRoot(storm_event **root, int& heapSize, string damage_type);
void deleteFatalityRoot(storm_event **root, int& heapSize, int* fatalNum);
void heapStatistics(int heapSize, int heapSizeOriginal);
void printInfo(int location, int index); //Basic Print Function
void processQueries(); //Query Handler Functions
bool monthRangeCheck(string start, string end); 

/*Global Variables*/
struct annual_storms* yearHolder; //Holds the entries of annual storms
struct hash_table_entry** hashHolder; //Holds the hash table
int yearHolderSize = 0; //Holds size of the array
int hashSize = 0; //Hash Table Size
int elementSize = 0; //Holds total elements for ease of use
int num_years = 0; //Number of years from command console

/*Driver Program*/
int main(int argc, char** argv)
{
	if(argc >= 3) //Run program
	{
		string temp = argv[1]; //Temporarily holds the string at spot 1
		num_years = 0; //Number of years of data to hold
		num_years = atoi(temp.c_str()); //Convert the string to an integer
		temp = ""; //Reset temp
		
		int current_year; //Holds the current year being looked at
		yearHolder = new annual_storms[num_years]; //Create a struct array of size n
		yearHolderSize = num_years; //Set size of the struct array
		
		for(int i = 0; i < num_years; i++) //Creates array of years to hold data
		{
			temp = argv[2+i]; 
			current_year = atoi(temp.c_str()); 
			yearHolder[i].year = current_year; //Store the year in the struct array
			yearHolder[i].array_size = 0; //Initially set to 0
			yearHolder[i].events = NULL; //Set pointer to NULL for now
			calculateLines(temp); //Calculates lines in current document
		}
		
		for(int i = 0; i < num_years; i++)//Create the hash table
		{
			hashSize = hashSize + yearHolder[i].array_size; //Add to size
			elementSize = elementSize + yearHolder[i].array_size; //For printing purposes
		}
		hashSize = calculatePrime(hashSize); //Creates hash table size based on primes
		
		hashHolder = new hash_table_entry* [hashSize]; //Create the hash table
		for(int i = 0; i < hashSize; i++) //Set every spot in hash table to NULL
		{
			hashHolder[i] = NULL;
		}
		
		for(int i = 0; i < num_years; i++) //Create array of details-YYYY & Hash Table
		{
			temp = argv[2+i];
			readDetails(temp); //Call function
		}
		
		for(int i = 0; i < num_years; i++) //Create objects of fatalities-YYYY
		{
			temp = argv[2+i];
			readFatality(temp); //Call function
		}
		
		/*Prints every bit of node information*/
		/*for(int i = 0; i < yearHolderSize; i++) //Print entire yearHolder to check if working
		{
			for(int j = 0; j < yearHolder[i].array_size; j++)
			{
				//cout << yearHolder[i].array_size << endl;
				//cout << j << endl;
				printInfo(i, j);
				cout << endl;
			}
		}*/
		//printHashTable(); //Prints the hash table to check for working
		
		processQueries(); //Handles the queries
		
		printHashLoad(); //Prints the load for the hash table
	}
	else //Return error message
		cout << "Invalid number of arguments" << endl;
	
	return 0;
}

/*Find Event*/
/*Searches thru hash table to find the event node and prints its info*/
void findEvent(int event_id)
{
	int index = hash(event_id); //Hash on the event ID
	hash_table_entry* hptr = hashHolder[index]; //Set a pointer to the spot
	int year = 0; //Year to search for in annual_storms struct
	int location = 0; //Location in annual_storms struct based on year
	int count = 0; //Location in storm_events struct
	
	if(hptr != NULL) //Make sure location has an event
	{
		if(hptr->event_id == event_id) //Check if first node is correct
		{
			year = hptr->year;
			for(int i = 0; i < num_years; i++) //Find year location
			{
				if(yearHolder[i].year == year)
					location = i;
			}
			count = hptr->event_index;
			printInfo(location, count); //Print node info
		}
		else //Iterate until finding the correct node or confirming it doesn't exist
		{
			bool stop = false;
			while(hptr != NULL && stop != true)
			{
				if(hptr->event_id != event_id) //Keep checking
					hptr = hptr->next;
				else //Found node and prints
				{	
					stop = true;
					year = hptr->year;
					for(int i = 0; i < num_years; i++) //Find year location
					{
						if(yearHolder[i].year == year)
							location = i;
					}
					count = hptr->event_index;
					printInfo(location, count); //Print node info
				}
			}
			//Iterating comes up empty
			if(stop == false)
				cout << "Storm event " << event_id << " not found." << endl;
		}
	}
	else //If hash comes up empty, return node doesn't exist
	{
		cout << "Storm event " << event_id << " not found." << endl;
	}
}

/*Initial Node Hash Function*/
/*Determines the location in the hash table to place an element */
int hashNode(int location, int count) //location is year, count is current node
{
	int index = 0;
	index = yearHolder[location].events[count].event_id % hashSize; //event_id % hashPrime
	
	return index;
}

/*Hash Function*/
/*Locates nodes using their event_id variable*/
int hash(int event_id)
{
	int index = 0;
	
	index = event_id % hashSize; //Hash for location in hash table
	
	return index;
}

/*Test For Prime*/
/*Helper function, determines if a value is prime*/
bool testForPrime(int value)
{
	int limit;
	int factor = 2;

    limit = (long)( sqrtf( (float) value + 0.5f ));
	
    while( (factor <= limit) && (value % factor) )
        factor++;

    return( factor > limit );
}

/*Calcualte Prime*/
/*Calculates a prime of at least (2 * size)*/
int calculatePrime(int size)
{
	int prime = (2 * size) + 1; //Program definition for prime start
	
	while(testForPrime(prime) != true) //Test for next prime
	{
		prime++;
	}
	
	return prime;
}

/*Calculate Lines*/
/*Calculates the total number of lines in all imported documents*/
void calculateLines(string current_year)
{
	int lines = -1; //Counting number of lines in csv file, burn the first line
	string line; //Allows dumping of info into line
	int location = 0; //Location of the year in yearHolder
	int cur_year = atoi(current_year.c_str()); //Convert the string to an integer
	
	string tempFile = "details-" + current_year + ".csv"; //Holds name of current file
	ifstream inFile; //Create inFile
	inFile.open(tempFile.c_str()); //Open the current file
	
	while(getline(inFile, line)) //Count number of lines in csv file
	{
			lines++; //Increase line count
	}
	
	for(int i = 0; i < yearHolderSize; i++) //Find location in yearHolder
	{
		if(yearHolder[i].year == cur_year)
			location = i;
	}
	yearHolder[location].array_size = lines; //Save number of elements to be in array
	
	inFile.close(); //Close the current file
}

/*Insert Hash*/
/*Inserts node into hash table with separate chaining*/
void insertHash(int eventID, int year, int count, int hashLocation)
{
	if(hashHolder[hashLocation] == NULL) //Insert immediately
	{
		hashHolder[hashLocation] = new hash_table_entry;
		hashHolder[hashLocation]->event_id = eventID;
		hashHolder[hashLocation]->year = year;
		hashHolder[hashLocation]->event_index = count;
		hashHolder[hashLocation]->next = NULL;
	}
	else //Create new node and place at front
	{
		hash_table_entry* nodeH = new hash_table_entry;
		nodeH->event_id = eventID;
		nodeH->year = year;
		nodeH->event_index = count;
		nodeH->next = hashHolder[hashLocation];
		hashHolder[hashLocation] = nodeH;
	}
}

/*Read Details*/
/*Reads the details file into the structs & create hash table*/
void readDetails(string current_year)
{
	int count = 0; //Tracks current line on for location tracking
	int location = 0; //Location of year
	int cur_year = atoi(current_year.c_str()); //Convert the string to an integer
	string line; //Allows dumping of info into line
		
	string tempFile = "details-" + current_year + ".csv"; //Holds name of current file
	ifstream inFile; //Create inFile
	inFile.open(tempFile.c_str()); //Open the current file
	getline(inFile, line); //Burn top line
	
	for(int i = 0; i < yearHolderSize; i++) //Find location in yearHolder
	{
		if(yearHolder[i].year == cur_year)
			location = i;
	}
	yearHolder[location].events = new storm_event[yearHolder[location].array_size]; //Create sub struct array for the current year
	
	while(getline(inFile, line)) //Loop through the lines to read data from csv file & save to node
	{
		storm_event node; //Create a new event node
		string temp; //Temporarily holds data
		
		stringstream current_line(line); //Stringstream the line
		
		getline(current_line, temp, ','); //Event_ID
		node.event_id = atoi(temp.c_str());
		
		getline(current_line, temp, ','); //State Name
		char state_in[ STATE_LEN ];
		strcpy(state_in, temp.c_str());
		strcpy(node.state, state_in);
		
		getline(current_line, temp, ','); //Year
		node.year = atoi(temp.c_str());
		
		getline(current_line, temp, ','); //Month Name
		char month_in[ MONTH_LEN ];
		strcpy(month_in, temp.c_str());
		strcpy(node.month_name, month_in);
		
		getline(current_line, temp, ','); //Event Type
		char event_in[ EVENT_LEN ];
		strcpy(event_in, temp.c_str());
		strcpy(node.event_type, event_in);
		
		getline(current_line, temp, ','); //CZ Type
		char cz_type_in = temp[0];
		node.cz_type = cz_type_in; 
		
		getline(current_line, temp, ','); //CZ Name
		char cz_name_in[ COUNTY_LEN ];
		strcpy(cz_name_in, temp.c_str());
		strcpy(node.cz_name, cz_name_in);
		
		getline(current_line, temp, ','); //Number of Direct Injuries
		node.injuries_direct = atoi(temp.c_str()); 
		
		getline(current_line, temp, ','); //Number of Indirect Injuries
		node.injuries_indirect = atoi(temp.c_str()); 
		
		getline(current_line, temp, ','); //Number of Direct Deaths
		node.deaths_direct = atoi(temp.c_str());
		
		getline(current_line, temp, ','); //Number of indirect deaths
		node.deaths_indirect = atoi(temp.c_str());
		
		getline(current_line, temp, ','); //Amount of property damage / Convert to integer
		int property_damage = convertToInt(temp); //Work on this
		node.damage_property = property_damage;
		
		getline(current_line, temp, ','); // Amount of crop damage / Convert to integer
		int crop_damage = convertToInt(temp); //Work on this
		node.damage_crops = crop_damage;
		
		getline(current_line, temp); //Strength of tornado on Fujita scale
		char tor_scale_in[ TOR_LEN ];
		strcpy(tor_scale_in, temp.c_str());
		strcpy(node.tor_f_scale, tor_scale_in);
		
		node.f = NULL; //Set fatality pointer to NULL for now
		
		yearHolder[location].events[count] = node; //Store the node in the apropriate year spot
		int hashLocation = hashNode(location, count); //Locate spot for hash node in table
		insertHash(node.event_id, node.year, count, hashLocation); //Inserts node into hash table
		
		count++; //Increase count
	}
	
	inFile.close(); //Close the file
}

/*Read Fatality*/
/*Extends reads details, reads fatality info into struct, adds to detail nodes using hash table*/
void readFatality(string current_year)
{
	int location = 0; //Location of year
	string line; //Allows dumping of info into line
	int cur_year = atoi(current_year.c_str()); //Convert the string to an integer
	
	string fatFile = "fatalities-" + current_year + ".csv"; //Holds name of current file
	ifstream fatalFile; //Create fatalFile
	fatalFile.open(fatFile.c_str()); //Open the current file
	
	for(int i = 0; i < yearHolderSize; i++) //Find location in yearHolder
	{
		if(yearHolder[i].year == cur_year)
			location = i;
	}
	
	getline(fatalFile, line); //Burn top line
	
	//Loop through event ID in fatality file to add to correct node using Hash table
	while(getline(fatalFile, line))
	{
		string temp; //Temporarily holds data from file
		fatality_event* fptr = new fatality_event; //Create new fatality object
		
		stringstream current_line(line); //Stringstream the line
		
		getline(current_line, temp, ','); //Fatality ID
		fptr->fatality_id = atoi(temp.c_str());
		
		getline(current_line, temp, ','); //Storm Event Id
		fptr->event_id = atoi(temp.c_str());
		
		getline(current_line, temp, ','); //Fatality Type
		char fatal_type_in = temp[0];
		fptr->fatality_type = fatal_type_in;
		
		getline(current_line, temp, ','); //Date/Time of Fatality
		char date_in[ DATE_LEN ];
		strcpy(date_in, temp.c_str());
		strcpy(fptr->fatality_date, date_in);
		
		getline(current_line, temp, ','); //Fatality Age
		fptr->fatality_age = atoi(temp.c_str());
		
		getline(current_line, temp, ','); //Fatality Gender
		char fatal_sex_in = temp[0];
		fptr->fatality_sex = fatal_sex_in;
		
		char fatality_location[ FAT_LOC ]; // Location of fatality
		char fatal_loc_in[ FAT_LOC ];
		strcpy(fatal_loc_in, temp.c_str());
		strcpy(fptr->fatality_location, fatal_loc_in);
		
		fptr->next = NULL; // Set pointer to NULL for now
		
		int node = hash(fptr->event_id); //Hash the fatality node
		hash_table_entry* hptr = hashHolder[node]; //Create a hash_table_entry ptr to iterate
		
		if(hptr != NULL) //Make sure the hash table spot isn't empty
		{
			if(hptr->event_id == fptr->event_id) //Check to see if first hash node is correct
			{	
				int count = hptr->event_index;
				if(yearHolder[location].events[count].f == NULL) //Check f ptr is empty
				{
					yearHolder[location].events[count].f = fptr;
				}
				else //Re-orient linked list if not empty
				{
					fatality_event* fptr2 = yearHolder[location].events[count].f;
					fptr->next = fptr2;
					yearHolder[location].events[count].f = fptr;
				}
			}
			else //Search thru hash table linked list to find correct spot & then insert
			{
				bool stop = false;
				while(hptr != NULL && stop != true)
				{
					if(hptr->event_id != fptr->event_id)
						hptr = hptr->next;
					else
					{
						stop = true;
					}
				}
				int count = hptr->event_index;
				if(yearHolder[location].events[count].f == NULL) //Check to see if f ptr is empty
				{
					yearHolder[location].events[count].f = fptr;
				}
				else //Re-orient linked list if not empty
				{
					fatality_event* fptr2 = yearHolder[location].events[count].f;
					fptr->next = fptr2;
					yearHolder[location].events[count].f = fptr;
				}
			}
		}
	}
}

/*Convert To Integer*/
/*Converts a shorthand string value to a full integer value*/
int convertToInt(string value)
{
	int newValue = 0;
	double tempValue = 0;
	char value_modifier = 'a';
	
	if(value.empty()) //If no value do nothing
		return newValue;
	else if(value == "0") //If value is 0, return 0
		return newValue;
	else //If some other value, calculate it
	{
		value_modifier = value[value.length() - 1]; //Find character value in string and save it
		value.erase(value.length() - 1, 1); //Remove the character value
		tempValue = atof(value.c_str());//Convert remaining string to a double
		
		//Multiply double by character value scalar
		if(value_modifier == 'K')
			tempValue = tempValue * 1000;
		else if(value_modifier == 'M')
			tempValue = tempValue * 1000000;
		else if(value_modifier == 'B')
			tempValue = tempValue * 1000000000;
		newValue = (int) tempValue; //Cast double to an integer
	}
	
	return newValue;
}

/*Print Info*/
/*Prints the information for the specified node*/
void printInfo(int location, int index)
{
	//Details of the Event
	cout << "Event ID: " << yearHolder[location].events[index].event_id << endl;
	cout << "State: " << yearHolder[location].events[index].state << endl;
	cout << "Year: " << yearHolder[location].events[index].year << endl;
	cout << "Month: " << yearHolder[location].events[index].month_name << endl;
	cout << "Event Type: " << yearHolder[location].events[index].event_type << endl;
	cout << "CZ Type: " << yearHolder[location].events[index].cz_type << endl;
	cout << "CZ Name: " << yearHolder[location].events[index].cz_name << endl;
	cout << "Direct Injuries: " << yearHolder[location].events[index].injuries_direct << endl;
	cout << "Indirect Injuries: " << yearHolder[location].events[index].injuries_indirect << endl;
	cout << "Direct Deaths: " << yearHolder[location].events[index].deaths_direct << endl;
	cout << "Indirect Deaths: " << yearHolder[location].events[index].deaths_indirect << endl;
	cout << "Property Damage: $" << yearHolder[location].events[index].damage_property << endl;
	cout << "Crop Damage: $" << yearHolder[location].events[index].damage_crops << endl;
	cout << "Tornado Fujita Scale: " << yearHolder[location].events[index].tor_f_scale << endl;
	cout << endl;
	
	//Event Fatalities
	if(yearHolder[location].events[index].f != NULL) //If fatalities present
	{
		fatality_event* fptr = yearHolder[location].events[index].f; //Set pointer for fatalities
		int fatalityCount = 1;
		
		while(fptr != NULL) //Check for no more fatalities
		{
			cout << "Fatality Number " << fatalityCount << endl;
			cout << "Fatality ID: " << fptr->fatality_id << endl;
			cout << "Event ID: " << fptr->event_id << endl;
			cout << "Fatality Type: " << fptr->fatality_type << endl;
			cout << "Fatality Date: " << fptr->fatality_date << endl;
			cout << "Fatality Age: " << fptr->fatality_age << endl;
			cout << "Fatality Sex: " << fptr->fatality_sex << endl;
			cout << "Fatality Location: " << fptr->fatality_location << endl;
			fptr = fptr->next;
		}
		cout << endl;
	}
	else //No fatalities for event
	{	
		cout << "Fatalities: No fatalities" << endl;
		cout << endl;
	}
}

/*Print Hash Table*/
/*Prints hash table contents by event ID*/
void printHashTable()
{
	for(int i = 0; i < hashSize; i++)
	{
		if(hashHolder[i] != NULL) //Show filled spots
		{
			cout << "Location: " << i << " ";
			hash_table_entry* ptr = hashHolder[i]; //Pointer to iterate
			while(ptr != NULL) //Check for the chaining
			{
				cout << ptr->event_id << " ";
				ptr = ptr->next;
			}
			cout << endl;
		}
		else
			cout << "Location: " << i << endl;
	}
}

/*Print Hash Load*/
/*Conveniently displays how full the hash table is and to what degree*/
void printHashLoad()
{
	int longestChain = 0;
	int count = 0;
	int loadFactor = 0;
	
	for(int i = 0; i < hashSize; i++)
	{
		if(hashHolder[i] != NULL) //Show filled spots
		{
			hash_table_entry* ptr = hashHolder[i]; //Pointer to iterate
			while(ptr != NULL) //Check for the chaining
			{
				count++;
				if(longestChain < count) //Find the longest possible linked list chain
					longestChain = count;
				ptr = ptr->next;
			}
		}
		count = 0;
	}
	
	int* lengthArray = new int[longestChain+1]; //Create array to reference & store values
	for(int i = 0; i < longestChain+1; i ++) //Initialize the array to all 0
		lengthArray[i] = 0;
	
	for(int i = 0; i < hashSize; i++) //Work through hash table again
	{
		count = 0;
		if(hashHolder[i] != NULL) //Check for filled spots
		{
			hash_table_entry* ptr = hashHolder[i];
			while(ptr != NULL) //Check for the chaining
			{
				count++;
				ptr = ptr->next;
			}
			lengthArray[count] = lengthArray[count] + 1; //Increment array value by length
		}
		else
		{
			lengthArray[0] = lengthArray[0] + 1; //Increment zero length array values
		}
	}
	
	cout << "Hash Table Size: " << hashSize << endl; //Print hash table size
	cout << "Number of Total Elements: " << elementSize << endl; //Print total elements in hash table
	cout << "Longest Chain: " << longestChain << endl; //Print longest chain value
	for(int i = 0; i <= longestChain; i++) //Print number of each chain length
	{
		cout << "Number of spots with " << i << " elements: " << lengthArray[i];
		float percent = ((float) lengthArray[i]/hashSize) * 100;
		cout << " | Percentage of total spots: " << percent << "%" << endl; //Show percentage
	}
	cout << "Hash Table Load Factor: " << ((float)elementSize / hashSize) << endl; //Print load factor
	delete lengthArray;
}

/*New BST Node*/
/*Creates a new BST node*/
struct bst* newNode(string type, int event_id)
{
	bst* temp = new bst;
	temp->s = type;
	temp->event_id = event_id;
	temp->left = NULL;
	temp->right = NULL;
	return temp;
}

/*Insert BST*/
/*Inserts a node into the BST*/
struct bst* insert(struct bst* node, string type, int event_id)
{
	if(node == NULL) //If tree is empty return the new node
		return newNode(type, event_id);
	
	if(type < node->s) //Check for field_name first; less than add to left
	{
		node->left = insert(node->left, type, event_id);
	}
	else if(type > node->s) //If field_name greater than add to right
		node->right = insert(node->right, type, event_id);
	else //Field Names are equal
		if(event_id < node->event_id) //Event ID is less than current node
			node->left = insert(node->left, type, event_id);
		else if(event_id > node->event_id) //Event ID is greater than current node
			node->right = insert(node->right, type, event_id);
		
	return node; //Return the node pointer
}

/*Delete Tree*/
/*Deletes the binary search tree*/
void deleteTree(bst* root)
{
	if (root == NULL)
		return;
	
	deleteTree(root->left);
	deleteTree(root->right);
	
	delete root;
}

/*Build Binary Search Tree*/
/*Function that creates the binary search tree*/
void buildBinarySearchTree(string year, string field_name, string start, string end)
{
	struct bst *root = NULL; //Holds the head of the bst
	int location = 0;
	int current_year = 0;
	int left = 0;
	int right = 0;
	if(year != "all")
		current_year = atoi(year.c_str());
	
	if(field_name == "state" && year != "all") //For state with specific year
	{
		for(int i = 0; i < yearHolderSize; i++) //Find location in yearHolder to create bst
		{
			if(current_year == yearHolder[i].year)
				location = i;
		}
		
		for(int i = 0; i < yearHolder[location].array_size; i++) //Compare with range
		{
			string temp = yearHolder[location].events[i].state;
			int event_id = yearHolder[location].events[i].event_id;
			if(temp[0] >= start[0] && temp[0] <= end[0]) //Check range to call insertion
			{
				root = insert(root, temp, event_id);
			}
		}
		
		inOrderPrint(root); //Print the tree
	}
	else if(field_name == "state" && year == "all") //For state with every year
	{
		for(int i = 0; i <yearHolderSize; i++) //Iterate through every year
		{
			for(int j = 0; j < yearHolder[i].array_size; j++) //Compare with range
			{
				string temp = yearHolder[i].events[j].state;
				int event_id = yearHolder[i].events[j].event_id;
				if(temp[0] >= start[0] && temp[0] <= end[0]) //Check range to call insertion
				{
					root = insert(root, temp, event_id);
				}
			}
		}
		
		inOrderPrint(root); //Print the tree
	}
	else if(field_name == "month_name" && year != "all") //For month with specific year
	{
		for(int i = 0; i < yearHolderSize; i++) //Find location in yearHolder to create bst
		{
			if(current_year == yearHolder[i].year)
				location = i;
		}
		
		for(int i = 0; i < yearHolder[location].array_size; i++) //Compare with range
		{
			string temp = yearHolder[location].events[i].month_name;
			int event_id = yearHolder[location].events[i].event_id;
			bool startMonth = monthRangeCheck(start, temp);
			bool endMonth = monthRangeCheck(temp, end);
			
			if(startMonth && endMonth) //Check month range to insert
			{
				root = insert(root, temp, event_id);
			}
		}
		
		inOrderPrint(root); //Print the tree
	}
	else if(field_name == "month_name" && year == "all") //For month with every year
	{
		for(int i = 0; i < yearHolderSize; i++) //Iterate thru every year
		{
			for(int j = 0; j < yearHolder[i].array_size; j++) //Compare with range
			{
				string temp = yearHolder[i].events[j].month_name;
				int event_id = yearHolder[i].events[j].event_id;
				bool startMonth = monthRangeCheck(start, temp);
				bool endMonth = monthRangeCheck(temp, end);
				
				if(startMonth && endMonth) //Check month range to insert
				{
					root = insert(root, temp, event_id);
				}
			}
		}
		
		inOrderPrint(root); //Print the tree
	}
	if(root == NULL) //If no matching events found
		cout << "No storm events found for given range" << endl;
	else
	{
		right = bstRightDepth(root->right); //Count height of right subtree
		left = bstLeftDepth(root->left); //Count height of left subtree
		
		cout << "BST Total Nodes: " << bstTotalNodes(root) << endl; //Count total nodes & print
		if(left >= right)
			cout << "Height of the BST: " << left + 1 << endl;
		else
			cout << "Height of the BST: " << right + 1 << endl;
		cout << "Height of Left Tree: " << left << endl; //Print left subtree
		cout << "Height of Right Tree: " << right << endl << endl; //Print right subtree
	}
	
	deleteTree(root); //Delete the BST
	root = NULL; //Set the root to NULL
}

/*In Order Print*/
/*Prints the BST in order*/
void inOrderPrint(struct bst* root)
{
	if(root != NULL)
	{
		inOrderPrint(root->left); //Recursive call to the left nodes
		
		int index = hash(root->event_id); //Hash on the event ID
		hash_table_entry* hptr = hashHolder[index]; //Set a pointer to the spot
		int year = 0; //Year to search for in annual_storms struct
		int location = 0; //Location in annual_storms struct based on year
		int count = 0; //Location in storm_events struct
		
		if(hptr->event_id == root->event_id) //Check if first node is correct
		{
			year = hptr->year;
			for(int i = 0; i < num_years; i++) //Find year location
			{
				if(yearHolder[i].year == year)
					location = i;
			}
			count = hptr->event_index;
			cout << "State/Month Name: " << root->s << endl;
			cout << "Event ID: " << yearHolder[location].events[count].event_id << endl;
			cout << "Year: " << yearHolder[location].events[count].year << endl;
			cout << "Event Type: " << yearHolder[location].events[count].event_type << endl;
			cout << "CZ Type: " << yearHolder[location].events[count].cz_type << endl;
			cout << "CZ Name: " << yearHolder[location].events[count].cz_name << endl << endl;
		}
		else //Iterate until finding the correct node or confirming it doesn't exist
		{
			bool stop = false;
			while(hptr != NULL && stop != true)
			{
				if(hptr->event_id != root->event_id) //Keep checking
					hptr = hptr->next;
				else //Found node and prints
				{	
					stop = true;
					year = hptr->year;
					for(int i = 0; i < num_years; i++) //Find year location
					{
						if(yearHolder[i].year == year)
							location = i;
					}
					count = hptr->event_index;
					cout << "State/Month Name: " << root->s << endl;
					cout << "Event ID: " << yearHolder[location].events[count].event_id << endl;
					cout << "Year: " << yearHolder[location].events[count].year << endl;
					cout << "Event Type: " << yearHolder[location].events[count].event_type << endl;
					cout << "CZ Type: " << yearHolder[location].events[count].cz_type << endl;
					cout << "CZ Name: " << yearHolder[location].events[count].cz_name << endl << endl;
				}
			}
		}
		
		inOrderPrint(root->right); //Recursive call to the right nodes
	}
}

/*BST Left Depth*/
/*Calculates the Depth of the Left BST*/
int bstLeftDepth(bst* root)
{
	if(root == NULL)
		return 0;
	else
	{
		int depthL = bstLeftDepth(root->left);
		int depthR = bstRightDepth(root->right);
		if(depthL > depthR)
			return (depthL + 1);
		else
			return (depthR + 1);
	}
}

/*BST Right Depth*/
/*Calculates the Depth of the Right BST*/
int bstRightDepth(bst* root)
{
	if(root == NULL)
		return 0;
	else
	{
		int depthR = bstRightDepth(root->right);
		int depthL = bstLeftDepth(root->left);
		if(depthL > depthR)
			return (depthL + 1);
		else
			return (depthR + 1);
	}
}

/*BST Total Nodes*/
/*Calculates the total number of nodes in the BST*/
int bstTotalNodes(bst* root)
{
	int n = 1;
	if(root == NULL)
		return 0;
	if(root->left != NULL)
	{
		n = n + bstTotalNodes(root->left); //Recursive call to the left node
	}
	if(root->right != NULL)
	{
		n = n + bstTotalNodes(root->right); //Recursive call to the right node
	}
	return n;
}

/*Build Max Heap*/
/*Builds the max heap for property and crop damage*/
void buildMaxHeap(int num_of_storms, string year, string damage_type)
{
	struct storm_event **root = NULL; //Holds the head of the bst
	int heapSize = 0;
	int heapSizeOriginal = 0;
	int location = 0;
	int current_year = 0;
	if(year != "all")
		current_year = atoi(year.c_str()); //Convert the year to an integer value
	
	if(current_year != 0 && damage_type == "damage_property") //For damage to property in specific year
	{
		for(int i = 0; i < yearHolderSize; i++) //Find location in yearHolder to create heap
		{
			if(current_year == yearHolder[i].year)
				location = i;
		}
		heapSize = yearHolder[location].array_size; //Find size for heap
		heapSizeOriginal = heapSize; //For statistics purposes
		root = new storm_event* [heapSize]; //Instantiate the heap
		
		for(int i = 0; i < heapSize; i++) //Assign each pointer to its relevant node
		{
			root[i] = &yearHolder[location].events[i];
		}
		
		int startIndex = (heapSize / 2) - 1; //Find start to heapify
		for(int i = startIndex; i >= 0; i--) //Heapify the array
		{
			heapifyProperty(root, heapSize, i);
		}
		
		for(int i = 0; i < num_of_storms; i++) //Remove elements from top as requested & print info
		{
			deleteRoot(root, heapSize, damage_type);
		}
	}
	else if(current_year != 0 && damage_type == "damage_crops") //For damage to crops in a specific year
	{
		for(int i = 0; i < yearHolderSize; i++) //Find location in yearHolder to create heap
		{
			if(current_year == yearHolder[i].year)
				location = i;
		}
		heapSize = yearHolder[location].array_size; //Find size for heap
		heapSizeOriginal = heapSize; //For statistics purposes
		root = new storm_event* [heapSize]; //Instantiate the heap
		
		for(int i = 0; i < heapSize; i++) //Assign each pointer to its relevant node
		{
			root[i] = &yearHolder[location].events[i];
		}
		
		int startIndex = (heapSize / 2) - 1; //Find start to heapify
		for(int i = startIndex; i >= 0; i--) //Heapify the array
		{
			heapifyCrops(root, heapSize, i);
		}
		
		for(int i = 0; i < num_of_storms; i++) //Remove elements from top as requested & print info
		{
			deleteRoot(root, heapSize, damage_type);
		}
	}
	else if(year == "all" && damage_type == "damage_property") //For damage to property in all years
	{
		for(int i = 0; i < yearHolderSize; i++) //Calculate the heap size
		{
			heapSize = heapSize + yearHolder[i].array_size;
		}
		heapSizeOriginal = heapSize; //For statistics purposes
		root = new storm_event* [heapSize]; //Instantiate the heap
		
		int count = 0; //Tracks location in heap
		for(int i = 0; i < yearHolderSize; i++) //Assign each pointer to its relevant node
		{
			for(int j = 0; j < yearHolder[i].array_size; j++)
			{
				root[count] = &yearHolder[i].events[j];
				count++; //Move to next spot in heap
			}
		}
		
		int startIndex = (heapSize / 2) - 1; //Find start to heapify
		for(int i = startIndex; i >= 0; i--) //Heapify the array
		{
			heapifyProperty(root, heapSize, i);
		}
		
		for(int i = 0; i < num_of_storms; i++) //Remove elements from top as requested & print info
		{
			deleteRoot(root, heapSize, damage_type);
		}
	}
	else if(year == "all" && damage_type == "damage_crops") //For damage to crops in all years
	{
		for(int i = 0; i < yearHolderSize; i++) //Calculate the heap size
		{
			heapSize = heapSize + yearHolder[i].array_size;
		}
		heapSizeOriginal = heapSize; //For statistics purposes
		root = new storm_event* [heapSize]; //Instantiate the heap
		
		int count = 0; //Tracks location in heap
		for(int i = 0; i < yearHolderSize; i++) //Assign each pointer to its relevant node
		{
			for(int j = 0; j < yearHolder[i].array_size; j++)
			{
				root[count] = &yearHolder[i].events[j];
				count++; //Move to next spot in heap
			}
		}
		
		int startIndex = (heapSize / 2) - 1; //Find start to heapify
		for(int i = startIndex; i >= 0; i--) //Heapify the array
		{
			heapifyCrops(root, heapSize, i);
		}
		
		for(int i = 0; i < num_of_storms; i++) //Remove elements from top as requested & print info
		{
			deleteRoot(root, heapSize, damage_type);
		}
	}
	else if(current_year != 0 && damage_type == "fatality") //Fatalities for a single year
	{
		for(int i = 0; i < yearHolderSize; i++) //Find location in yearHolder to create heap
		{
			if(current_year == yearHolder[i].year)
				location = i;
		}
		heapSize = yearHolder[location].array_size; //Find size for heap
		heapSizeOriginal = heapSize; //For statistics purposes
		root = new storm_event* [heapSize]; //Instantiate the heap
		
		int *fatalityArray = new int [heapSize]; //Instantiate array for fatalities
		for(int i = 0; i < heapSize; i++)
		{
			fatalityArray[i] = 0; //Set initial values to zero
		}
		
		for(int i = 0; i < heapSize; i++) //Assign each pointer to its relevant node
		{
			root[i] = &yearHolder[location].events[i];
			fatality_event* ptr = root[i]->f;
			
			if(ptr != NULL)
			{
				while(ptr != NULL) //Look for fatality number
				{
					fatalityArray[i] = fatalityArray[i] + 1; //Add fatality
					ptr = ptr->next; //Move to next
				}
			}
		}
		
		int startIndex = (heapSize / 2) - 1; //Find start to heapify
		for(int i = startIndex; i >= 0; i--) //Heapify the array
		{
			heapifyFatality(root, fatalityArray, heapSize, i);
		}
		
		for(int i = 0; i < num_of_storms; i++) //Remove elements from top as requested & print info
		{
			deleteFatalityRoot(root, heapSize, fatalityArray);
		}
		
		delete fatalityArray; //Delete the array of integers
	}
	else if(year == "all" && damage_type == "fatality") //Fatalities for all years
	{
		for(int i = 0; i < yearHolderSize; i++) //Calculate the heap size
		{
			heapSize = heapSize + yearHolder[i].array_size;
		}
		heapSizeOriginal = heapSize; //For statistics purposes
		root = new storm_event* [heapSize]; //Instantiate the heap
		
		int *fatalityArray = new int [heapSize]; //Instantiate array for fatalities
		for(int i = 0; i < heapSize; i++)
		{
			fatalityArray[i] = 0; //Set initial values to zero
		}
		
		int count = 0; //Tracks location in heap
		for(int i = 0; i < yearHolderSize; i++) //Assign each pointer to its relevant node
		{
			for(int j = 0; j < yearHolder[i].array_size; j++)
			{
				root[count] = &yearHolder[i].events[j];
				fatality_event* ptr = root[count]->f;
				if(ptr != NULL)
				{
					while(ptr != NULL) //Look for fatality number
					{
						fatalityArray[count] = fatalityArray[count] + 1; //Add fatality
						ptr = ptr->next; //Move to next
					}
				}
				count++; //Move to next spot in heap
			}
		}
		
		int startIndex = (heapSize / 2) - 1; //Find start to heapify
		for(int i = startIndex; i >= 0; i--) //Heapify the array
		{
			heapifyFatality(root, fatalityArray, heapSize, i);
		}
		
		for(int i = 0; i < num_of_storms; i++) //Remove elements from top as requested & print info
		{
			deleteFatalityRoot(root, heapSize, fatalityArray);
		}
		
		delete fatalityArray; //Delete the array of integers
	}
	else //If something goes wrong
		cout << "Could not locate events" << endl;
		
	delete root; //Delete the max heap
		
	heapStatistics(heapSize, heapSizeOriginal); //Print statistical information
}

/*Heap Statistics*/
/*Handles necessary record keeping for assignment*/
void heapStatistics(int heapSize, int heapSizeOriginal)
{
	int height = heapSize; //For heap height
	int leftHeight; //For left subtree height
	int rightHeight; //For right subtree height
	int tracker = 1; //Tracks position in heap
	int count = 0; //Keeps track of the height
	
	while(tracker < heapSize) //Find height of heap/left subtree
	{
		tracker = tracker * 2 + 1;
		count++;
	}
	
	leftHeight = count; //Set the left subtree height
	height = count + 1; //Set the heap height
	tracker = 2; //Reset tracker
	count = 0; //Reset count
	
	while(tracker < heapSize) //Find height of the right subtree
	{
		tracker = tracker * 2 + 1;
		count++;
	}
	rightHeight = count; //Set the right subtree height
	
	//Print info to console
	cout << "Original Heap Nodes: " << heapSizeOriginal << endl;
	cout << "Current Heap Nodes: " << heapSize << endl;
	cout << "Original Heap Height: " << height << endl;
	cout << "Original Left Subtree Height: " << leftHeight << endl;
	cout << "Original Right Subtree Height: " << rightHeight << endl << endl;
}

/*Delete Fatality Root*/
/*Deletes the root of the max heap & prints its contents*/
void deleteFatalityRoot(storm_event **root, int& heapSize, int* fatalNum)
{
	fatality_event* fptr = root[0]->f; //Create pointer to iterate thru fatalities
	
	if(fptr != NULL) //Check if fptr has a target
	{
		int count = 1;
		cout << "Fatality Info for Event ID: " << fptr->event_id << endl << endl;
		
		while(fptr != NULL) //Print fatality info for each node
		{
			cout << "Fatality Number " << count << endl;
			cout << "Fatality ID: " << fptr->fatality_id << endl;
			cout << "Event ID: " << fptr->event_id << endl;
			cout << "Fatality Type: " << fptr->fatality_type << endl;
			cout << "Fatality Date: " << fptr->fatality_date << endl;
			cout << "Fatality Age: " << fptr->fatality_age << endl;
			cout << "Fatality Sex: " << fptr->fatality_sex << endl;
			cout << "Fatality Location: " << fptr->fatality_location << endl << endl;
			fptr = fptr->next;
			count++;
		}
	}
	else
	{
		cout << "No Fatalities for Event ID: " <<root[0]->event_id << endl << endl;
	}
	
	root[0] = root[heapSize - 1]; //Get rid of the head
	fatalNum[0] = fatalNum[heapSize - 1];
	heapSize = heapSize - 1; //Reduce the heapSize
	
	heapifyFatality(root, fatalNum, heapSize, 0); //Fix the heap property
}

/*Delete Root*/
/*Deletes the root of the max heap & prints its contents*/
void deleteRoot(storm_event **root, int& heapSize, string damage_type)
{
	if(damage_type == "damage_property") //Print info for property damage
	{
		cout << "Event ID: " <<root[0]->event_id << endl;
		cout << "Event Type: " << root[0]->event_type << endl;
		cout << "Property Damage: $" << root[0]->damage_property << endl << endl;
	}
	else if(damage_type == "damage_crops") //Print info for crop damage
	{
		cout << "Event ID: " <<root[0]->event_id << endl;
		cout << "Event Type: " << root[0]->event_type << endl;
		cout << "Property Damage: $" << root[0]->damage_crops << endl << endl;
	}
	
	root[0] = root[heapSize - 1]; //Put the last value at the root
	heapSize = heapSize - 1; //Reduce the size of the heap being worked with
	
	if(damage_type == "damage_property") //Fix the property heap
		heapifyProperty(root, heapSize, 0);
	else if(damage_type == "damage_crops") //Fix the crop heap
		heapifyCrops(root, heapSize, 0);
}

/*Heapify Property*/
/*Heapifies an array based on property damage*/
void heapifyProperty(storm_event **root, int n, int i)
{
	int largest = i; //Largest value
	int l = 2 * i + 1; //Left subtree
	int r = 2 * i + 2; //Right subtree
	
	if(l < n && root[l]->damage_property > root[largest]->damage_property) //Check left
		largest = l;
	if(r < n && root[r]->damage_property > root[largest]->damage_property) //Check right
		largest = r;
	if(largest != i) //Swap the nodes in the array
	{
		storm_event *temp = root[i];
		root[i] = root[largest];
		root[largest] = temp;
		heapifyProperty(root, n, largest); //Recursively call the heap
	}
}

/*Heapify Crops*/
/*Heapifies an array based on crop damage*/
void heapifyCrops(storm_event **root, int n, int i)
{
	int largest = i; //Largest value
	int l = 2 * i + 1; //Left subtree
	int r = 2 * i + 2; //Right subtree
	
	if(l < n && root[l]->damage_crops > root[largest]->damage_crops) //Check left
		largest = l;
	if(r < n && root[r]->damage_crops > root[largest]->damage_crops) //Check right
		largest = r;
	if(largest != i) //Swap the nodes in the array
	{
		storm_event *temp = root[i];
		root[i] = root[largest];
		root[largest] = temp;
		heapifyProperty(root, n, largest); //Recursively call the heap
	}
}

/*Heapify Fatality*/
/*Heapifies an array based on fatality information*/
void heapifyFatality(storm_event **root, int* fatalNum, int n, int i)
{
	int largest = i; //Largest value
	int l = 2 * i + 1; //Left subtree
	int r = 2 * i + 2; //Right subtree
	
	if(l < n && fatalNum[l] > fatalNum[largest]) //Check left
		largest = l;
	if(r < n && fatalNum[r] > fatalNum[largest]) //Check right
		largest = r;
	if(largest != i) //Swap the nodes in the arrays
	{
		storm_event *temp = root[i]; //Swap the heap pointers
		root[i] = root[largest];
		root[largest] = temp;
		
		int tempNum = fatalNum[i]; //Swap the fatality values
		fatalNum[i] = fatalNum[largest];
		fatalNum[largest] = tempNum;
		
		heapifyFatality(root, fatalNum, n, largest); //Recursively call the heap
	}
}

/*Process Queries*/
/*Handles the query handling for the program*/
void processQueries()
{
	string temp = ""; //Clear temp
	int queries_num = 0; //Collect number of queries to perform
	cin >> queries_num;
	cin.ignore(); //Flush the newline
	cout << endl; //Make easier to read
		
	/*Queries Section*/
	for(int i = 0; i < queries_num; i++)
	{
		string query_input = "";
		getline(cin, query_input); //Collect user input
		cout << endl; //Make easier to read
		stringstream query_line(query_input); //Convert to stringstream to parse
		
		getline(query_line, temp, ' ');
		if(temp == "find") //Check for keyword find
		{
			getline(query_line, temp, ' ');
			if(temp == "event") //Check for keyword event
			{	
				getline(query_line, temp); //Store event_id in temp
				int eventID = atoi(temp.c_str()); //Convert event_id to integer
				
				findEvent(eventID); //Call function
			}
			else if(temp == "max") //Check for keyword max
			{
				getline(query_line, temp, ' ');
				int num_of_storms = atoi(temp.c_str()); //Convert number of storms to integer
				if(temp == "fatality") //Check for keyword fatality
				{
					getline(query_line, temp, ' ');
					num_of_storms = atoi(temp.c_str()); //Convert number of storms to integer
					if(num_of_storms <= 50 && num_of_storms > 0) //Check number is correctly formatted
					{
						getline(query_line, temp, ' ');
						string specific_year = temp;
						if(temp == "all" || (specific_year > "1900" && specific_year < "2020")) //Check for correct year types
						{
							buildMaxHeap(num_of_storms, specific_year, "fatality");
						}
						else //Checked for all correct keywords
						{
							cout << "Query not in correct format" << endl; //Print problem
							i = i - 1; //Reset query number
						}
					}
					else //Checked for all correct keywords
					{
						cout << "Query not in correct format" << endl; //Print problem
						i = i - 1; //Reset query number
					}
				}
				else if(num_of_storms <= 50 && num_of_storms > 0) //Check number is correctly formatted
				{
					getline(query_line, temp, ' ');
					string specific_year = temp;
					if(temp == "all" || (specific_year > "1900" && specific_year < "2020")) //Check for correct year types
					{
						getline(query_line, temp);
						string damage_type = temp;
						if(temp == "damage_property" || temp == "damage_crops") //Check for damage type keyword
						{
							buildMaxHeap(num_of_storms, specific_year, damage_type);
						}
						else //Checked for all correct keywords
						{
							cout << "Query not in correct format" << endl; //Print problem
							i = i - 1; //Reset query number
						}
					}
					else
					{
						cout << "Query not in correct format" << endl; //Print problem
						i = i - 1; //Reset query number
					}
				}
				else //Checked for all correct keywords
				{
					cout << "Query not in correct format" << endl; //Print problem
					i = i - 1; //Reset query number
				}
			}
			else
			{
				cout << "Query not in correct format" << endl; //Print problem
				i = i - 1; //Reset query number
			}
		}
		else if(temp == "range") //Check for keyword range
		{
			getline(query_line, temp, ' ');
			string specific_year = temp; //Convert the year to integer
			if(specific_year == "all" || (specific_year > "1900" && specific_year < "2020")) //Check for correct year types
			{
				getline(query_line, temp, ' ');
				if(temp == "state") //Check for keyword state
				{
					string start; //For start of range
					string end; //For end of range
					getline(query_line, temp, ' ');
					start = temp;
					getline(query_line, temp);
					end = temp;
					if(start <= end && start >= "A" && end <= "Z") //Make sure range is setup correctly
					{
						buildBinarySearchTree(specific_year, "state", start, end);
					}
					else //Checked for all correct keywords
					{
						cout << "Query not in correct format" << endl; //Print problem
						i = i - 1; //Reset query number
					}
				}
				else if(temp == "month_name") //Check for keyword month_name
				{
					string start;
					string end;
					getline(query_line, temp, ' ');
					start = temp;
					getline(query_line, temp);
					end = temp;
					bool check = false;
					check = monthRangeCheck(start, end);
					if(check) //Make sure range is setup correctly
					{
						buildBinarySearchTree(specific_year, "month_name", start, end);
					}
					else
					{
						cout << "Query not in correct format" << endl; //Print problem
						i = i - 1; //Reset query number
					}
				}
				else //Checked for all correct keywords
				{
					cout << "Query not in correct format" << endl; //Print problem
					i = i - 1; //Reset query number
				}
			}
			else
			{
				cout << "Query not in correct format" << endl; //Print problem
				i = i - 1; //Reset query number
			}
		}
		else //Checked for correct keywords
		{
			cout << "Query not in correct format" << endl; //Print problem
			i = i - 1; //Reset query number
		}
	}
}

/*Month Range Checker*/
/*Checks to make sure months input are correct; Helper for range command*/
bool monthRangeCheck(string start, string end)
{
	string month[2];
	int range[2];
	month[0] = start;
	month[1] = end;
	
	for(int i = 0; i < 2; i++) //Assign each month a numerical value
	{
		if(month[i] == "January")
			range[i] = 1;
		else if(month[i] == "February")
			range[i] = 2;
		else if(month[i] == "March")
			range[i] = 3;
		else if(month[i] == "April")
			range[i] = 4;
		else if(month[i] == "May")
			range[i] = 5;
		else if(month[i] == "June")
			range[i] = 6;
		else if(month[i] == "July")
			range[i] = 7;
		else if(month[i] == "August")
			range[i] = 8;
		else if(month[i] == "September")
			range[i] = 9;
		else if(month[i] == "October")
			range[i] = 10;
		else if(month[i] == "November")
			range[i] = 11;
		else if(month[i] == "December")
			range[i] = 12;
		else
			range[i] = -1;
	}
	
	if(range[0] <= range[1] && range[0] != -1) //If range works return true
		return true;
	else
		return false;
}