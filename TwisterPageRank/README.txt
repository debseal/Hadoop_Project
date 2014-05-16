Steps Required to Run Page Rank Application============================================Prerequisites--------------1.) You'll need to have Java 1.6 or above installed in your system 	and its bin directory added to the path, so the javac, java, and jar	commands may be executed directly from the shell.	2.) You'll need to have Twister setup properly along with the desired	message broker, i.e. NaradaBrokering or Apache ActiveMQ. The variable	$TWISTER_HOME refers to the Twister home directory.	3.) You'll need to have password less login setup for the nodes that you	intend to run Twister on. 	4.) You'll need to have some basic knowledge about commands available in	a Bash (or any other) shell.	Steps------1.) Complete sections marked as /* COMPLETE HERE */ in following java files.	src/cgl/imr/samplespagerank/PageRankMapTask.java	src/cgl/imr/samplespagerank/PageRankReduceTask.java	src/cgl/imr/samplespagerank/PageRankCombiner.java	2.) Compile all the classes in the src directory. 	You'll need to add all the jar files in $TWISTER_HOME/lib to your java classpath.	3.) Create a jar file consisting of the compiled classes and copy that into 	$TWISTER_HOME/apps directory	4.) Start message broker if not done already. Also, start or restart Twister at this point.	5.) Input data is given in the en0000-01and02_reset_idx_and_square.am file. It contains	361892 lines excluding the first line. The first line simply lists this number. You'll	need to split this file (again, excluding the first line) into as many parts as your 	intended number of map tasks. Also, each part should list how many data lines are in that	file on top. For example, if you decide to split this into three then you can have two parts	with 120631 data lines and the other with 120630 data lines. Remember to include the number	of data lines as the first line in each file. Please prefix your files as pr_in_ so it's	easy to refer in following steps.	6.) Create a directory called pr inside $TWISTER_HOME/data. If you plan to run on multiple	nodes, then you'll need to do this in each node. Twister has a utlity script to automate	this, please ask the instructor for more information.	7.) Copy the partitions created out of input data into $TWISTER_HOME/data/pr. Again, if you	are running on multiple nodes then use Twister's utility script to copy files across all nodes.	8.)	Create a partition file for the data inside $TWISTER_HOME/data/pr using the create_partition_file.sh	that you find inside $TWISTER_HOME/bin. You can use the following command inside $TWISTER_HOME/bin	directory if you followed the above steps correctly.		./create_partition_file pr pr_in_ pr.out		This will create a file called pr.out inside $TWISTER_HOME/bin directory.	9.) Run page rank program using the run_pagerank.sh script inside bin directory. This script takes the	following arguments in the given order,		num urls -- this is the total number of urls, which is 361892 for the given file.		num map tasks -- desired number of map tasks		num reduce tasks -- desired number of reduece tasks		partition file -- path to the partition file created in step 8.		output file -- path to output results

Pre-Condition:
--------------
Assume you have already copy the Twister-Pagerank-${Release}.jar into "apps" directory.

Generating Data:
----------------
./gen_data.sh [input file prefix][num splits=num maps][num urls]

e.g. ./gen_data.sh data/data_ 8 1600

Distributing Data:
------------------

To distribute the generated data files you can use the twister.sh utility available in $TWISTER_HOM/bin directory as follows.

./twister.sh put [input data directory (local)][destination directory (remote)]
destination directory - relative to data_dir specified in twister.properties

e.g. ./twister.sh put ../samples/pagerank/bin/data/ /pagerank

Here /pagerank is the relative path of the sub directory that is available inside the data_dir of all compute nodes. You can use ./twister.sh mkdir to create any sub directory inside data_dir.

Create Partition File:
----------------------
Irrespective of whether you distributed data using above method or manually you need to create a partition file to run the application. Please run the following script in $TWISTER_HOM/bin directory as follows.

./create_partition_file.sh [common directory][file filter][partition file]

e.g. ./create_partition_file.sh /pagerank data_ pagerank.pf

Run PageRank:
---------------

Once the above steps are successful you can simply run the following shell script to run PageRank appliction.

./run_pagerank.sh [num urls][num map tasks][num reduce tasks][partition file][output file]                    

e.g. ./run_pagerank.sh 1600 8 1 partition.pf pagerank.txt 

During the processing, you will see the difference in each iteration. When the difference is less than a 
predefined threshold,the Marcov chain converge,the computation stops,you can find the final PageRank values 
in the output file.                           




