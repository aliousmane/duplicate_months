#include "frequent_values.h"
#include "utils.h"
#include "station.h"
#include "python_function.h"

using namespace std;
using namespace UTILS;

namespace FREQUENT_VALUES
{
	void  fvc(station* stat, std::vector<std::string> variable_list, std::vector<std::string>  flag_col, boost::gregorian::date start, boost::gregorian::date end, std::ofstream &logfile)
	{
		map<int, int > month_ranges;
		month_ranges = UTILS::month_starts_in_pairs(start, end);
		//month_ranges_years = month_ranges.reshape(-1,12,2)
		int v = 0;
		for (string variable : variable_list)
		{
			MetVar * st_var = stat->getMetvar(variable);
			float  reporting_accuracy = UTILS::reporting_accuracy(apply_filter_flags(st_var));
			//apply flags - for detection only
			valarray<float> filtered_data = apply_filter_flags(st_var);

		}
	}
	

}