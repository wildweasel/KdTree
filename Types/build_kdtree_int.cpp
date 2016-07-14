/**********************************************************************
*
*  	build_kdtree.cpp
* 
* 	Build a KD Tree of ints from correctly formatted data (csv), then
*  	save the tree to disk.
* 
* 	USAGE: build_kdtree <data input filename> <savetree filename>
* 
***********************************************************************/
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include "../KDTree.h"

// Simplify change of input coordinate type
typedef int coord;

// Input the data points for the KD tree into a local storage vector
// Assume correct formatting
int parseDataFile(std::ifstream &dataFile, 
				  std::vector<Point<coord>> &data){
	
	// Use the inner loop index to get the dimensionality of the data
	unsigned int j = 0;
	
	std::string line;
	for(unsigned int i=0; getline(dataFile, line); i++){
		std::istringstream issLine(line);	
		std::string number;
		
		// New point
		data.push_back(Point<coord>(i));
		for(j = 0; getline(issLine, number, ','); j++){
			std::istringstream issNumber(number);	
			coord d;
			issNumber >> d;
			data[i].coords.push_back(d);						 
		}				
	}
	return j;
}

// build_kdtree requires 2 arguments - the name of the input data for 
// the tree and the filename for the disk save.
int main(int argc, char *argv[]){
	
	if (argc != 3) 
		std::cout << "USAGE: build_kdtree <data input filename> " << 
				     "<savetree filename>" << std::endl;
	else{
		// load the given tree points
		std::ifstream dataInputFileStream(argv[1]);
		if (!dataInputFileStream.is_open())
			std::cout << "Could not open data input file" << std::endl;
		else{
			// Get the tree data
			std::vector<Point<coord>> data;			
			int dims = parseDataFile(dataInputFileStream, data);
			dataInputFileStream.close();
			
			// Build the tree
			KDTree<coord> kdtree(dims);			
			kdtree.build(data);

			// Save the tree
			std::ofstream saveTreeFileStream(argv[2]);
			if (!saveTreeFileStream.is_open())
				std::cout << "Could not open file for saving tree" 
					      << std::endl;
			else{
				kdtree.saveTree(saveTreeFileStream);
				saveTreeFileStream.close();
			}
		}
	}
	return 0;
}
