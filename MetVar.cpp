#include<string>
#include<iostream>
#include"MetVar.h"
#include <valarray> 

using namespace std;
MetVar::MetVar(std::string name, std::string long_name)
{
	MetVar::m_name = name;
	MetVar::m_long_name = long_name;
}
MetVar::~MetVar()
{
}
string MetVar::getName(){ return MetVar::m_name; }
string MetVar::getLong_Name(){ return MetVar::m_long_name; }
string MetVar::getUnits(){ return MetVar::m_units; }
void MetVar::setDtype(std::string dtype){ MetVar::m_dtype = dtype; }
void MetVar::setUnits(std::string units){ MetVar::m_units = units; }
void MetVar::setCalendar(std::string calendar){ MetVar::m_calendar = calendar; }
void MetVar::setMdi(std::string mdi){ MetVar::m_mdi = mdi; }
void  MetVar::setValidMax(std::string valid_max){ MetVar::m_valid_max = valid_max; }
void  MetVar::setValidMin(std::string valid_min){ MetVar::m_valid_min = valid_min; }
void  MetVar::setCoordinates(std::string coordinates){ MetVar::m_coordinates = coordinates; }
void  MetVar::setFdi(std::string fdi){ MetVar::m_fdi = fdi; }
void  MetVar::setCellmethods(std::string cell_methods){ MetVar::m_cell_methods = cell_methods; }
void MetVar::setStandard_name(std::string standard_name){ MetVar::m_standard_name = standard_name; }
void MetVar::setReportingStats(std::string reporting_stats){ MetVar::m_reporting_stats = reporting_stats; }
string MetVar::getMdi(){ return m_mdi; }
string MetVar::getDtype(){ return MetVar::m_dtype; }
void MetVar::setFlagged_obs(std::vector<std::string> flagged_obs){ MetVar::m_flagged_obs = flagged_obs; }
void MetVar::setFlagged_value(string flagged_value){ MetVar::m_flagged_value = flagged_value; }
void MetVar::setData(valarray<string> data){ MetVar::m_data = data; }
valarray<string> MetVar::getData(){ return MetVar::m_data; }
string MetVar::toString()
{
	return "Variable :" + MetVar::m_name + ", " + MetVar::m_long_name;
}
void MetVar::set_MetVar_attributes(std::string standard_name, std::string units, std::string mdi, std::string dtype)
{
	m_units = units;
	m_standard_name = standard_name;
	m_dtype = dtype;
	m_mdi = mdi;

}