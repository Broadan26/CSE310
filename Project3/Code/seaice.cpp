#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>

using namespace std; //Because lazy

//Holds each of the 63x63 cells
struct threshold_graph
{
	int size;
	bool land;
	struct connected_Node* list;
};

//Struct for each connected node in graph
struct connected_Node
{
	int location;
	struct connected_Node* next;
};

/*Forward Declarations*/
string** generateFileNames(int num_years, int start_year); //Generates file names
void openFiles(string** filesList, int num_years); //Opens files
void readFiles(ifstream& inFile); //Reads files
void pearsonCoeffXXAndYY(int num_years); //Calculate Sxx/Syy
void calculateXY(int num_years); //Calculate Sxy
void calculateR(); //Calculate R values
struct threshold_graph** generateThresholdGraph(float threshold); //Create threshold graphs
void printHistograms(); //Print histograms
int connectedComponents(struct threshold_graph** tptr); //Calculate Connected Components
int DFSUtil(int current, bool visited[], struct threshold_graph** tptr); //Utility
float clusteringCoefficient(struct threshold_graph** tptr, bool land); //Calculates clustering coefficient
void floydWarshall(int** graph); //Floyd Warshall for shortest path
int** generateGraph(struct threshold_graph** tptr); //Generates graph for Floyd Warshall
void printFloydWarshall(int** dist); //Prints Floyd-Warshall statistics
double meanEdgeDistribution(struct threshold_graph** tptr, bool land); //Calculates mean edges in random G

/*Global Variables*/
struct threshold_graph** graph95; //Holds graph for .95 threshold
struct threshold_graph** graph925; //Holds graph for .925 threshold
struct threshold_graph** graph90; //Holds graph for .9 threshold
float dataSet[3969][16][52]; //Holds all the floating point binary values
float coefficientsXX[3969][2]; //Holds the coefficient XX/YY values
bool endEarly = false; //Controls if program ends early or not
float coefficientsXY[3969][3969]; //Holds the coefficients for XY values
float rValues[3969][3969]; //Holds the final coefficient calculations

