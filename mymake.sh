#!/bin/bash

############## Create build-dir
logdir="build-log"

if [ ! -d "$logdir" ]; then
	mkdir $logdir
	echo "Log directory created => $logdir"
fi

############## Get time label
time_label=`date '+%Y%m%d_%H%M%S'`


############## Make & install
make prefix=$HOME/ws/ws_eclipse/dos2unix/build > $logdir/make_$time_label.log
make prefix=$HOME/ws/ws_eclipse/dos2unix/build install > $logdir/install_$time_label.log


