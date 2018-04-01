#!/bin/bash
#This code adapted from "Elegant Parallel Processes in bash" by
# Kevin L. Sitze
#Usage:   cat split.txt | process.sh n
# 
#Example:   cat seed7.txt | process.sh 10

tdim=$1
LOGFILES=/var/log/fte_log
echo "LOGFILES=${LOGFILES}"
echo "target dimension=$tdim"
declare -a availableHosts=('192.168.0.4' '192.168.0.3' '192.168.0.11'
                           '192.168.1.12' '192.168.0.13' '192.168.0.10'
                           '192.168.0.16')

# setup a named pipe (FIFO)
myfifo=$( mktemp --dry-run )
mkfifo --mode=0700 $myfifo
exec 3<>$myfifo # hook pipe up to descriptor 3 for read/write
rm -f $myfifo # pipe remains intact until closed
maximum=${#availableHosts[@]} # hosts
echo "Available Hosts = ${maximum}"
running=0
echo "one"
while read sfile; do
    while (( running >= maximum )) ; do
        if read -u 3 cpid host; then
            wait $cpid
		  #echo "A cpid=${cpid}"
            #echo "A host= ${host}"
    	       #echo "A1 ${#availableHosts[@]}"
            availableHosts=(${host} ${availableHots[@]})
    	       #echo "A2 ${#availableHosts[@]}"
            (( --running ))
        fi
    done
    host=${availableHosts[0]} 
    #echo "B1 host=${host}"
    #echo "B2 ${#availableHosts[@]}"
    availableHosts=("${availableHosts[@]:1}")
    #echo "B3 ${#availableHosts[@]}"
    #echo "B4 ${availableHosts[0]}"
    ( # child process
	   logfile=${LOGFILES}/$(basename "$sfile").log
	   echo "host=${host} seed file=${sfile} logfile=${logfile}"
        ssh ${USER}@$host "${HOME}/MsPaper/threads/fte ${tdim} ${sfile}" > ${logfile}
	   pid=${BASHPID}
	   #echo "C pid=${pid}"
	   #echo "C host=${host}"
        echo $pid ${host} 1>&3
    ) &
    (( ++running ))
done
wait
exit 0
