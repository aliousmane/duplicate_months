#include "station.h"

#include<string>
#include<iostream>
#include <algorithm>

using namespace std;

struct MetVar;

	station::station(string id,string name, double lat, double lon, double elev,string wmo)
	{
		m_id = id;
		m_lon = lon;
		m_lat = lat;
		m_elev = elev;
		m_name = name;
		m_wmoid = wmo;
	}
	station::~station()
	{
	}
	string station::toString()
	{
		return "station  :" + m_id + "  "+ m_name+  "+  Longitude: " + to_string(m_lon) + "  Latitude:  " + to_string(m_lat) + "  Elevation  :" + to_string(m_elev);

	}
	double station::getLat(){ return station::m_lat; }
	double station::getLon(){ return station::m_lon; }
	double station::getElev(){ return station::m_elev; }
	string station::getId(){ return station::m_id; }
	string station::getName(){ return station::m_name; }
	string station::getWmoId(){ return station::m_wmoid; }
	void station::setQc_flags(string *qc_flags){ /*station::m_qc_flags = qc_flags;*/ }
	void station::setMetVar(MetVar metvar, string var){ (station::m_Met_var)[var]=metvar; }
	MetVar station::getMetvar(string var)
	{ 
		
		return station::m_Met_var[var]; 
	}
	void station::setTime_units(string units){ m_time.units = units; }
	void station::setTime_data(std::vector<int> data){ copy(data.begin(), data.end(), std::back_inserter(m_time.data)); }
	std::string station::getTime_units(){ return m_time.units; }
	std::vector<int> station::getTime_data(){ return m_time.data; }
	void station::setHistory(string history){ m_history=history; }

	