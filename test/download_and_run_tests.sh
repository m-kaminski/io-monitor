#!/bin/bash

# start from scratch
rm -rf io-monitor

git clone https://github.com/pauldardeau/io-monitor.git
cd io-monitor

NDEBUG=1 make

cd test

./run_tests.sh > ../../test_log.txt

cd ../..
