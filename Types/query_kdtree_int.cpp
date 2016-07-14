/**********************************************************************
*
*  	query_kdtree.cpp
* 
* 	Read in a integer KD Tree saved by "build_kdtree", read in sample 
* 	data from csv (correctly formatted), run the sample data through the
*  	tree and output the results to a file
* 
* 	USAGE: query_kdtree <load tree filename> <query list filename> 
* 						<result filename>
* 
***********************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "../KDTree.h"

// Simplify change of input coordinate type
typedef int coord;

// query_kdtree requires 3 arguments - the name of the saved tree data,
// the name of the file containing the sample points, and the filename
// for the saved results
int main(int argc, char *argv[]){
	
	if (argc != 4) 
		std::cout << "USAGE: query_kdtree <load tree filename>" << 
			     "<query list filename> <result filename>" << std::endl;
	else{
		// Load the saved tree
		std::ifstream loadTreeFileStream(argv[1]);
		if (!loadTreeFileStream.is_open())
			std::cout << "Could not open tree file" << std::endl;
		else{
			// Rebuild the tree
			KDTree<coord> kdtree(loadTreeFileStream);
			loadTreeFileStream.close();
						
			// Get the sample data
			std::ifstream queryListFileStream(argv[2]);
			if (!queryListFileStream.is_open())
				std::cout << "Could not open query list file" 
						  << std::endl;
			else{
				
				// Open a file for results
				std::ofstream resultFileStream(argv[3]);
				if (!resultFileStream.is_open())
					std::cout << "Couldn't open file for saving results"
							  << std::endl;
				else{
					// Run each sample data point through the tree
					for(std::string line; getline(queryListFileStream, 
												  line);){
						
						std::istringstream issLine(line);													
						Point<coord> queryVals;												
						for(std::string number; getline(issLine, number,
														','); ){
							std::istringstream issNumber(number);	
							coord d;
							issNumber >> d;
							queryVals.coords.push_back(d);			 
						}		
						// Find the nearest neighbor
						Point<coord> closest = 
								kdtree.queryPoint(queryVals);
						// Write the nearest neighbor's index to results
						resultFileStream << closest.index << "," << 
										 closest.distance << std::endl;
					}			
					resultFileStream.close();
				}
				queryListFileStream.close();
			}
		}
	}
}
