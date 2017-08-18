#pragma once
#include<string>
#include <vector>
#include <map>


class MetVar;
struct s_time
	{
		std::string units;
		std::vector<int> data;
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
		void setQc_flags(std::string * qc_flags);
		void setMetVar(MetVar* metvar, std::string var);
		MetVar* getMetvar(std::string var);
		void setTime_units(std::string units);
		void setTime_data(std::vector<int> data);
		std::string getTime_units();
		std::vector<int> getTime_data();
		void setHistory(std::string history);
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
	std::vector<std::string> m_qc_flags;
	std::map<std::string, MetVar* >  m_Met_var;
	s_time m_time;
	std::string m_history;
};
