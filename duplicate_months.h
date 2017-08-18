#pragma once
#include"station.h"
#include "utils.h"
#include <vector>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include<iostream>

namespace DUPLICATE_MONTHS
{

	void dmc(station stat, std::vector<std::string> variable_list, std::vector<std::string> full_variable_list, std::vector<int> flag_col,
		boost::gregorian::date start,boost::gregorian::date end, std::ofstream &logfile);

}