/*Driver Code*/
int main(int argc, char** argv)
{
	int num_years = 16; //Number of file years to open
	int start_year = 1990; //Holds the starting year
	bool fileCheck = true; //Allows program to continue
	float threshold1 = .95; //Highest threshold
	float threshold2 = .925; //Middle threshold
	float threshold3 = .9; //Lowest threshold

	string** filesList = generateFileNames(num_years, start_year);//Generate filenames
	openFiles(filesList, num_years); //Opens the files in fileList
	cout << "Finished Reading Files" << endl;

	for (int i = 0; i < 16; i++)
	{
		delete[] filesList[i];
	}
	delete[] filesList;

	if (endEarly == false) //Perform only if file operations were successful
	{
		//Calculate the coefficients Sxx & Syy 	
		pearsonCoeffXXAndYY(num_years); //Generate the Sxx/Syy values & mean info
		cout << "Finished Calculating Sxx/Syy" << endl;

		//Calculate the coefficients Sxy
		calculateXY(num_years);
		cout << "Finished calculating Sxy" << endl;

		//Calculate R for every cell
		calculateR();

		//Create graph for each threshold
		graph95 = generateThresholdGraph(threshold1);
		cout << "Finished creating threshold graph for .95" << endl;
		graph925 = generateThresholdGraph(threshold2);
		cout << "Finished creating threshold graph for .925" << endl;
		graph90 = generateThresholdGraph(threshold3);
		cout << "Finished creating threshold graph for .90" << endl << endl;

		//Display the histograms
		printHistograms();
		cout << "Histograms Printed" << endl << endl;

		//DFS the graphs
		cout << "Connected Components Data" << endl;
		cout << "Threshold .95" << endl;
		int tempSize = connectedComponents(graph95);
		cout << "Threshold .95 Connected Components: " << (tempSize + 783) << endl;
		cout << "Threshold .95 Connected Components (w/o land): " << tempSize << endl << endl;
		cout << "Threshold .925" << endl;
		tempSize = connectedComponents(graph925);
		cout << "Threshold .925 Connected Components: " << (tempSize + 783) << endl;
		cout << "Threshold .925 Connected Components (w/o land): " << tempSize << endl << endl;
		cout << "Threshold .90" << endl;
		tempSize = connectedComponents(graph90);
		cout << "Threshold .90 Connected Components: " << (tempSize + 783) << endl;
		cout << "Threshold .90 Connected Components (w/o land): " << tempSize << endl << endl;
		cout << "Finished Connected Components" << endl << endl;


		//Clustering Coefficient
		cout << "Clustering Coefficient Data" << endl; //For .95 base G
		float temp = clusteringCoefficient(graph95, true);
		cout << "Clustering Coefficient .95: " << temp << endl;
		temp = clusteringCoefficient(graph95, false);
		cout << "Clustering Coefficient .95 (w/o land): " << temp << endl;

		double edgeDistro95 = meanEdgeDistribution(graph95, false); //For .95 random G
		cout << "Clustering Coefficient .95 Random Graph (w/o land): " << (double)(edgeDistro95) / 3186 << endl;
		double edgeDistroLand95 = meanEdgeDistribution(graph95, true);
		cout << "Clustering Coefficient .95 Random Graph: " << (double)(edgeDistroLand95) / 3969 << endl << endl;

		temp = clusteringCoefficient(graph925, true); //For .925 base G
		cout << "Clustering Coefficient .925: " << temp << endl;
		temp = clusteringCoefficient(graph925, false);
		cout << "Clustering Coefficient .925 (w/o land): " << temp << endl;

		double edgeDistro925 = meanEdgeDistribution(graph925, false); //For .925 random G
		cout << "Clustering Coefficient .925 Random Graph (w/o land): " << (double)(edgeDistro925) / 3186 << endl;
		double edgeDistroLand925 = meanEdgeDistribution(graph925, true);
		cout << "Clustering Coefficient .925 Random Graph: " << (double)(edgeDistroLand925) / 3969 << endl << endl;

		temp = clusteringCoefficient(graph90, true); //For .90 base G
		cout << "Clustering Coefficient .90: " << temp << endl;
		temp = clusteringCoefficient(graph90, false);
		cout << "Clustering Coefficient .90 (w/o land): " << temp << endl;

		double edgeDistro90 = meanEdgeDistribution(graph90, false); //For .90 random G
		cout << "Clustering Coefficient .90 Random Graph (w/o land): " << (double)(edgeDistro90) / 3186 << endl;
		double edgeDistroLand90 = meanEdgeDistribution(graph90, true);
		cout << "Clustering Coefficient .90 Random Graph: " << (double)(edgeDistroLand90) / 3969 << endl << endl;

		cout << "Finished Clustering Coefficient Calculations" << endl << endl;

		//Floyd-Warshall Characteristic Shortest Path
		cout << "Floyd Warshall Data" << endl << endl;
		int** graph = generateGraph(graph95); //Create graph
		cout << "Graph for .95" << endl;
		floydWarshall(graph); //Find shortest path

		double result = log2(3186) / log2(edgeDistro95); //For random graph
		cout << "Characteristic Path of .95 Random Graph: " << result << endl << endl;

		for (int i = 0; i < 3969; i++) //Free memory up
			delete[] graph[i];
		delete[] graph;

		graph = generateGraph(graph925); //Create graph
		cout << "Graph for .925" << endl;
		floydWarshall(graph); //Find shortest path

		result = log2(3186) / log2(edgeDistro925); //For random graph
		cout << "Characteristic Path of .925 Random Graph: " << result << endl << endl;

		for (int i = 0; i < 3969; i++) //Free memory up
			delete[] graph[i];
		delete[] graph;

		graph = generateGraph(graph90); //Create graph
		cout << "Graph for .90" << endl;
		floydWarshall(graph); //Find shortest path

		result = log2(3186) / log2(edgeDistro90); //For random graph
		cout << "Characteristic Path of .90 Random Graph: " << result << endl << endl;

		for (int i = 0; i < 3969; i++) //Free memory up
			delete[] graph[i];
		delete[] graph;

	}
	else //Display reason for early exit
	{
		cout << "Files could not be read correctly. Likely incorrect filepath specified." << endl;
		cout << "This program uses filepath: CS310_project_subregion/year/Beaufort_Sea_diffwWWyYYYY+landmask" << endl;
	}

	return 0;
}

/*Mean Edge Distribution*/
/*Calculates the mean vertex degree per graph*/
double meanEdgeDistribution(struct threshold_graph** tptr, bool land)
{
	double edgeMean = 0;
	for (int i = 0; i < 3969; i++) //Find all the edges
	{
		edgeMean = edgeMean + tptr[i]->size;
	}
	if (land == false) //For no land
		edgeMean = (double)edgeMean / 3186;
	else //For land
		edgeMean = (double)edgeMean / 3969;

	return edgeMean;
}

