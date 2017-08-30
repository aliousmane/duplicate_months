#include"Internal_checks.h"
#include<vector>
#include <ctime>
#include "station.h"
#include<sstream>
#include<iostream>
#include <string>
#include<fstream>
#include "netCDFUtils.h"
#include "netcdf.h"
#include "ncFile.h"
#include "ncDim.h"
#include "ncVar.h"
#include<errno.h>
#include<exception>
#include <cmath>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include<boost/filesystem.hpp>
#include<boost/chrono.hpp>
#include "duplicate_months.h"
#include "utils.h"
#include "frequent_values.h"


using namespace std;
using namespace NETCDFUTILS;
using namespace netCDF;
using namespace boost;


namespace INTERNAL_CHECKS
{
	void internal_checks(vector<station>  &station_info, test mytest, bool second, string *DATE)
	{
		boost::gregorian::date  DATESTART = boost::gregorian::date_from_iso_string(DATE[0]);
		boost::gregorian::date  DATEEND = boost::gregorian::date_from_iso_string(DATE[1]);
		bool first = !second;
		
		int st = 0;
		for (station stat: station_info )
		{
			
			cout << boost::gregorian::day_clock::local_day() << endl;  //Date du jour au format Www Mmm dd hh:mm:ss yyyy\n
			cout << "Station Number :  " << st + 1 << " / " << station_info.size() << endl;
			cout << "Station Identifier:  " << (stat).getId() << endl;
			// Ouverture du fichier en mode ecriture lors du premier appel de la fonction ou en mode append la seconde fois
			ofstream logfile;
			stringstream sst;
			sst << LOG_OUTFILE_LOCS << (stat).getId() << ".log";

			if (first)
			{
				try{logfile.open(sst.str().c_str());}
				catch (std::exception e)
				{
					cout << e.what() << endl;
				}
			}
			else
			{
				try{logfile.open(sst.str().c_str(), ofstream::out | ofstream::app);}
				catch (std::exception e)
				{
					cout << e.what() << endl;
				}
			}
			logfile << boost::gregorian::day_clock::local_day() << endl;
			logfile << "Internal Checks" << endl;
			logfile << "Station Identifier : " << (stat).getId() << endl;

			boost::posix_time::ptime process_start_time = boost::posix_time::second_clock::local_time();
			
			/* latitude and longitude check  */
			if (std::abs((stat).getLat()) > 90.)
			{
				logfile << (stat).getId() << "	Latitude check" << DATESTART.year()<<DATEEND.year()-1<<"  Unphysical latitude ";
				logfile.close();
				continue;
			}
			if (std::abs((stat).getLon()) > 180.)
			{
				logfile << (stat).getId() << "Longitude Check"  << DATESTART.year() << DATEEND.year() - 1 << "  Unphysical longitude ";
				logfile.close();
				continue;
			}
			// if running through the first time
			valarray<bool> match_to_compress;
			if (first)
			{
				// tester si le fichier existe
				string filename = NETCDF_DATA_LOCS + (stat).getId() + ".nc";
				boost::filesystem::path p{ filename};
				try
				{	boost::filesystem::exists(p);}
				catch ( std::exception&  e)
				{
					cout << e.what() << endl;
				}
				//read in data
				NETCDFUTILS::read(filename,&stat,process_var,carry_thru_vars);
				
				//lire dans le fichier netcdf
				logfile << "Total station record size" << stat.getMetvar("time")->getData().size() << endl;

				match_to_compress = UTILS::create_fulltimes(&stat,process_var, DATESTART, DATEEND, carry_thru_vars);

				//Initialiser station.qc_flags
				for (string var : process_var)
				{
					MetVar* st_var = stat.getMetvar(var);
					//st_var.setReportingStats(/*utils.monthly_reporting_statistics(st_var, DATASTART, DATAEND)*/);
				}
			}
			else if (second)
			{
				string filename = NETCDF_DATA_LOCS + (stat).getId() + "_mask.nc";
				boost::filesystem::path p{ filename };
				try
				{
					boost::filesystem::exists(p);
				}
				catch (std::exception&  e)
				{
					cout << e.what() << endl;
				}
				NETCDFUTILS::read(filename, &stat, process_var, carry_thru_vars);
				match_to_compress = UTILS::create_fulltimes(&stat, process_var, DATESTART, DATEEND, carry_thru_vars);
			}
			if (mytest.duplicate) //check on temperature ONLY
			{
				//Appel � la fonction duplicate_months de qc_tests
				vector<string> variable_list = { "temperature" };
				
				DUPLICATE_MONTHS::dmc(stat,variable_list, process_var,0, DATESTART, DATEEND,logfile);
			}
			if (mytest.odd)
			{

			}
			if (mytest.frequent)
			{
				/*vector<string> variable_list;
				variable_list.push_back("temperatures");
				variable_list.push_back("dewpoints");
				variable_list.push_back("slp");
				vector<int> flag_col;
				flag_col.push_back(3);
				flag_col.push_back(2);
				flag_col.push_back(1);*/
				FREQUENT_VALUES::fvc(&stat, { "temperatures","dewpoints","slp" }, { 1, 2, 3 }, DATESTART, DATEEND, logfile);
			}
			if (mytest.diurnal)
			{
				if (std::abs(stat.getLat() <= 60))
					cout << "  ";
				else
					logfile << "Diurnal Cycle Check not run as station latitude" << stat.getLat()<< "  > 60 ";
			}

			//Write to file
			if (first)
			{
				string filename = NETCDF_DATA_LOCS + (stat).getId() + "_internal.nc";
				NETCDFUTILS::write(filename, &stat, process_var, carry_thru_vars, match_to_compress);
			}
			else if (second)
			{
				string filename = NETCDF_DATA_LOCS + (stat).getId() + "_internal2.nc";
				NETCDFUTILS::write(filename, &stat, process_var, carry_thru_vars, match_to_compress);
			}
			logfile << boost::gregorian::day_clock::local_day() << endl;
			logfile << "processing took " << posix_time::second_clock::local_time() - process_start_time << "  s" << endl;
			if (logfile)
				logfile.close();//Fermeture du fichier
			cout << "Internal Checks completed" << endl;
		} //end for station
		
	}
}