/**********************************************************************
*
*  	KDTree.h
* 
* 	Specification and implementation (due to templating) of a the KD 
* 	tree.  The tree is a binary search tree, with wrapped Point's for 
* 	nodes.  The tree is built based on customizeable methods to select
*   nodes and partition the dataset (defaults are median and max range).
* 
***********************************************************************/
#ifndef KDTREE_H
#define KDTREE_H

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>  
#include <fstream>
#include "Point.h"

template <class T>

class KDTree{

public:

	KDTree(int dims):dims(dims){};
	
	// Alternate constructor loads tree from file
	KDTree(std::ifstream &filestream){
		
		std::string line;
		getline(filestream, line);
		
		// Parse the first line - top node of the tree
		dims = parseLine(line, point, pointAxis);
		
		// Continue building the tree
		buildChildren(filestream);
	}

	// Destructor - recursion takes care of the whole tree
	virtual ~KDTree(){
		delete left;
		delete right;
	}

	// Build the KDTree from a vector of data points
	// (public facing version of the function)	
	void build(std::vector<Point<T>> &data){
		build(data, data.begin(), data.end());
	}

	// Find the nearest neighbor to the given point
	Point<T> queryPoint(Point<T> &query){
		
		// Make a guess at the closest tree point - the current 
		// (top-level) node
		Point<T> bestGuess = point;
		bestGuess.distance = distance(point, query);
		
		// Check both sides of the tree (since the parent node is the 
		// current best guess, we know we have to check both sides 
		// without measuring at this point)
		if(left != nullptr)
			left->find(query, bestGuess);		
		if(right != nullptr)
			right->find(query, bestGuess);
		
		return bestGuess;
	}

	// Save the tree to disk.  We need to mark the NULL branches, to 
	// avoid ambiguity in reconstruction.
	void saveTree(std::ofstream &filestream){

		// Include the point index (original position in input file) and
		// partition axis direction in the save
		filestream << point.printPoint() << "," << point.index << "," 
				   << pointAxis << std::endl;
		
		// Recurse the tree 
		if(left != nullptr)
			left->saveTree(filestream);
		else
			filestream << "NULL" << std::endl;
		
		if(right != nullptr)
			right->saveTree(filestream);
		else
			filestream << "NULL" << std::endl;		
	}
	
protected:
	
	// Find the Euclidean distance between two points
	virtual double distance(Point<T> &p1, Point<T> &p2){
		double dist=0;
		for(auto it1 = std::begin(p1.coords), 
			it2 = std::begin(p2.coords); it1 != std::end(p1.coords); 
			++it1, ++it2)
			dist += (*it1-*it2)*(*it1-*it2);
		return std::sqrt(dist);
	}
	
	virtual typename std::vector<Point<T>>::iterator 
		getSplitPoint(typename std::vector<Point<T>>::iterator front, 
					  typename std::vector<Point<T>>::iterator rear){
		return front + (rear - front)/2;
	}
	
	// Find the data dimension with the greatest span inside the range
	virtual int chooseDimension(std::vector<Point<T>> &data, 
						typename std::vector<Point<T>>::iterator front, 
						typename std::vector<Point<T>>::iterator rear){

		// For each dimension in the data, run a custom minmax with the
		// same single-dimension compare lamba from sortDim()
		std::vector<T> mins, maxs;
		for(unsigned int i = 0; i < dims; i++){
			auto minMax = std::minmax_element(front,rear, 
						[i](const Point<T>& p1, const Point<T>& p2) {
							return p1.coords[i] < p2.coords[i];
                        });
			mins.push_back(minMax.first->coords[i]);
			maxs.push_back(minMax.second->coords[i]);
		}

		double maxVal = maxs[0]-mins[0];
		int maxDim = 0;
		for(auto i = 1; i < dims; i++){			
			T diff = maxs[i]-mins[i];
			if(diff > maxVal){
				maxVal = diff;
				maxDim = i;
			}
		}
		
		return maxDim;
	}

private:
	
	int dims;				// Tree Point dimensionality
	Point<T> point;			
	int pointAxis;			// Direction of current partition
	KDTree *left = nullptr;
	KDTree *right = nullptr;	
	