/*Generate Graph*/
/*Generates a 3969 x 3969 graph for an adjacency list*/
int** generateGraph(struct threshold_graph** tptr)
{
	struct connected_Node* nptr; //For linked list
	int** fw = new int* [3969]; //Graph to return
	for (int i = 0; i < 3969; i++)
	{
		fw[i] = new int[3969];
	}

	for (int i = 0; i < 3969; i++) //Generate spots
	{
		for (int j = 0; j < 3969; j++)
		{
			nptr = tptr[i]->list;
			fw[i][j] = 999999; //Give initial value of infinity immediately
			if (i == j)
				fw[i][j] = 0; //Set to 0
			while (nptr != NULL)
			{
				if (nptr->location == j) //If node is nearby
					fw[i][j] = 1;
				nptr = nptr->next; //Iterate thru
			}
		}
	}
	return fw;
}

/*Floyd Warshall*/
/*Finds the shortest paths thru the graph*/
void floydWarshall(int** graph)
{
	for (int k = 0; k < 3969; k++) //Keep checking for distance improvements
	{
		for (int i = 0; i < 3969; i++)
		{
			if (graph[i][k] != 999999)
			{
				for (int j = 0; j <= i; j++) //swapped out for(int j = 0; j < 3969; j++)
				{
					if (graph[k][j] != 999999)
					{
						int temp = graph[i][k] + graph[k][j];
						if (temp == 999999 || temp < graph[i][j])
						{
							graph[i][j] = temp;
							graph[j][i] = temp;
						}
					}
				}
			}
		}
	}

	printFloydWarshall(graph); //Print the distribution
}

/*Print Floyd Warshall*/
/*Prints the Floyd Warshall distribution map*/
void printFloydWarshall(int** dist)
{
	double sum = 0; //For average
	int max = 0; //For max path length
	int count = 0; //For average division
	for (int i = 0; i < 3969; i++)
	{
		for (int j = i + 1; j < 3969; j++)
		{
			if (dist[i][j] < 3970) //Check for possible path
			{
				sum = sum + dist[i][j];
				count++;
				if (dist[i][j] > max) //Check for possible max
					max = dist[i][j];
			}
		}
	}
	float avg = (float)sum / count; //Return the average distance

	cout << "Characteristic Path Length: " << avg << endl;
	cout << "Maximum Possible Path length: " << max << endl;
}

/*Clustering Coefficient*/
/*Calculates the clustering coefficient of the adjacency lists*/
float clusteringCoefficient(struct threshold_graph** tptr, bool land)
{
	struct threshold_graph* lptr; //Node being looked at
	int numerator[3969]; //Holds numerator
	int denom[3969]; //Holds current denominator value k
	double cluster[3969]; //Cluster calculations
	float clusterAVG; //Returned average value at end
	int count = 0; //Count of edges
	for (int i = 0; i < 3969; i++) //Calculate the denominator
	{
		denom[i] = tptr[i]->size;
	}
	for (int i = 0; i < 3969; i++) //Numerator, Iterating thru each node
	{
		struct connected_Node* nptr = tptr[i]->list;
		while (nptr != NULL) //Iterate thru linked list of node
		{
			lptr = tptr[nptr->location]; //Node to look at
			struct connected_Node* kptr = lptr->list; //Node looked at's connected nodes
			struct connected_Node* iterator = nptr; //Iterator
			while (kptr != NULL)
			{
				if (iterator != NULL) //Check if iteration is done
				{
					int temp1 = kptr->location;
					int temp2 = iterator->location;
					if (temp1 == temp2) //Increment count of edges if identical
					{
						count++;
					}
					iterator = iterator->next; //Iterate thru list
				}
				else
				{
					kptr = kptr->next; //Move to next node in chain
					iterator = nptr; //Reset iterator
				}
			}
			nptr = nptr->next;
		}
		numerator[i] = count; //Store the size, always even
		count = 0; //Reset count
	}

	for (int i = 0; i < 3969; i++) //Calculate the clustering
	{
		if (denom[i] > 1)
			cluster[i] = (double)(2 * numerator[i]) / (denom[i] * (denom[i] - 1));
		else
			cluster[i] = 0;
	}
	clusterAVG = 0; //Set to 0 initially
	for (int i = 0; i < 3969; i++) //Calculate the sum
	{
		clusterAVG = clusterAVG + cluster[i];
	}

	if (land == true) //Check for land inclusion
		clusterAVG = (clusterAVG / 3969); //Average with land
	else
		clusterAVG = (clusterAVG / 3186); //Average without land

	return clusterAVG;
}

