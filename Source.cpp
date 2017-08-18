
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include "netcdf.h"
#include "ncFile.h"
#include "ncDim.h"
#include "ncException.h"
#include "ncVar.h"
#include "ncVarAtt.h"


using namespace std;
using namespace netCDF;

using namespace netCDF::exceptions;

template<typename T>
inline vector<bool> in1D(std::vector<T> *v1, std::vector<T> *v2)
{
	vector<bool> vec;
	for (int i = 0; i < v1->size(); i++)
	{
		if (std::binary_search(v2->begin(), v2->end(), (v1->at(i))))
			vec.push_back(true);
		else vec.push_back(false);
	}
	return vec;
}
template<typename T>
inline void CreateTab(std::vector<T> v_tab, T *tab)
{

	for (int i = 0; i < v_tab.size(); i++)
	{
		*(tab + i) = v_tab[i];
	}
}
template<typename T>
inline T getAttribute(netCDF::NcVar var, const std::string chaine)
{
	netCDF::NcVarAtt var_att = var.getAtt(chaine);
	T att;
	var_att.getValues(att);
	return att;
}
int main()
{
	string nom_file = "Hour since 20150101";
	int HoursBetween = 10;
	/*int * t_temperatures = new int[HoursBetween];
	std::vector<int> v_tab;
	for (int i = 0; i < 10; i++)
		v_tab.push_back(i + 1);
	CreateTab<int>(v_tab, t_temperatures);
	for (int i = 0; i < 10; i++)
		cout << t_temperatures[i] << endl;
	delete t_temperatures;*/
	const string filename = "C:\\Users\\oali\\Documents\\Visual Studio 2013\\Projects\\duplicate_months\\duplicate_months\\Data\\Weather\\NetCDF_files\\8300418H.nc";
	NcFile ncFile;
	//try
	//{
	//	//Ouverture du fichier netcdf  relié à la station en mode read
	//	NcFile ncFile(filename, NcFile::replace, NcFile::nc4classic);

	//	NcDim time = ncFile.addDim("time", HoursBetween);
	//	cout << ncFile.getDimCount() << endl;
	//}
	//catch (NcException &e)
	//{
	//	cerr << e.what() << endl;
	//}
	
	try{
		//NcFile ncFile(filename, NcFile::read, NcFile::nc4classic);
		//NcVar var;
		//var = ncFile.getVar("temperatures");
		//NcVarAtt var_att = var.getAtt("long_name");
		// string att;
		//var_att.getValues(att);
		//cout << att << endl;
		//cout << var.getAttCount() << endl;
		//
	
		//	 att = getAttribute<string>(var, "long_name");
		//	//Instanciation  objet MetVar 
		//
		//	//Attribut unit
		//	att=getAttribute<string>(var, "units");
		//	//attribut dtype
		//				
		//	att=getAttribute<string>(var, "missing_value");
		//	NcGroupAtt attr = ncFile.getAtt("history");
		//	attr.getValues(att);
		//	cout << att << endl;
		//	////Attribut valid_max
			//this_var.setValidMax(getAttribute<string>(var, "valid_max"));
			////Attribut valid_min
			//this_var.setValidMin(getAttribute<string>(var, "valid_min"));
			////Attribut standard_name
			//this_var.setStandard_name(getAttribute<string>(var, "standard_name"));
			////Attribut coordinates
			//this_var.setCoordinates(getAttribute<string>(var, "coordinates"));
			////Attribut cell_methods
			//this_var.setCellmethods(getAttribute<string>(var, "cell_methods"));
			////flag à implementer plus tard
			//stat.setMetVar(this_var, *variable);
		
		//read in the flagged_obs array
		nom_file = nom_file.substr(nom_file.find("since"));
		nom_file = nom_file.substr(nom_file.find(' '));
		cout << nom_file << endl;

		vector<int> v1, v2;
		vector<bool> vec;
		v1 = { 05, 51, 52, 52 };
		v2 = { 0, 1, 2 };
		vec = in1D(&v2,&v1);
		for(int i = 0; i < vec.size(); i++)
			cout << vec[i] << "\t";
		cout << endl;
		std::vector<int> nums{ 3, 4, 2, 8, 15, 267 };

		auto print = [](const int& n) { std::cout << " gg" << n; };

		std::cout << "before:";
		std::for_each(nums.begin(), nums.end(), print);
		std::cout << '\n';
	}
	catch (NcException &e)
	{
		cerr << e.what() << endl;
	}
	std::system("PAUSE");
}