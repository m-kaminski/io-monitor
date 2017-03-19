#!/bin/bash

TEST_LOG_DIR=test_log_`date +%s`
mkdir $TEST_LOG_DIR

FAILURE=""

for dir in `ls` ; do
    if [ -x $dir/test.sh ] ; then
	echo Executing test in $dir
	cd $dir
	./test.sh 2>&1 >../$TEST_LOG_DIR/out_$dir
	if [ $? -eq 0 ] ; then
	    echo Test successful;
	else
	    echo Test failed;
	    FAILURE="One or more tests failed"
	fi
	cd ..
    fi
done


echo $FAILURE
