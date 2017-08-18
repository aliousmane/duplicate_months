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


using namespace std;
using namespace NETCDFUTILS;
using namespace netCDF;
using namespace boost;
using namespace DUPLICATE_MONTHS;

namespace INTERNAL_CHECKS
{
	void internal_checks(vector<station>  &station_info, bool duplicate, bool second,string *DATE)
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
				logfile << "Total station record size" << endl;//  len(station.time.data)" << endl; //len(station.time.data) n
				UTILS::create_fulltimes(&stat,process_var, DATESTART, DATEEND, carry_thru_vars);
			}
			else if (second)
			{
				//lire le fichier _mask.nc
				//match_to_compress = utils.create_fulltimes(station, process_vars, DATASTART, DATAEND, carry_thru_vars)
			}
			if (duplicate)
			{
				//Appel à la fonction duplicate_months de qc_tests
				vector<string> variable_list;
				vector<int> flag_col;
				DUPLICATE_MONTHS::dmc(stat ,variable_list, process_var,flag_col, DATESTART, DATEEND,logfile);
			}

			//Write to file
			if (first)
			{
			}
			else if (second)
			{

			}
			logfile << boost::gregorian::day_clock::local_day() << endl;
			logfile << "processing took " << posix_time::second_clock::local_time() - process_start_time << "  s" << endl;
			if (logfile)
				logfile.close();
			cout << "Internal Checks completed" << endl;
		}
		//Fermeture du fichier
	}
}