/*Connected Components*/
/*Calculates the connected components of the threshold graphs*/
int connectedComponents(struct threshold_graph** tptr)
{
	bool* visited = new bool[3969]; //Track visited nodes
	int* groupSizes = new int[3969]; //Track sizes of components
	int count = 0; //Track component number
	int groupSize = 0; //Track count of nodes in list

	for (int i = 0; i < 3969; i++) //Initialize tracker to false, sizes to 0
	{
		visited[i] = false;
		groupSizes[i] = 0;
	}

	for (int i = 0; i < 3969; i++)
	{
		if (visited[i] == false) //Check for visited
		{
			//cout << "Component " << (count+1) << ": ";
			groupSize = DFSUtil(i, visited, tptr); //Visit if not checked
			groupSizes[i] = groupSize; //Store the size
			//cout << endl;
			if (tptr[i]->land == false)
			{
				count++; //Increment component count
				cout << "Component #" << count << ": " << groupSize << endl;
			}
		}
	}

	for (int i = 1; i < 3969; i++) //Iterate thru list and print component sizes
	{
		int printCount = 0;
		for (int j = 0; j < 3969; j++)
		{
			if (groupSizes[j] == i) //If size matches print it
				printCount++;
		}
		if (printCount > 0) //Print size of component
		{
			cout << "Number of connected components size " << i << " is: " << printCount << endl;
			if (i == 1)
				cout << "Number of connected components size " << i << " excluding land is: " << (printCount - 783) << endl;
		}
	}
	delete visited; //Clear up memory
	delete groupSizes; //Clear up memory

	return count; //Return component number
}

/*DFS Util*/
/*Utility method for connectedComponents, recurses thru graph*/
int DFSUtil(int current, bool visited[], struct threshold_graph** tptr)
{
	visited[current] = true; //Set visited to true
	int count = 1; //Track count of nodes in list
	//cout << "(" << current << ") "; //Print current node visiting

	struct connected_Node* nptr = tptr[current]->list; //Assign node pointer to linked list
	while (nptr != NULL) //Iterate thru linked list
	{
		int location = nptr->location;
		if (!visited[location]) //Recurse if not visited yet
		{
			count += DFSUtil(location, visited, tptr);
		}
		nptr = nptr->next; //Move to next node in list
	}
	return count;
}

/*Print Histogram*/
/*Prints the histogram of the threshold graphs*/
void printHistograms()
{
	struct threshold_graph** tptr; //Temp pointer
	tptr = graph95;

	cout << "Histogram for Threshold .95" << endl;
	for (int i = 3968; i >= 0; i--)
	{
		int count = 0;
		string line = "";
		for (int j = 0; j < 3969; j++)
		{
			if (graph95[j]->size == i)
			{
				count = count + 1;
				line = line + "x ";
			}
		}
		if (i == 0)
		{
			string lineO = "";
			cout << "Size w/o land: " << i << ", ";
			for (int i = 0; i < (count - 783); i++)
			{
				lineO = lineO + "x ";
			}
			cout << "Count: " << (count - 783) << " ";
			cout << lineO << endl;
		}
		if (count > 0)
		{
			cout << "Size: " << i << ", ";
			cout << "Count: " << count << " ";
			cout << line << endl;

		}
	}

	tptr = graph925;
	cout << "Histogram for Threshold .925" << endl;
	for (int i = 3968; i >= 0; i--)
	{
		int count = 0;
		string line = "";
		for (int j = 0; j < 3969; j++)
		{
			if (graph925[j]->size == i)
			{
				count = count + 1;
				line = line + "x ";
			}
		}
		if (i == 0)
		{
			string lineO = "";
			cout << "Size w/o land: " << i << ", ";
			for (int i = 0; i < (count - 783); i++)
			{
				lineO = lineO + "x ";
			}
			cout << "Count: " << (count - 783) << " ";
			cout << lineO << endl;
		}
		if (count > 0)
		{
			cout << "Size: " << i << ", ";
			cout << "Count: " << count << " ";
			cout << line << endl;
		}
	}

	tptr = graph90;
	cout << "Histogram for Threshold .90" << endl;
	for (int i = 3968; i >= 0; i--)
	{
		int count = 0;
		string line = "";
		for (int j = 0; j < 3969; j++)
		{
			if (graph90[j]->size == i)
			{
				count = count + 1;
				line = line + "x ";
			}
		}
		if (i == 0)
		{
			string lineO = "";
			cout << "Size w/o land: " << i << ", ";
			for (int i = 0; i < (count - 783); i++)
			{
				lineO = lineO + "x ";
			}
			cout << "Count: " << (count - 783) << " ";
			cout << lineO << endl;
		}
		if (count > 0)
		{
			cout << "Size: " << i << ", ";
			cout << "Count: " << count << " ";
			cout << line << endl;
		}
	}
}

