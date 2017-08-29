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

		const int MIN_DATA_REQUIRED = 500;// to create histogram for complete record
		const int MIN_DATA_REQUIRED_YEAR = 100; // to create histogram
		map<int, int > month_ranges;
		month_ranges = UTILS::month_starts_in_pairs(start, end);
		vector<vector<pair<int, int>>> month_ranges_years;
		int iteration = 1;
		vector<pair<int, int>> month;
		int index = 0;
		for (map<int, int>::iterator month_it = month_ranges.begin(); month_it != month_ranges.end(); month_it++)
		{
			if (iteration <= 12)
			{
				month.push_back(make_pair(month_it->first, month_it->second));
				iteration++;
			}
			else
			{
				month_ranges_years.push_back(month);
				index++;
				month.clear();
				month.push_back(make_pair(month_it->first, month_it->second));
				iteration = 2;
			}

		}
		month_ranges_years.push_back(month);
	
		int v = 0;
		for (string variable : variable_list)
		{
			MetVar * st_var = stat->getMetvar(variable);
			valarray<float> filtered_data = apply_filter_flags(st_var);
			float  reporting_accuracy = UTILS::reporting_accuracy(filtered_data);

			//apply flags - for detection only   (filtered_data)
			valarray<float> season_data;
			int thresholds[3];
			for (int season : {0, 1, 2, 3, 4})
			{
				if (season == 0)
				{
					//all year
					season_data = PYTHON_FUNCTION::masked_values<float>(filtered_data, st_var->getFdi());
					thresholds[0] = 30;
					thresholds[1] = 20;
					thresholds[2] = 10;
				}
				else
				{
					thresholds[0] = 20;
					thresholds[1] = 15;
					thresholds[2] = 10;
					
					vector<vector<pair<int, int>>>::reverse_iterator year;
					for (year = month_ranges_years.rbegin(); year != month_ranges_years.rend(); year++)
					{
	
						//churn through months extracting data, accounting for fdi and concatenating together
						if (season == 1) // mars,avril,may
						{
							valarray<float> new_filtered_data= filtered_data[std::slice((*year)[2].first,6,1)];
							new_filtered_data = PYTHON_FUNCTION::masked_values(new_filtered_data, st_var->getFdi());
							//std::copy(std::begin(new_filtered_data), std::end(new_filtered_data), std::end(season_data));
							season_data = season_data + new_filtered_data; //fonction pour concatener 
						}
						else if (season == 2) //june, july, august
						{
							valarray<float> new_filtered_data = filtered_data[std::slice((*year)[5].first, 6, 1)];
							new_filtered_data = PYTHON_FUNCTION::masked_values(new_filtered_data, st_var->getFdi());
							season_data = season_data + new_filtered_data;
							//std::copy(std::begin(new_filtered_data), std::end(new_filtered_data), std::end(season_data));
						}
						else if (season == 3) // september,october,november
						{
							valarray<float> new_filtered_data = filtered_data[std::slice((*year)[8].first, 6, 1)];
							new_filtered_data = PYTHON_FUNCTION::masked_values(new_filtered_data, st_var->getFdi());
							season_data = season_data + new_filtered_data;
						}
						else //december + january,februay
						{
							valarray<float> new_filtered_data = filtered_data[std::slice((*year)[0].first, 4, 1)];
							new_filtered_data = PYTHON_FUNCTION::masked_values(new_filtered_data, st_var->getFdi());
							season_data = season_data + new_filtered_data;
							new_filtered_data = filtered_data[std::slice((*year)[11].first, 2, 1)];
							new_filtered_data = PYTHON_FUNCTION::masked_values(new_filtered_data, st_var->getFdi());
							season_data = season_data + new_filtered_data;
						}
					}
				}
				valarray<float> seven_bins;
				if (season_data.size() > MIN_DATA_REQUIRED )
				{
					valarray<float> bins,bincenters;
					
					if (0 < reporting_accuracy  && reporting_accuracy <= 0.5)  //-1 used as missing value
						UTILS::create_bins(season_data, 0.5, bins, bincenters);
					else
						UTILS::create_bins(season_data, 1, bins, bincenters);

					valarray<float> binEdges(bins);
					valarray<float> hist = PYTHON_FUNCTION::histogram<float>(season_data, binEdges);

					valarray<float> bad_bin(0., hist.size());
					//scan through bin values and identify bad ones
					int e = 0;
					for (float element : hist)
					{
						if (e > 3 && e <= hist.size() - 3) //don't bother with first three or last three bins
						{
							seven_bins = hist[std::slice(e - 3, 7, 1)];
							if (seven_bins[3] == seven_bins.max() && seven_bins[3] != 0) //is local maximum and != zero
							{
								if (seven_bins[3] / seven_bins.sum() >= 0.5 && seven_bins[3] >= thresholds[0])
								{
									//contains >50 % of data and is greater than threshold
									bad_bin[e] = 1;
								}
							}
						}
						e++;
					}
					//having identified possible bad bins, check each year in turn
					vector<vector<pair<int, int>>>::reverse_iterator year;
					for (year = month_ranges_years.rbegin(); year != month_ranges_years.rend(); year++)
					{
						valarray<float> year_data;
						float year_flags;
						if (season == 0)
						{
							year_data= st_var->getData()[std::slice((*year)[0].first, 24, 1)];
							year_data = PYTHON_FUNCTION::masked_values(year_data, st_var->getFdi());
							//year_flags
						}
						if (season == 1) // mars,avril,may
						{
							year_data = st_var->getData()[std::slice((*year)[2].first, 6, 1)];
							year_data = PYTHON_FUNCTION::masked_values(year_data, st_var->getFdi());
							//year_flags
						}
						else if (season == 2) //june, july, august
						{

							year_data = st_var->getData()[std::slice((*year)[5].first, 6, 1)];
							year_data = PYTHON_FUNCTION::masked_values(year_data, st_var->getFdi());
							//year_flags
						}
						else if (season == 3) // september,october,november
						{

							year_data = st_var->getData()[std::slice((*year)[8].first, 6, 1)];
							year_data = PYTHON_FUNCTION::masked_values(year_data, st_var->getFdi());
							//year_flags
						}
						else //december + january,februay
						{

							//year_data = st_var->getData()[std::slice((*year)[2].first, 6, 1)];
							//year_data = PYTHON_FUNCTION::masked_values(year_data, st_var->getFdi());
							//year_flags
						}
					}
				}
			}

		}
	}
	

}