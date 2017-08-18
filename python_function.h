#pragma once
#include<algorithm>
#include <vector>
#include <valarray>


namespace PYTHON_FUNCTION
{
	/*Return evenly spaced numbers over a specified interval.
	//
	//Returns `num` evenly spaced samples, calculated over the
	//interval[`start`, `stop`].*/
	template<typename T>
	inline void linspace(std::vector<T> *t, int start, int stop, int number)
	{

		int delta = int((stop - start) / (number - 1));
		for (int i = 0; i < number; i++)
			(*t).push_back(start + i*delta);
	}
	
	
	inline void arange(std::vector<int> *t, int stop, int start = 0)
	{

		for (int i = start; i < stop; i++)
			(*t).push_back(i);
	}

	/*Test whether each element of a 1 - D array is also present in a second array
	Returns a boolean array the same length as array 1 that is true where
	an element of array 1 is in array 2 and false otherwise
	@Docstring numpy.in1D python*/
	template<typename T>
	inline std::valarray<T> in1D(std::vector<int> v1, std::vector<int> v2)
	{
		std::valarray<T> vec(v1.size());
		for (int i = 0; i < v1.size(); i++)
		{
			if (std::binary_search(v2.begin(), v2.end(), (v1.at(i))))
				vec[i]=true;
			else vec[i]=false;
		}
		return vec;
	}
}