	// Populate part of the KDTree from the data within the given range
	void build(std::vector<Point<T>> &data, 
			   typename std::vector<Point<T>>::iterator front, 
			   typename std::vector<Point<T>>::iterator rear){
		
		// Enact the given splitting heuristic
		// Choose the splitting dimension
		int maxRangeDim = chooseDimension(data, front, rear);
		// Sort the data based on the discovered wide spanning dimension
		sortDim(data, maxRangeDim, front, rear);
		// Get the split point to store in the tree
		typename std::vector<Point<T>>::iterator split = 
											getSplitPoint(front, rear);
		
		// Store the data at the median...
		point = *split;
		// ...and the partition dimension
		pointAxis = maxRangeDim;
				
		// Use recurion to build the rest of the KD tree
		if(front != split){
			left = new KDTree<T>(dims);
			left->build(data, front, split);
		}
		if(split+1 != rear){
			right = new KDTree<T>(dims);
			right->build(data, split+1, rear);
		}	
	}
	
	// Sort the data within the current range based on the value of a 
	// single dimension
	void sortDim(std::vector<Point<T>> &data, int dim, 
				 typename std::vector<Point<T> >::iterator front, 
				 typename std::vector<Point<T> >::iterator rear){
		
		// Run the custom version of STL sort with a lambda expression 
		// that orders the points based solely on their value in the 
		// given dimension
		std::sort(front, rear, 
					[dim](const Point<T>& p1, const Point<T>& p2){ 
						return p1.coords[dim] < p2.coords[dim];
					});
	}

	// Look through the current (sub)tree to find the nearest neighbor
	void find(Point<T> &query, Point<T> &bestGuess){
		
		// Is the current point closer to the query point than the 
		// previous closest point?
		double dist = distance(point, query);
		if(dist < bestGuess.distance){
			bestGuess = point;
			bestGuess.distance = dist;
		}
		
		// If the current partition includes a sub-partition on the same
		// half as the current point, traverse it. Also, if any part of 
		// the other half of the partition is closer to the current 
		// point than the current best guess nearest neighbor, traverse
		// it as well		
		if(left != nullptr && 
			(query.coords[pointAxis] <= point.coords[pointAxis] || 
			 std::abs(point.coords[pointAxis]-query.coords[pointAxis]) 
						< bestGuess.distance))			 
			left->find(query, bestGuess);		
				
		if(right != nullptr && 
			(query.coords[pointAxis] >= point.coords[pointAxis] || 
			 std::abs(point.coords[pointAxis]-query.coords[pointAxis]) 
				< bestGuess.distance))
			right->find(query, bestGuess);
		
	}

	// Rebuild both children of a kd-tree from disk file. We need to be
	// able to "look ahead" to check if the children are NULL before we
	// allocate any memory for them.  (No empty roots in this tree)
	void buildChildren(std::ifstream &filestream){
		
		std::string line;	
		
		getline(filestream, line);		
		if(line != "NULL"){
			left = new KDTree<T>(dims);
			parseLine(line, left->point, left->pointAxis);		
			left->buildChildren(filestream);
		}
			
		getline(filestream, line);		
		if(line != "NULL"){		
			right = new KDTree<T>(dims);
			parseLine(line, right->point, right->pointAxis);
			right->buildChildren(filestream);
		}
	}

	// Parse a single line of the saved kd tree file format for recovery
	int parseLine(std::string line, Point<T> &parsedPoint, 
				  int &parsedPointAxis){
					  
		std::istringstream issLine(line);	
		std::string number;
		
		// The first length-2 numbers of the line are the node point
		int j=0;  
		for(j = 0; getline(issLine, number, ','); j++){
			std::istringstream issNumber(number);	
			T d;
			issNumber >> d;
			parsedPoint.coords.push_back(d);			 
		}
		
		// The last number in the line is the dimension of the partition
		// axis
		parsedPointAxis = parsedPoint.coords[j-1];		
		parsedPoint.coords.pop_back();
		
		// The second-to-last number of the line is the location of the
		// node point in the original input file
		parsedPoint.index = parsedPoint.coords[j-2];
		parsedPoint.coords.pop_back();

		// use the index to capture tree dimensionality (i.e. the number
		// of numbers in the node point)
		return j-1;
	}
};

#endif


