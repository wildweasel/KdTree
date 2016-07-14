/**********************************************************************
*
*  	Point.h
* 
* 	Specification and implementation (due to templating) of a simple 
* 	wrapper around a vector representing a single input to or point in
* 	the kd-tree.  The wrapper also is capable of keeping the original
* 	file index and the distance to the nearest neighbor stored with the
* 	Point.
* 
***********************************************************************/
#ifndef POINT_H
#define POINT_H

#include <vector>
#include <string>
#include <sstream>

template <class T>

class Point{
public:	
	// The point
	std::vector<T> coords;
	int index;
	// Don't use the template for distance - we want distance precision
	// even for less precise data types (esp. integers)
	double distance;

	Point(){}
	Point(int i):index(i){}
	
	// Simple string formatting output method
	std::string printPoint(){
		if(coords.empty())
			return "";
			
		std::stringstream out;		
		for (auto i: coords)
			out << i << ',';
		// clear the trailing comma
		std::string outStr = out.str();
		outStr.pop_back();
		return outStr;
	}
};

#endif
