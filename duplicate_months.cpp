
#include"station.h"
#include <vector>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <map>
#include "utils.h"
#include<iostream>
#include"MetVar.h"

using namespace std;
using namespace UTILS;



namespace DUPLICATE_MONTHS
{
	/*
    :param obj station stat: station object with suitable attributes 
    :param list variable_list: list of netcdf variables to process
    :param list full_variable_list: the variables for flags to be applied to
    :param list flag_col: which column to set in flag array
    :param datetime start: data start
    :param datetime end: data end
    :param file logfile: logfile to store outputs
    
	*/
	void dmc(station stat, std::vector<std::string> variable_list, std::vector<std::string> full_variable_list,
		std::vector<int> flag_col, boost::gregorian::date start, boost::gregorian::date end, ofstream &logfile)
	{
		const  int MIN_DATA_REQUIRED = 20;
		//get array of MAP<INT,INT> start / end pairs
		map<int, int > month_ranges;
		//month_ranges = UTILS::month_starts_in_pairs(start, end);
		int v = 0;
		for (vector<string>::iterator variable = variable_list.begin(); variable != variable_list.end(); ++variable)
		{
			MetVar *st_var = stat.getMetvar(*variable); //Recuperer la variable meteo de la station
			vector<int> duplicated;
			int sm = 0;
			for (map<int, int>::iterator source_month = month_ranges.begin(); source_month != month_ranges.end(); ++source_month)
			{
				cout << "Month" << (sm + 1) << "of " << month_ranges.size() << endl;
				vector<float> source_data;  //st_var.data[int(source_month[0]) : int(source_month[1])]  source_month[0]=
				if (duplicated[sm] == 0)   // don't repeat if already a duplicated
				{
					int tm = 0;
					for (map<int, int>::iterator target_month = source_month; target_month != month_ranges.end(); ++target_month)
					{
						if (target_month == source_month) continue;
						vector<float> target_data ; //st_var.data[int(target_month[0]) : int(target_month[1])]
						tm++;
						//match the data periods
						int overlap = std::min(source_data.size(), target_data.size());
						vector<float> s_data, t_data;
						for (int i = 0; i < overlap; i++)
						{
							s_data[i] = source_data[i];
							t_data[i] = target_data[i];
						}
					}
				}
			}
		}


	}

}