/*Generate Threshold Graph*/
/*Creates a graph from given r values based on the threshold*/
struct threshold_graph** generateThresholdGraph(float threshold)
{
	struct threshold_graph** tptr; //Temp pointer

	tptr = new threshold_graph * [3969]; //Create the array of events


	for (int i = 0; i < 3969; i++) //Initialize the struct array
	{
		tptr[i] = new threshold_graph; //Create new struct object at spot
		tptr[i]->size = 0; //Set initial size to 0
		tptr[i]->list = NULL; //No list yet
		if (dataSet[i][0][0] == 168) //Set land status
		{
			tptr[i]->land = true;
		}
		else
			tptr[i]->land = false;
	}

	for (int i = 0; i < 3969; i++) //Node1
	{
		for (int j = i; j < 3969; j++) //Node2
		{
			if (rValues[i][j] > threshold && j != i) //Check for same node
			{
				tptr[i]->size += 1; //Increment node1 size
				if (tptr[i]->list == NULL) //Add to front
				{
					tptr[i]->list = new connected_Node;
					tptr[i]->list->location = j;
					tptr[i]->list->next = NULL;
				}
				else //Shift the list over
				{
					connected_Node* newNode = new connected_Node;
					newNode->location = j;
					newNode->next = tptr[i]->list;
					tptr[i]->list = newNode;
				}
				tptr[j]->size += 1; //Increment node2 size
				if (tptr[j]->list == NULL) //Add to front
				{
					tptr[j]->list = new connected_Node;
					tptr[j]->list->location = i;
					tptr[j]->list->next = NULL;
				}
				else //Shift the list over
				{
					connected_Node* newNode = new connected_Node;
					newNode->location = i;
					newNode->next = tptr[j]->list;
					tptr[j]->list = newNode;
				}
			}
		}
	}
	return tptr;
}

/*Calculate R Value*/
/*Calculates the R values generated by the correlation coefficient*/
void calculateR()
{
	for (int i = 0; i < 3969; i++) //Node1
	{
		for (int j = i; j < 3969; j++) //Node2
		{
			float denominator;
			float numerator;
			float xx = coefficientsXX[i][0]; //Sxx for Node1
			float yy = coefficientsXX[j][0]; //Syy for Node2
			denominator = sqrt(xx * yy); //Set denominator value
			numerator = coefficientsXY[i][j]; //Set numerator value
			rValues[i][j] = numerator / denominator; //Calculate coefficient

			if (rValues[i][j] < 0) //Take absolute value
				rValues[i][j] = (rValues[i][j]) * (-1);
		}
	}
}

/*Calculate Sxy*/
/*Calculates the Sxy value for the correlation coefficient formula*/
void calculateXY(int num_years)
{
	for (int i = 0; i < 3969; i++) //Node1
	{
		for (int j = i; j < 3969; j++) //Node2
		{
			for (int l = 0; l < num_years; l++) //Years
			{
				for (int p = 0; p < 52; p++) //Weeks
				{
					if (p == 0 && l == 0)
						coefficientsXY[i][j] = 0;
					float temp1 = dataSet[i][l][p]; //Node1
					float temp2 = dataSet[j][l][p]; //Node2
					if (dataSet[i][l][p] != 168 && dataSet[i][l][p] != 157)
					{
						temp1 = temp1 - coefficientsXX[i][1]; //Subtract mean for node1
					}
					else
					{
						temp1 = 0;
					}
					if (dataSet[j][l][p] != 168 && dataSet[j][l][p] != 157)
					{
						temp2 = temp2 - coefficientsXX[j][1]; //Subtract mean for node2
					}
					else
					{
						temp2 = 0;
					}
					coefficientsXY[i][j] = coefficientsXY[i][j] + (temp1 * temp2); //Store Sxy
				}
			}
		}
		//cout << "finished: " << i << endl;
	}
}

