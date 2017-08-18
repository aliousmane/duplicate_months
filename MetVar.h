#pragma once
#include<string>
#include <vector>
#include <valarray> 
/*
Class for meteorological variable
*/
class MetVar
{
public:
	MetVar();
	MetVar(std::string name, std::string long_name);
	~MetVar();
	std::string toString();
	std::string getName();
	std::string getLong_Name();
	std::string getUnits();
	std::string getDtype();
	void setDtype(std::string dtype);
	void setUnits(std::string units);
	void setCalendar(std::string calendar);
	void setMdi(std::string mdi);
	void setValidMax(std::string valid_max);
	void setValidMin(std::string valid_min);
	void setCoordinates(std::string coordinates);
	void setFdi(std::string fdi);
	void setCellmethods(std::string cell_methods);
	void setStandard_name(std::string standard_name);
	void setReportingStats(std::string reporting_stats);
	void set_MetVar_attributes(std::string standard_name, std::string units, std::string mdi, std::string dtype);
	void setFlagged_obs(std::vector<std::string> flagged_obs);
	void setFlagged_value(std::string flagged_value);
	void setData(std::valarray<std::string> data);
	std::valarray<std::string>getData();
	std::string getMdi();
protected :std::string m_name;
private:
	
	std::string m_long_name;
	std::string m_units;
	std::string m_standard_name;
	std::string m_mdi;
	std::string m_dtype;
	std::string m_calendar;
	std::string m_valid_max;
	std::string m_valid_min;
	std::string m_coordinates;
	std::string m_cell_methods;
	std::string m_fdi;
	std::string m_flagged_value;
	std::string m_reporting_stats;
	std::vector<int> flags;
	std::valarray<std::string> m_data;
	std::vector <std::string> m_flagged_obs;
};