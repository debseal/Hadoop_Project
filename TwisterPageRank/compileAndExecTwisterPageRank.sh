#!/bin/bash

rm /root/software/twister-0.9/apps/*

# clean existing compiled class
echo "Clean built java class and jar"
ant clean

# compile your code and shows errors if any
echo "Compiling source code with ant"
ant

if [ -f /root/MoocHomeworks/TwisterPageRank/dist/lib/cglTwisterPageRankMooc.jar ]
then
    echo "Source code compiled!"
else
    echo "There may be errors in your source code, please check the debug message."
    exit 255
fi

mkdir -p /root/software/twister-0.9/apps
echo "Copy dist/lib/cglHBaseMooc.jar file to hadoop lib under /root/software/hadoop-1.1.2/lib/"
cp /root/MoocHomeworks/TwisterPageRank/dist/lib/cglTwisterPageRankMooc.jar /root/software/twister-0.9/apps
chmod 777 /root/software/twister-0.9/apps/cglTwisterPageRankMooc.jar

if [ -f /root/software/twister-0.9/apps/cglTwisterPageRankMooc.jar ]
then
    echo "File copied!"
else
    echo "There may be errors when copying file, please check if directory /root/software/twister-0.9/apps exists."
    exit 254
fi

# start activeMQ
rm /root/software/apache-activemq-5.4.2/data -rf
cd /root/software/apache-activemq-5.4.2/bin
./activemq start

sleep 10

# configure twister
echo "*****************************"
cd $TWISTER_HOME/bin
./TwisterPowerMakeUp.sh
./start_twister.sh

sleep 5

# copy data to pr data directory
rm -rf $TWISTER_HOME/data/pr
mkdir -p $TWISTER_HOME/data/pr
cp /root/MoocHomeworks/TwisterPageRank/resource/splitdataInto2parts/* $TWISTER_HOME/data/pr

# make partition file
./create_partition_file.sh pr data pr.out

# run the program
cd /root/MoocHomeworks/TwisterPageRank/bin/
mkdir -p /root/MoocHomeworks/TwisterPageRank/output
./run_pagerank.sh 361892 2 1 $TWISTER_HOME/bin/pr.out project3.txt 
cp project3.txt /root/MoocHomeworks/TwisterPageRank/output

# stop all 
cd /root/software/apache-activemq-5.4.2/bin
./activemq stop

cd $TWISTER_HOME/bin
./stop_twister.sh

cd /root/MoocHomeworks/TwisterPageRank/bin/

line=localhost
echo $line
for pp in `ssh $line pgrep java`;do
   echo $pp
   ssh $line kill -9 $pp
done

sleep 3


# capture the standard output
echo "pagerank Finished execution, see output in output/project3.txt."

echo ""
