#!/bin/bash

run_ce_solver()
{
	PROGRAMM_NAME="$1"
	LOG=$2
	maxSec=$3
	maxSecPerInstance=$4
	maxNotSolved=$5


	FILES=$(ls random/*.input)

	if [ $6 -eq 1 ]; then
		FILES=$(ls -rS dimacs/*.dimacs)
	fi
	if [ $6 -eq 2 ]; then
		FILES=$(ls -rS snap/*.dimacs)
	fi

	rm -f time.txt

	overallTime=$(date +%s);
	now=$(date +%s);
	elapsed=`expr $now - $overallTime`;
	notSolved=0
	showedHint=0

	for f in $FILES
	do
		if [ $elapsed -le $maxSec -a $notSolved -le $maxNotSolved ]; then
			echo $f >> $LOG
			# start everything in a new process group such that we can kill everything if necessary
			(setsid /usr/bin/time -f "%e" -a -o time.txt $PROGRAMM_NAME< $f 1> prog_out.txt 2>&1) & PID=$!

			# kill processes on exit
			trap "{ kill -$PID 2>/dev/null; }" TERM
			trap "{ kill -9 -$PID 2>/dev/null; }" EXIT

			waited=0
			alive=0
			disown $PID;
			kill -0 $PID 2>/dev/null && alive=1;
			while [ $alive -eq 1 -a $waited -le $maxSecPerInstance ]; do
				sleep 2;
				(( waited += 2 ));
				kill -0 $PID 2>/dev/null || alive=0
			done
			if [ $alive -eq 1 ]; then
				# process still exists, kill it softly, then brutally, if necessary
				kill -TERM -$PID 2>/dev/null; sleep 1; kill -9 -$PID 2>/dev/null;
				finished=0;
				(( notSolved += 1 ));
			else
				finished=1;
			fi

			k=$(grep -ve "^#" prog_out.txt | wc -l)
			recursiveSteps=$(grep -e "#recursive steps:" prog_out.txt | sed -e 's/.*recursive steps: \([0-9]*\).*/\1/' )
			cat prog_out.txt >> $LOG

			rm -f prog_out.txt

# 			time=$(cat time.txt | awk '/system/{print $3}' | sed -e 's/elapsed//');
			time=$(cat time.txt);
			verify="";

			if [ "$finished" -eq "1" ]; then
				solFile=${f/input/solution};
				solFile=${solFile/.dimacs/.solution};
				solNumber=$(cat $solFile);
				if [ -n "$solNumber" ] && [ "$solNumber" -eq "$solNumber" ] 2>/dev/null; then
					if [ "$solNumber" -eq "$k" ]; then
						verify="correct";
					else
						verify=">>INCORRECT<<";
					fi
				fi
			fi

			echo -e $f"\t"$time"s\t"$k"\t"$recursiveSteps"\t"$finished"\t"$verify
			echo "" >> $LOG

			rm -f time.txt

			now=$(date +%s);
			elapsed=`expr $now - $overallTime`;
		else
			if [ $showedHint -eq 0 ]; then
				if [ $notSolved -ge $maxNotSolved ]; then
					echo "$notSolved instances not solved. Script aborted."
				else
					echo "maximal time of $maxSec sec elapsed. Script aborted."
				fi
				showedHint=1;
			fi
		fi
	done
}


PROGRAMM_NAME="../vertex_cover"  	# insert your program here
LOG=log.txt									# specify the name of the log file
maxSec=43200									# overall allowed time for the whole script
maxSecPerInstance=300							# allowed time (in seconds) for one instance
maxNotSolved=10								# no of instances the program is allowed to fail to solve. If reached, then the script is aborted

echo "run random instances $PROGRAMM_NAME (Tab-separated columns: File, Time in seconds, solution size, recursive steps, finished, solution size verified)"
run_ce_solver "$PROGRAMM_NAME" $LOG $maxSec $maxSecPerInstance $maxNotSolved 0

echo "run dimacs instances $PROGRAMM_NAME (Tab-separated columns: File, Time in seconds, solution size, recursive steps, finished, solution size verified)"
run_ce_solver "$PROGRAMM_NAME" $LOG $maxSec $maxSecPerInstance $maxNotSolved 1

echo "run snap instances $PROGRAMM_NAME (Tab-separated columns: File, Time in seconds, solution size, recursive steps, finished, solution size verified)"
run_ce_solver "$PROGRAMM_NAME" $LOG $maxSec $maxSecPerInstance $maxNotSolved 2
