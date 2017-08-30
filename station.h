#pragma once
#include<string>
#include <vector>
#include <map>
#include <valarray>
#include <boost/numeric/ublas/matrix.hpp>


struct s_time
	{
		std::string units;
		std::vector<int> data;
	};
struct MetVar
{
public:
	MetVar::MetVar(){}

	MetVar::MetVar(std::string name, std::string long_name)
	{
		MetVar::m_name = name;
		MetVar::m_long_name = long_name;
	}
	MetVar::~MetVar()
	{
	}
	std::string getName(){ return m_name; }
	std::string getLong_Name(){ return m_long_name; }
	std::string getUnits(){ return m_units; }
	void setDtype(std::string dtype){ m_dtype = dtype; }
	void setUnits(std::string units){ m_units = units; }
	void setCalendar(std::string calendar){ m_calendar = calendar; }
	void setMdi(std::string mdi){ m_mdi = mdi; }
	void setValidMax(std::string valid_max){ m_valid_max = valid_max; }
	void setValidMin(std::string valid_min){ m_valid_min = valid_min; }
	void setCoordinates(std::string coordinates){ m_coordinates = coordinates; }
	void setFdi(float fdi){ m_fdi = fdi; }
	void setCellmethods(std::string cell_methods){ m_cell_methods = cell_methods; }
	void setStandard_name(std::string standard_name){ m_standard_name = standard_name; }
	void setReportingStats(std::valarray<float> reporting_stats){ m_reporting_stats = reporting_stats; }
	void setFlagged_obs(std::valarray<float> flagged_obs){ m_flagged_obs = flagged_obs; }
	void setFlagged_value(float flagged_value){ m_flagged_value = flagged_value; }
	void setFlags(std::valarray<float> flag){ m_flags = flag; }
	void setFlags(std::valarray<size_t> v_flag,float flag){ m_flags[v_flag] = flag; }
	void setData(std::valarray<float> data){ m_data = data; }
	void setMaskedData(std::valarray<float> data){ m_masked_data = data; }
	std::string getMdi(){ return m_mdi; }
	float getFdi(){ return m_fdi; }
	std::string getDtype(){ return m_dtype; }
	std::string getStandardname(){ return m_standard_name; }
	std::valarray<float> getFlagged_obs(){ return m_flagged_obs; }
	std::valarray<float> getReportingStats(){ return m_reporting_stats; }
	std::valarray<float> getData(){ return m_data; }
	std::valarray<float> getMaskedData(){ return m_masked_data; }
	std::valarray<float> getFlags(){ return m_flags; }
	std::string getValidMax(){return m_valid_max ; }
	std::string getValidMin(){return m_valid_min ; }
	std::string getCoordinates(){ return m_coordinates; }
	std::string getCellmethods(){ return m_cell_methods; }
	std::string getCalendar(){ return m_calendar; }
	std::string toString()
	{
		return "Variable :" + m_name + ", " + m_long_name;
	}
	void MetVar::set_MetVar_attributes(std::string standard_name, std::string units, std::string mdi, std::string dtype)
	{
		m_units = units;
		m_standard_name = standard_name;
		m_dtype = dtype;
		m_mdi = mdi;

	}
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
	float m_fdi;
	float m_flagged_value;
	std::valarray<float> m_reporting_stats;
	std::valarray<float> m_flags;
	std::valarray<float> m_data;
	std::valarray<float> m_masked_data; // donn�es sans valeurs manquantes
	std::valarray <float> m_flagged_obs;
};
class station
{

public:
		station();
		station(std::string id, std::string name, double lat, double lon, double elev, std::string wmo);
		virtual ~station();
		std::string toString();
		std::string getId();
		std::string getName();
		std::string getWmoId();
		void setQc_flags(std::valarray<std::valarray<float>> qc_flags);
		/* Remplir qc_flag correpondant � la variable qui se trouve � la position colonne
		*/
		void setQc_flags(std::valarray<float> qc_flags,std::slice indices,int index);
		std::valarray<std::valarray<float>> getQc_flags();
		void setMetVar(MetVar metvar, std::string var);
		MetVar* getMetvar(std::string var);
		void setTime_units(std::string units);
		void setTime_data(std::vector<int> data);
		std::string getTime_units();
		std::vector<int> getTime_data();
		void setHistory(std::string history);
		std::string getHistory();
		double getLat();
		double getLon();
		double getElev();
		

private:
	std::string m_id;
	std::string m_name;
	std::string m_wmoid;
	double m_lat;
	double m_lon;
	double m_elev;
	std::valarray<std::valarray<float>> m_qc_flags;
	std::map<std::string, MetVar >  m_Met_var;
	s_time m_time;
	std::string m_history;
};
