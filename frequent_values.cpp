#include "frequent_values.h"
#include "utils.h"
#include "station.h"

using namespace std;


namespace FREQUENT_VALUES
{
	void  fvc(station stat, std::vector<std::string> variable_list, std::vector<std::string>  flag_col, boost::gregorian::date start, boost::gregorian::date end, std::ofstream &logfile)
	{
		map<int, int > month_ranges;
		month_ranges = UTILS::month_starts_in_pairs(start, end);

	}
}