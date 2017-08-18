#pragma once
#include<vector>
#include "station.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include<map>


namespace UTILS
{
	inline std::vector<int> month_starts(boost::gregorian::date start, boost::gregorian::date end);
	inline std::map<int, int> month_starts_in_pairs(boost::gregorian::date start, boost::gregorian::date end);
	void create_fulltimes(station* stat, std::vector<std::string> var_list, boost::gregorian::date start,
		boost::gregorian::date end, std::vector<std::string> opt_var_list, bool do_input_station_id = true,
		bool do_qc_flags = true, bool do_flagged_obs = true);
}