PA3: Implement and Evaluate a User-Space Memory Allocator 		       10/26/17
---------------------------------------------------------------------------------------
README

(1) Measurements
(2) reset.sh
(3) Running the program
---------------------------------------------------------------------------------------
(1) 	The program has built-in performance metrics that measure the time spent
	executing the psumalloc and psufree functions. The measured time is recorded
	in nanoseconds. The data is stored in their respective "measure_malloc.txt" and
	"measure_free.txt" files. Each line contains one measurement. In addition to 
	timings, the number of failed psumallocs was also recorded. This information is
	stored in the "malloc_fail.txt" file. The number of fails was not recorded, but
	for each occurence a line of text is appended to the file. If there is not
	enough space to accomodate the allocation request the string "Lack of memory
	occurred!" is appended to the file. If external fragmentation occurs, the 
	string "External Fragmentation occurred!" would be appended. 
---------------------------------------------------------------------------------------
(2)	For obvious reasons, the files mentioned in (1) will not self-delete after 
	execution. The program is set to append to the files, so without deleting them
	the program will continually add to them. To handle this, a shell script file
	must be run prior to the main program. This shell script will delete the 
	files and recreate them. This ensures that the files are cleared of their
	contents. The shell script will also recreate the files, so the main program
	can execute more efficiently during its first run. To run this shell script, 
	type: ./reset.sh inside your bash terminal.
---------------------------------------------------------------------------------------
(3)     Before running the program, (2) must be closely observed or unforeseen
	consequences could occur. To specify a workload, you open the MakeFile and set
	(line 3) "FILE=" to either "mixed_workload.c" or "small_workload.c". Then to
	set the specific fit algorithm, open the workload file and set the (line 6)
	algo to either "0" for best-fit or "1" for worst-fit. Now you're ready to run
	the program. This can be accomplished by typing: ./prog3 in the bash terminal.
=======================================================================================
AUTHORS

	Jonathon Dahl
	Vibhu C. Patel
	Sanyukta Baluni
---------------------------------------------------------------------------------------
Supporting Authors: 
Ata Fatahi Baarzi, Aman Jain, Diman Tootaghaj, and Bhuvan Urgaonkar
=======================================================================================
BUGS
	None
=======================================================================================

