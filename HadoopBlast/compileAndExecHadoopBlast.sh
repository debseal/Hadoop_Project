#!/bin/bash

# clean existing compiled class
echo "Clean built java class and jar"
ant clean

# compile your code and shows errors if any
echo "Compiling source code with ant"
ant

if [ -f build/blast-hadoop.jar ]
then
    echo "Source code compiled!"
else
    echo "There may be errors in your source code, please check the debug message."
    exit 255
fi

cd /root/software/hadoop-1.1.2/
. ./MultiNodesOneClickStartUp.sh /root/software/jdk1.6.0_33/ nodes

sleep 30

cd /root/MoocHomeworks/HadoopBlast/resources/
# wget http://salsahpc.indiana.edu/tutorials/apps/BlastProgramAndDB.tar.gz
cd /root/MoocHomeworks/HadoopBlast/
# run Hadoop Blast

#hadoop dfs -mkdir input
hadoop dfs -put resources/blast_input HDFS_blast_input
hadoop dfs -copyFromLocal resources/BlastProgramAndDB.tar.gz BlastProgramAndDB.tar.gz
hadoop jar /root/MoocHomeworks/HadoopBlast/build/blast-hadoop.jar BlastProgramAndDB.tar.gz bin/blastx /tmp/hadoop-test/ db nr HDFS_blast_input HDFS_blast_output '-query #_INPUTFILE_# -outfmt 6 -seg no -out #_OUTPUTFILE_#'

# capture the standard output
rm -rf output
mkdir output
hadoop dfs -get HDFS_blast_output output

echo "HadoopBlast Finished execution, see output in output/."
