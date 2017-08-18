#include"utils.h"
#include "station.h"
#include<vector>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include< map>
#include <string>
#include "python_function.h"
#include <valarray> 


using namespace std;
using namespace boost;
using namespace PYTHON_FUNCTION;
namespace UTILS
{
	/*
		Returns locations of month starts(using hours as index)
	*/
	inline vector<int> month_starts(boost::gregorian::date start, boost::gregorian::date end)
	{
		vector<int> month_locs;
		boost::gregorian::date Date = start;
		while (Date < end)
		{
			boost::gregorian::date_duration difference = Date - start;
			month_locs.push_back(difference.days() * 24);
				//increment counter
				if (Date.month() < 10)
					Date = boost::gregorian::date_from_iso_string(""+to_string(Date.year())+"0"+to_string((Date.month()+1))+"01"); 
				else if (Date.month() < 12)
					Date = boost::gregorian::date_from_iso_string("" + to_string(Date.year()) + to_string((Date.month()+1)) + "01");
				else
					Date = boost::gregorian::date_from_iso_string("" + to_string(Date.year()+1) + to_string((Date.month() + 1)) + "01");
					
		}
		return month_locs;
	}
	/*
		Create array of month start / end pairs
			: param datetime start : start of data
			: param datetime end : end of data
			: returns : month_ranges : Nx2 array
	*/
	inline map<int,int> month_starts_in_pairs(boost::gregorian::date start, boost::gregorian::date end)
	{
			//set up the arrays of month start locations
			vector<int> m_starts = month_starts(start, end);

		    map<int,int > month_ranges; 
			
			for (int i =0 ; i < m_starts.size()-1; i++)
			{
				month_ranges[m_starts[i]] = m_starts[i + 1];
			}
			boost::gregorian::date_duration difference = end - start;
			
			month_ranges[m_starts[m_starts.size()-1]] = difference.days() * 24;

			return month_ranges;
	}

	void create_fulltimes(station* stat, vector<string> var_list, boost::gregorian::date start,boost::gregorian::date end, vector<string> opt_var_list, bool do_input_station_id, bool do_qc_flags, bool do_flagged_obs)
	{
		//expand the time axis of the variables
		boost::gregorian::date_duration DaysBetween = end - start;
		vector<int> fulltimes;
		
		//adjust if input netCDF file has different start date to desired
		cout << stat->getTime_units();
		string time_units = stat->getTime_units();
		cout << time_units << endl;
		//Extraire la date de la chaîne
		time_units = time_units.substr(time_units.find("since "));  
		time_units = time_units.substr(time_units.find(" "));
		time_units.erase(std::remove(time_units.begin(), time_units.end(), ' '), time_units.end());
		boost::gregorian::date  netcdf_start = boost::gregorian::date_from_iso_string(time_units);
		boost::gregorian::date_duration offset = start- netcdf_start;
		PYTHON_FUNCTION::arange(&fulltimes, DaysBetween.days() * 24,offset.days()*24);

		valarray<bool> match, match_reverse;

		match = PYTHON_FUNCTION::in1D<bool>(fulltimes, stat->getTime_data());
		match_reverse = PYTHON_FUNCTION::in1D<bool>(stat->getTime_data(), fulltimes);

		//if optional/carry through variables given, then set to extract these too
		vector<string> full_var_list;
		copy(var_list.begin(), var_list.end(), std::back_inserter(full_var_list));

		if (opt_var_list.size() != 0)
		{
			copy(opt_var_list.begin(), opt_var_list.end(), std::back_inserter(full_var_list));
		}
		vector<string> final_var_list;
		copy(full_var_list.begin(), full_var_list.end(), std::back_inserter(final_var_list));
		if (do_input_station_id)  final_var_list.push_back("input_station_id");

		for (vector<string>::iterator variable = final_var_list.begin(); variable != final_var_list.end(); variable++)
		{
			MetVar st_var = stat->getMetvar(*variable);
			//use masked arrays for ease of filtering later
			
			std::valarray<string> valmask(st_var.getMdi(),fulltimes.size());
			valmask[match];
			st_var.getData()[match_reverse];
			//but re-mask those filled timestamps which have missing data

		}
			
	}
}