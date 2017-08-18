#include "netCDFUtils.h"
#include <string>
#include<iostream>
#include<sstream>
#include<fstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tokenizer.hpp>
#include<boost/filesystem.hpp>
#include<boost/format.hpp>
#include<ctime>
#include<iterator>
#include "netcdf.h"
#include "ncFile.h"
#include "ncDim.h"
#include "ncVar.h"
#include <vector>
#include "station.h"
#include <zlib.h>
#include <cstring>
#include <boost/filesystem.hpp>
#include "ncException.h"
#include "map"
#include "ncVarAtt.h"
#include "python_function.h"


using namespace PYTHON_FUNCTION;
using namespace boost;
using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;


namespace NETCDFUTILS
{

	void MakeNetcdfFiles(const string fichier, string *DATE, station * stat)
	{
		string namefile = stat->getId();


		boost::gregorian::date  DATESTART = boost::gregorian::date_from_iso_string(DATE[0]);
		boost::gregorian::date  DATEEND = boost::gregorian::date_from_iso_string(DATE[1]);
		//duree entre DATASTART et DATAEND
		boost::gregorian::date_duration DaysBetween = DATEEND - DATESTART;
		cout << DaysBetween.days();
		int HoursBetween = int(DaysBetween.days()) * 24;
		cout << HoursBetween << endl;
		vector<int> TimeStamps;
		PYTHON_FUNCTION::linspace<int>(&TimeStamps, 0, HoursBetween - 1, HoursBetween);
		vector<int> ValidYears;
		PYTHON_FUNCTION::linspace<int>(&ValidYears, int(DATESTART.year()), int(DATEEND.year() - 1), int(DATEEND.year() - 1) - int(DATESTART.year()) + 1);
		string dubiousfile = LOG_OUTFILE_LOCS + "dubious_data_files.txt";
		boost::gregorian::date  dbg_sttime = boost::gregorian::day_clock::local_day();

		//Création des tableaux où mettre les variables meteo

		std::vector<double> temperatures;
		std::vector<int> temperature_flags;
		std::vector<double> dewpoints;
		std::vector<int>dewpoint_flags;
		std::vector<double> windspeeds;
		std::vector<int> windspeeds_flags;
		std::vector<double> humidity;
		std::vector<int> humiditys_flags;
		std::vector<double> winddirs;
		std::vector<int> winddirs_flags;
		std::vector<double>  pressure;
		std::vector<int> pressure_flags;


		//Tells you what the true input station id was for the duplicate
		//using list as string array.
		vector<string> input_station_id;
		for (int i = 0; i < HoursBetween; i++)
			input_station_id.push_back("NULL");

		//Initialisation des tableaux de variables meteo
		fill<double>(&temperatures, HoursBetween);
		fill<int>(&temperature_flags, HoursBetween);
		fill<double>(&dewpoints, HoursBetween);
		fill<int>(&dewpoint_flags, HoursBetween);
		fill<double>(&windspeeds, HoursBetween);
		fill<int>(&windspeeds_flags, HoursBetween);
		fill<double>(&humidity, HoursBetween);
		fill<int>(&humiditys_flags, HoursBetween);
		fill<double>(&winddirs, HoursBetween);
		fill<int>(&winddirs_flags, HoursBetween);
		fill<double>(&pressure, HoursBetween);
		fill<int>(&pressure_flags, HoursBetween);


		//if extra : Ajouter des variables meteo supplémentaires

		boost::gregorian::date  dbg_lasttime = boost::gregorian::day_clock::local_day();
		//ouvrir le fichier csv qui contient des données
		int  last_obs_time = 0;
		ifstream input;
		stringstream sst;
		sst << fichier;
		string chaine = sst.str();
		char_separator<char> sep(",");
		boost::filesystem::exists(chaine.c_str());
		try
		{
			input.open(chaine.c_str());
		}
		catch (std::exception e)
		{
			cout << e.what() << endl;
		}
		if (!input)exit(-1);
		string ligne = "";
		getline(input, ligne);
		while (!input.eof())
		{

			string year, month, day;
			int hour;
			getline(input, ligne);
			if (ligne == "") break;
			int i = 0;
			string data[10] = { "0" };
			tokenizer<char_separator<char>> tokens(ligne, sep);
			for (const string& t : tokens)
			{
				data[i] = t;
				//cout << data[i] << endl;
				i++;
			}
			year = data[0];
			month = (data[1].size() == 1) ? "0" + data[1] : data[1];
			day = (data[2].size() == 1) ? "0" + data[2] : data[2];
			hour = atoi(data[3].c_str());
			boost::gregorian::date  dt_time;
			cout << year << month << day << data[3] << "	";;
			try
			{
				dt_time = boost::gregorian::date_from_iso_string(year + month + day);
				//throw boost::gregorian::bad_day_of_month();
			}
			catch (boost::exception &e)
			{
				cout << "error with boost::gregorian";
			}
			//duree entre DATASTART et la date de l'observation
			boost::gregorian::date_duration obs_date = dt_time - DATESTART;
			//cout << dt_time << "\t";
			int  obs_time = obs_date.days() * 24 + hour;
			cout << obs_time << "\t";
			int time_loc = obs_time;
			//test if this time_loc out of bounds:
			if (time_loc != HoursBetween)
			{
				double currentT = temperatures[time_loc];
				double currentD = dewpoints[time_loc];
				double newT = atof(data[4].c_str());//temperature
				double newD = atof(data[5].c_str());//dewpoint
				bool Extract = false;
				// no extract happened for this time stamp as yet - so extract
				if (input_station_id[time_loc] == "null") // this is not an overwrite, so nothing extra needs doing
					Extract = true;
				//if currently have no T or D data
				else if (currentT == FLTMDI && currentD == FLTMDI)
				{
					if (input_station_id[time_loc] == namefile)
					{
						if (newT != FLTMDI || newD != FLTMDI)
							Extract = true;
						else if (last_obs_time != 0)
							if (std::abs(TimeStamps[time_loc] - obs_time) < std::abs(TimeStamps[time_loc] - last_obs_time))
								Extract = true;
					}
					else Extract = true;
				}
				//if already have T but _no_ D OR D but _no_ T, but new one has T and D, take this line
				//   this is an overwrite - so also check that overwriting with the same station
				else if (((currentT != FLTMDI && currentD == FLTMDI) || (currentT == FLTMDI && currentD != FLTMDI)) && (newT != FLTMDI && newD != FLTMDI))
				{
					if (input_station_id[time_loc] == namefile)
						Extract = true;
				}
				//have D but no T, and new observation comes up with T, select if closer
				else if ((currentT == FLTMDI && currentD != FLTMDI) && (newT != FLTMDI))
				{
					// if overwriting, only do so with observation closer to time stamp
					if (input_station_id[time_loc] == namefile)// # tests if already read into this time stamp
					{
						if (last_obs_time != 0)//  not really necessary as already have at least 1 obs, but still...
							if (std::abs(TimeStamps[time_loc] - obs_time) < std::abs(TimeStamps[time_loc] - last_obs_time)) //# if time stamp closer than last one
								Extract = true;
					}
				}
				// if already have T and D, and new one also has T and D, but at closer time stamp, take this line
				//				#    this is an overwrite - so also check that overwriting with the same station
				else if ((currentT != FLTMDI && currentD != FLTMDI) && (newT != FLTMDI && newD != FLTMDI))
				{
					if (input_station_id[time_loc] == namefile)
					{
						if (last_obs_time != 0)  // not really necessary as already have at least 1 obs, but still...
							if (std::abs(TimeStamps[time_loc] - obs_time) < std::abs(TimeStamps[time_loc] - last_obs_time)) //# if time stamp closer than last one
								Extract = true;
					}
				}
				else Extract = false;   //just in case;
				//sort last obs_time -
				last_obs_time = obs_time;
				if (input_station_id[time_loc] == "null")
					input_station_id[time_loc] = namefile;
				//main variables
				if (Extract)
				{
					temperatures[time_loc] = atof(data[4].c_str());
					temperature_flags[time_loc] = INTMDI;
					dewpoints[time_loc] = atof(data[5].c_str());
					dewpoint_flags[time_loc] = INTMDI;
					humidity[time_loc] = atof(data[6].c_str());
					humiditys_flags[time_loc] = INTMDI;
					windspeeds[time_loc] = atof(data[7].c_str());
					windspeeds_flags[time_loc] = INTMDI;
					winddirs[time_loc] = atof(data[7].c_str());
					winddirs_flags[time_loc] = INTMDI;
					pressure[time_loc] = atof(data[9].c_str());
					pressure_flags[time_loc] = INTMDI;

					// Optional variables
				}
			}
		}
		//Creation des fichiers netcdf

		string netcdf_filename = NETCDF_DATA_LOCS + namefile + ".nc";
		char const * const FileName = netcdf_filename.c_str();
		NcFile ncFile(FileName, NcFile::replace, NcFile::nc4classic);
		//Creer les dimensions
		NcDim time = ncFile.addDim("time", HoursBetween);
		NcDim char_len = ncFile.addDim("character_length", 4);
		NcDim long_char_len = ncFile.addDim("long_character_length", 12);
		NcDim coords_len = ncFile.addDim("coordinate_length", 1);
		//Ecrire les coordonnées
		double t_latitude[1];
		double t_longitude[1];
		double t_elevation[1];
		t_latitude[0] = stat->getLat();
		t_longitude[0] = stat->getLon();
		t_elevation[0] = stat->getElev();
		write_coordinates<double>(&ncFile, "latitude", coords_len, "latitude", "station_latitude", "degrees_north", "Y", t_latitude);
		write_coordinates<double>(&ncFile, "longitude", coords_len, "longitude", "station_longitude", "degrees_east", "X", t_longitude);
		write_coordinates<double>(&ncFile, "elevation", coords_len, "surface_altitude", "vertical distance above the surface", "meters", "Z", t_elevation);

		//station ID as base variable
		NcVar nc_var = ncFile.addVar("station id", ncChar, long_char_len);
		nc_var.putAtt("Standard_name", "station_identification_code");
		nc_var.putAtt("long_name", "Station ID number");
		char strtoarray_ID[10];
		strcpy(strtoarray_ID, stat->getId().c_str());
		nc_var.putVar(strtoarray_ID);
		// create variables
		NcVar timesvar = ncFile.addVar("time", ncDouble, time);
		timesvar.setCompression(false, true, 9);
		NcVar stationsvar = ncFile.addVar("input_station_id", ncChar, (time, long_char_len));
		stationsvar.setCompression(false, true, 9);
		NcVar tempsvar = ncFile.addVar("temperatures", ncDouble, time);
		tempsvar.setCompression(false, true, 9);
		NcVar tempsflagsvar = ncFile.addVar("temperature_flags", ncShort, time);
		tempsflagsvar.setCompression(false, true, 9);
		NcVar dewsvar = ncFile.addVar("dewpoints", ncDouble, time);
		dewsvar.setCompression(false, true, 9);
		NcVar dewsflagsvar = ncFile.addVar("dewpoint_flags", ncShort, time);
		dewsflagsvar.setCompression(false, true, 9);
		NcVar wsvar = ncFile.addVar("windspeeds", ncDouble, time);
		wsvar.setCompression(false, true, 9);
		NcVar wsfvar = ncFile.addVar("windspeeds_flags", ncShort, time);
		wsfvar.setCompression(false, true, 9);
		NcVar wdvar = ncFile.addVar("winddirs", ncDouble, time);
		NcVar  wdfvar = ncFile.addVar("winddirs_flags", ncShort, time);
		wdvar.setCompression(false, true, 9);
		NcVar  prvar = ncFile.addVar("pressure", ncDouble, time);
		prvar.setCompression(false, true, 9);
		NcVar  prfvar = ncFile.addVar("pressure_flags", ncShort, time);
		prfvar.setCompression(false, true, 9);
		timesvar.putAtt("long_name", "time_of_measurement");
		timesvar.putAtt("standard_name", "time");
		timesvar.putAtt("units", "hours since "+DATE[0]);
		timesvar.putAtt("axis", "T");
		timesvar.putAtt("calendar", "gregorian");
		timesvar.putAtt("valid_min", "0.");
		timesvar.putAtt("start_year", DATE[0]);
		timesvar.putAtt("start_month", DATE[1]);
		timesvar.putAtt("end_month", "12");

		stationsvar.putAtt("long_name", "Primary source for timestep");
		stationsvar.putAtt("missing_value", "null");

		// temperature  min et max

		auto max_temp = std::max_element(std::begin(temperatures), std::end(temperatures));
		auto min_temp = std::min_element(std::begin(temperatures), std::end(temperatures));

		// tester le contenu de max et min _temp
		///NcVar nc_var, const string long_name, const string cell_method, T missing_value, const  string  units, const string  axis, double vmin, double vmax, const string coordinates, const string standard_name = "")
		write_attributes<double>(tempsvar, "Dry bulb air temperature at screen height (~2m)", "latitude: longitude: time: point (nearest to reporting hour)", FLTMDI, "degree_Celsius", "T", *max_temp, *min_temp, "latitude longitude elevation", "surface_temperature");
		WriteFlagAttributes<int>(tempsflagsvar, "flags for temperature", INTMDI, "T");

		auto max_dew = std::max_element(std::begin(dewpoints), std::end(dewpoints));
		auto min_dew = std::min_element(std::begin(dewpoints), std::end(dewpoints));
		write_attributes<double>(dewsvar, "Dew point temperature at screen height (~2m)", "latitude: longitude: time: point (nearest to reporting hour)", FLTMDI, "degree_Celsius", "T", *max_dew, *min_dew, "latitude longitude elevation", "dew_point_temperature");
		WriteFlagAttributes<int>(dewsflagsvar, "flags for dewpoint temperature", INTMDI, "T");

		auto max_ws = std::max_element(std::begin(windspeeds), std::end(windspeeds));
		auto min_ws = std::min_element(std::begin(windspeeds), std::end(windspeeds));
		write_attributes<double>(wsvar, "Wind speed at mast height (~10m)", "latitude: longitude: time: point (nearest to reporting hour)", FLTMDI, "meters per second", "T", *max_ws, *min_ws, "latitude longitude elevation", "wind_speed");
		WriteFlagAttributes<int>(wsfvar, "flags for windspeed", INTMDI, "T");

		write_attributes<double>(wdvar, "Wind Direction at mast height (~10m)", "latitude: longitude: time: point (nearest to reporting hour)", FLTMDI, "degree", "T", 360, 0, "latitude longitude elevation", "wind_from_direction");
		WriteFlagAttributes<int>(wdfvar, "flags for wind direction", INTMDI, "T");

		auto max_prs = std::max_element(std::begin(pressure), std::end(pressure));
		auto min_prs = std::min_element(std::begin(pressure), std::end(pressure));
		write_attributes<double>(prvar, "Reported Sea Level Pressure at screen height (~2m)", "latitude: longitude: time: point (nearest to reporting hour)", FLTMDI, "mbar", "T", 360, 0, "latitude longitude elevation", "air_pressure_at_sea_level");
		WriteFlagAttributes<int>(prfvar, "flags for pressure", INTMDI, "T");

		//global attributes
		ncFile.putAtt("station_information", "Where station is a composite the station id refers to the primary source used in the timestep and does apply to all elements");
		boost::format fmter("%1%");
		fmter% boost::gregorian::day_clock().local_day();
		ncFile.putAtt("file_created", fmter.str());
		ncFile.putAtt("station_id", stat->getId());
		ncFile.putAtt("latitude", boost::lexical_cast<std::string>(stat->getLat()));
		ncFile.putAtt("longitude", boost::lexical_cast<std::string>(stat->getLon()));
		ncFile.putAtt("elevation", boost::lexical_cast<std::string>(stat->getElev()));
		ncFile.putAtt("Conventions", " CF - 1.6");
		ncFile.putAtt("date_created", fmter.str());
		ncFile.putAtt("history", "Created by ...");

		cout << " Writing data to netcdf file" << endl;

		double * t_temperatures = new double[HoursBetween];
		double *t_dewpoints = new double[HoursBetween];
		double *t_windspeeds = new double[HoursBetween];
		double *t_pressure = new double[HoursBetween];
		double *t_winddirs = new double[HoursBetween];
		int  *t_TimeStamps = new int[HoursBetween];
		int *t_temperature_flags = new int[HoursBetween];
		int *t_dewpoint_flags = new int[HoursBetween];
		int *t_windspeeds_flags = new int[HoursBetween];
		int *t_winddirs_flags = new int[HoursBetween];
		int *t_pressure_flags = new int[HoursBetween];


		CreateTab<int>(TimeStamps, t_TimeStamps);
		CreateTab<double>(temperatures, t_temperatures);
		CreateTab<int>(temperature_flags, t_temperature_flags);
		CreateTab<double>(dewpoints, t_dewpoints);
		CreateTab<int>(dewpoint_flags, t_dewpoint_flags);
		CreateTab<double>(windspeeds, t_windspeeds);
		CreateTab<int>(windspeeds_flags, t_windspeeds_flags);
		CreateTab<double>(winddirs, t_winddirs);
		CreateTab<int>(winddirs_flags, t_winddirs_flags);
		CreateTab<double>(pressure, t_pressure);
		CreateTab<int>(pressure_flags, t_pressure_flags);


		timesvar.putVar(t_TimeStamps);
		tempsvar.putVar(t_temperatures);
		tempsflagsvar.putVar(t_temperature_flags);
		dewsflagsvar.putVar(t_dewpoints);
		dewsflagsvar.putVar(t_dewpoint_flags);
		wsvar.putVar(t_windspeeds);
		wsfvar.putVar(t_windspeeds_flags);
		wdvar.putVar(t_winddirs);
		wdfvar.putVar(t_winddirs_flags);
		prvar.putVar(t_pressure);
		prfvar.putVar(t_pressure_flags);

		//Free memory
		delete t_TimeStamps;
		delete t_temperatures;
		delete t_temperature_flags;
		delete t_dewpoints;
		delete t_dewpoint_flags;
		delete t_windspeeds;
		delete t_windspeeds_flags;
		delete t_winddirs;
		delete t_winddirs_flags;
		delete t_pressure;
		delete t_pressure_flags;
		//if extra ....

		//zip file
		cout << ncFile.getName() << endl;
		cout << "Done station " << stat->getName() << endl;
		cout << boost::gregorian::day_clock::local_day() - dbg_sttime << endl;
		cout << boost::gregorian::day_clock::local_day() << endl;

	}
	/*
	//	Reads the netcdf file
	//and appends attributes to station object variables to be read passed
	// in as list
	*/
	void read(string filename, station *stat, std::vector<std::string> process_var, vector<string>  opt_var_list, bool read_input_station_id, bool read_qc_flags, bool read_flagged_obs)
	{

		try
		{

			NcFile ncFile(filename.c_str(), NcFile::read, NcFile::nc4classic);

			//Ouverture du fichier netcdf  relié à la station en mode read
			if (ncFile.isNull()) return;

			vector<string> full_var_list;
			copy(process_var.begin(), process_var.end(), std::back_inserter(full_var_list));
			if (opt_var_list.size() != 0)
			{
				copy(opt_var_list.begin(), opt_var_list.end(), std::back_inserter(full_var_list));
			}

			if (read_input_station_id)
			{
				full_var_list.push_back("time");
				full_var_list.push_back("input_station_id");
			}
			else full_var_list.push_back("time");

			for (vector<string>::iterator variable = full_var_list.begin(); variable != full_var_list.end(); ++variable)
			{
				NcVar var = ncFile.getVar(*variable); //Extraire la variable netcdf associée à la variable de process_vars;
				/*map<string, NcVarAtt> atts = var.getAtts();
				for (map<string, NcVarAtt>::iterator it = atts.begin(); it !=atts.end(); it++)
				cout << (*it).first << endl;*/
				string att = getAttribute<string>(var, "long_name");
				//Instanciation  objet MetVar avec le nom de la variable netcdf
				MetVar this_var = MetVar(*variable, att);
				//Ajout des attributs à l'objet Metvar
				//Attribut unit
				try{ this_var.setUnits(getAttribute<string>(var, "units")); }
				catch (NcException &e){  }
				//attribut dtype
				NcType dtype = var.getType();
				this_var.setDtype(dtype.getName());

				// section for non - default netcdf attributes
				if (*variable == "time")
				{
					this_var.setCalendar(getAttribute<string>(var, "calendar"));
					
					stat->setTime_units(getAttribute<string>(var, "units"));
					
				}
				//Attribut missing value (mdi)
				try{ this_var.setMdi(getAttribute<string>(var, "missing_value")); }
				catch (NcException &e)
				{
					if ((*variable) == "temperatures")
						this_var.setMdi("-1e30");
					else if ((*variable) == "input_station_id")
						this_var.setMdi("null");
				}
				//Attribut valid_max
				try{ this_var.setValidMax(getAttribute<string>(var, "valid_max")); }
				catch (NcException &e){}
				//Attribut valid_min
				try{
					this_var.setValidMin(getAttribute<string>(var, "valid_min"));
				}
				catch (NcException ){}
				//Attribut standard_name
				try{
					this_var.setStandard_name(getAttribute<string>(var, "standard_name"));
				}
				catch (NcException ){}
				//Attribut coordinates
				try{
					this_var.setCoordinates(getAttribute<string>(var, "coordinates"));
				}
				catch (NcException ){}
				//Attribut cell_methods
				try{
					this_var.setCellmethods(getAttribute<string>(var, "cell_methods"));
				}
				catch (NcException ){}
				
				try
				{
					this_var.setFlagged_value(getAttribute<string>(var, "flagged_value"));
				}
				catch (NcException& e)
				{
					if (*variable == "temperatures" || *variable == "dewpoints" || *variable == "slp" || *variable == "windspeeds")
						this_var.setFlagged_value ("-2.e30");
					else
						this_var.setFlagged_value("-888");
				}
				//Ajouter la variable méteo à la liste des variables de la station stat
				stat->setMetVar(this_var, *variable);
				
			}
			//read in the qc_flags array
			if (read_qc_flags == true)
			{
				try
				{
					string* qc_flags_values = new string[stat->getTime_data().size()];
					NcVar qc_flags = ncFile.getVar("quality_control_flags");
					/*try
					{ 
						qc_flags.getVar(qc_flags_values); 
					}
					catch (...){}*/
					stat->setQc_flags(qc_flags_values);    // add flags to  the attribute flag of the station
				}
				catch (NcException&  e)
				{
					//cerr << e.what() << endl;
					cout << "no QC flags available" << endl;
				}
			}
			//read in the flagged_obs array
			if (read_flagged_obs == true)
			{
				try
				{
					//Check if variable flagged_value exist in the netCDF file
					NcVar flagged_obs = ncFile.getVar("flagged_value");
				}
				catch (NcException& e)
				{
					cerr << e.what() << endl;
					cout << "no flagged obs available in netcdf file" << endl;
					//if doesn't exist, make an empty array  len(station.time.data),len(var_list)])

					vector< vector<string> > flagged_obs; //matrice [temps][variable]

					vector<string> column;

					//and set empty array to be correct MDI for variable

					for (vector<string>::iterator var = process_var.begin(); var != process_var.end(); ++var)
					{
						MetVar st_var = stat->getMetvar(*var);
						for (int i = 0; i < stat->getTime_data().size(); i++) //Nombre d'observations recueillies.
							column.push_back(st_var.getMdi());
						//Affecter la valeur manquante sur la colonne correspondant à la variable meteo
						flagged_obs.push_back(column);
					}
				}
			}
			//read in reporting statistics - just to carry through
			try
			{
				NcVar reporting_stats = ncFile.getVar("reporting_stats");
				string* reporting_values = new string[stat->getTime_data().size()];
				//reporting_stats.getVar(reporting_values);
				int v = 0;
				for (vector<string>::iterator var = process_var.begin(); var != process_var.end(); ++var, v++)
				{
					MetVar st_var = stat->getMetvar(*var);
					//st_var->setReportingStats(reporting_values[v]);
				}
			}
			catch (NcException& e)
			{
				cout << "no reporting stats available in netcdf file" << endl;
			}
			try
			{
				// Global attribute history
				NcGroupAtt attr = ncFile.getAtt("history");
				string att;
				attr.getValues(att);
				stat->setHistory(att);
			}
			catch (NcException& e)
			{
				stat->setHistory("");
			}
		}
		catch (NcException& e)
		{
			cerr << e.what() << endl;
		}
		
	}

}

