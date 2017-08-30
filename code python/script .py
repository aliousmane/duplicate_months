#!/usr/local/sci/bin/python
#
import os
import subprocess


#subprocess.call(["D:\Logiciel\Anaconda3\Scripts\ipython3.exe"])

#os.system(Ipython)
#os.chdir("D://HadISD_v2/project/")



"""
Station Selection

Using the ISD FTP site, process the raw ISD station listings to obtain the set
of stations matching HadISD selection criteria and update the files.  
"""
#run station_selection.py --plots --update_files
#runfile('D:/HadISD_v2/project/mk_netcdf_files.py', wdir='D:/HadISD_v2/project')
"""

Uses the output of the station selection to download the files.  Will need to
update the location of the files in lines 47-48 of this script
"""
#run get_isd_files.py
	
"""
Using the raw ISD files creates netCDF files and outputs in locations specified
in lines 40-42.  Last year in data can be specified with --end keyword
"""
#subprocess.run([" mk_netcdf_files.py "])

"""
Run the internal checks - check file locations in lines 33-35 (37-39 in
neighbour_checks) and also the start and end times.  Individual tests can be
specified with keywords to the internal_checks, and the flags applied with the
--masking keyword for the neighbour checks.
"""
#run internal_checks.py --all
#runfile('D:/HadISD_v2/project/internal_checks.py', wdir='D:/HadISD_v2/project')
#run neighbour_checks.py --masking
runfile('D:/HadISD_v2/project/neighbour_checks.py', wdir='D:/HadISD_v2/project')
"""
Analysis & Plotting
Print summary information, maps of the fail rates and also calculate the
humidity and heat stress indices.
"""
#run qc_summary.py
runfile('D:/HadISD_v2/project/qc_summary.py', wdir='D:/HadISD_v2/project')
#run plot_fail_rate_map.py
runfile('D:/HadISD_v2/project/plot_fail_rate_map.py', wdir='D:/HadISD_v2/project')
#run calculate_humidity_and_indices.py
runfile('D:/HadISD_v2/project/calculate_humidity_and_indices.py', wdir='D:/HadISD_v2/project')