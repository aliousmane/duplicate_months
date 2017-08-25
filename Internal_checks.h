#include<vector>
#include "station.h"
#include<string>

namespace
{
	std::vector<std::string> process_var{ "temperatures", "dewpoints", "windspeeds", "winddirs", "slp" };
	std::vector<std::string> carry_thru_vars;
	
}
namespace INTERNAL_CHECKS

{
	void internal_checks(std::vector<station> &station_info, bool duplicate,bool odd,bool frequent, bool second,std::string *DATE);
}