/*Pearson Correlation Coefficient XX/YY*/
/*Calculates the Pearson Correlation Coefficient for a single node*/
void pearsonCoeffXXAndYY(int num_years)
{
	for (int i = 0; i < 3969; i++)
	{
		int count = 0;
		float sum = 0;
		float mean = 0;
		float coeff = 0;

		for (int j = 0; j < num_years; j++) //Find Sum and Count for mean calculation
		{
			for (int k = 0; k < 52; k++)
			{
				if (dataSet[i][j][k] != 157 && dataSet[i][j][k] != 168)
				{
					sum = sum + dataSet[i][j][k];
					count = count + 1;
				}
			}
		}
		if (count != 0) //If division is possible
		{
			mean = (sum / count); //Calculate the mean

			for (int j = 0; j < num_years; j++) //Calculate the coefficient
			{
				for (int k = 0; k < 52; k++)
				{
					if (dataSet[i][j][k] != 157 && dataSet[i][j][k] != 168)
					{
						coeff = coeff + pow((dataSet[i][j][k] - mean), 2.0);
					}
				}
			}

			//Print to check results
			/*cout << "(" << i << ") ";
			cout << "Mean: " << mean << " ";
			cout << "Sum: " << sum << " ";
			cout << "Count: " << count << " ";
			cout << "Coefficient: " << coeff << endl;*/

			coefficientsXX[i][0] = coeff; //Assign coeff to spot 0
			coefficientsXX[i][1] = mean; //Assign Mean to spot 1
		}
		else //Give ignored values
		{
			coefficientsXX[i][0] = 168;
			coefficientsXX[i][1] = 168;
		}
	}
}

/*Read Files*/
/*Reads the 2D float array from the current open file*/
void readFiles(ifstream& inFile, int week, int year)
{
	float f; //Used for sizing of binary file

	for (int i = 0; i < 3969; i++)
	{
		inFile.read(reinterpret_cast<char*>(&f), sizeof(float)); //Read File
		dataSet[i][year][week] = f; //Store into correct spot
	}
}

/*Open Files*/
/*Reads in the files needed for the graph functions*/
void openFiles(string** filesList, int num_years)
{
	string tempFile; //Temporarily holds the file name
	bool ableToOpen = true; //Break out of openFiles
	int i = 0;
	int j = 0;
	ifstream inFile; //Declare the file holder

	while (i < num_years && ableToOpen == true)
	{
		while (j < 52 && ableToOpen == true)
		{
			tempFile = filesList[i][j]; //Store the name in the string
			inFile.open(tempFile.c_str(), ios::binary); //Open the file
			if (inFile.fail()) //Check for file opening incorrectly
			{
				ableToOpen = false;
				endEarly = true;
				cout << "Unable to find file" << endl;
				cout << tempFile << endl;
			}
			else //If file opened successfully perform tasks
			{
				readFiles(inFile, j, i);
			}

			inFile.close(); //Close the file
			j++; //J moves to end
		}
		i++; //I moves to end
		j = 0; //Reset J
	}
}

/*Generate File Names*/
/*Generates the needed filenames for the program*/
string** generateFileNames(int num_years, int start_year)
{
	string** filesList = new string * [num_years]; //Instantiate file name array
	for (int i = 0; i < 16; i++) //Generate spot for each week
	{
		filesList[i] = new string[52];
	}

	string tempFile; //Holds the name of the file to open
	int year_int = 0; //Modifiable integer year
	string year; //Holds the modified string year
	string week; //Holds the current week

	for (int i = 0; i < num_years; i++) //For the 16 folders
	{
		ostringstream stream; //Used for conversions
		year = ""; //Reset the year string
		year_int = start_year + i; //Set the year to 1990 + i
		stream << year_int; //Convert the year to a stream
		year = stream.str(); //Convert the stream to a string

		for (int j = 1; j <= 52; j++) //For the 52 files in each folder
		{
			tempFile = ""; //Reset tempFile every time

			if (j < 10) //Add the zero to the front of the int value
			{
				ostringstream temp; //Create a temp stream
				week = "0"; //Reset week
				temp << j; //Convert j to a stream
				week = week + temp.str(); //Concatenate the string
			}
			else //Else just convert int to string
			{
				ostringstream temp; //Create a temp stream
				week = ""; //Reset week
				temp << j; //Convert j to a stream
				week = temp.str(); //Convert stream to a string
			}

			//Beaufort_SeadiffwXXyYYYY+landmask for each filename
			tempFile = "CS310_project_subregion/" + year + "/Beaufort_Sea_diffw" + week + "y" + year + "+landmask";

			filesList[i][j - 1] = tempFile; //Store the name in the array
		}
	}

	return filesList; //Return the filled array of data
}