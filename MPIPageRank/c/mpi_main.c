/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   MPI PageRank                    					*/
/*   CGL Indiana University             		 		*/
/*   Author: Hui Li							*/
/*   Email: lihui@indiana.edu						*/
/*   01/13/2011                        					*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     /* strtok() */
#include <sys/types.h>  /* open() */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>     /* getopt() */
#include <mpi.h>
	
#include "pagerank.h"
int      _debug;

static void usage(char *argv0, double threshold) {
    char *help =
        "Usage: %s -i filename -n num_iterations\n"
        "       -i filename    : adjacency matrix input file\n"
        "       -n num_iterations: number of iterations\n"
        "       -t threshold   : threshold value (default %.4f)\n"
        "       -o             : output timing results (default yes)\n"
        "       -d             : enable debug mode\n"
		"       -h             : print help information\n";
    fprintf(stderr, help, argv0, threshold);
}

int main(int argc, char **argv) {
           int     opt;
    extern char   *optarg;
    extern int     optind;
           int     i, j;
           int     is_output_timing, is_print_usage;
           int     num_iterations;

           int     numUrls, totalNumUrls;
           char   *filename;
           int    **am_index;  //int[numUrls][2]  am_index[i][0]refers to second index for am, am_index[i][1] refers to length of target urls list
           int 	  *adjacency_matrix;        	/* [numTotalWebPages] */
           double  *rank_values_table;           /* [numUrls] */
           double   threshold;
           double  timing, io_timing, computing_timing;

           int        rank, nproc, mpi_namelen;
           char       mpi_name[MPI_MAX_PROCESSOR_NAME];
           MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Get_processor_name(mpi_name,&mpi_namelen);

    /* some default values */
    _debug           = 1;
    threshold        = 0.001;
    numUrls      	 = 0;
    is_output_timing = 1;
    is_print_usage   = 0;
    num_iterations   = 1;
    filename         = NULL;
    
    while ( (opt=getopt(argc,argv,"i:n:t:o:d:h"))!= EOF) {
        switch (opt) {
            case 'i': filename=optarg;
                      break;
            case 't': threshold=atof(optarg);
                      break;
            case 'n': num_iterations = atoi(optarg);
                      break;
            case 'o': is_output_timing = 1;
                      break;
            case 'd': _debug = 1;
                      break;
            case 'h': is_print_usage = 1;
                      break;
            default: is_print_usage = 1;
                      break;
        }
    }

    if (filename == 0 || is_print_usage == 1) {
        if (rank == 0) usage(argv[0], threshold);
        MPI_Finalize();
        exit(1);
    }

    if (_debug) printf("Proc:%d of %d running on %s\n", rank, nproc, mpi_name);

    MPI_Barrier(MPI_COMM_WORLD);
    io_timing = MPI_Wtime();

    /* read local adjacency matrix from file ------------------------------------------*/
    mpi_read(filename, &numUrls, &am_index, &adjacency_matrix,MPI_COMM_WORLD);
  
    /* print the target links of first 3 web page in adjacency matrix */
    
    /*if (_debug) { 
        int num = (numUrls < 3) ? numUrls : 3;
        int index;
        for (i=0; i<num; i++) {
		index = am_index[i][0]-am_index[0][0];
		printf("Proc:%d id:%d len:%d",rank,i,am_index[i][1]);
		for (j=0;j<am_index[i][1];j++)
			printf(" %d",adjacency_matrix[index+j]);
		printf("\n");
        }//for
    }*/

    timing            = MPI_Wtime();
    io_timing         = timing - io_timing;
    computing_timing = timing;

    MPI_Allreduce(&numUrls, &totalNumUrls, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    /* global page rank value table */
    rank_values_table = (double*) malloc(totalNumUrls * sizeof(double));
    assert(rank_values_table != NULL);

    /* assign the initial rank value for each web page */
    if (rank == 0) {
	double initial_rank = 1.0/(double)totalNumUrls;
    for (i=0; i<totalNumUrls; i++)
        rank_values_table[i] = initial_rank;
    }
    /* broad cast the intial rank values to all other compute nodes */
    MPI_Bcast(rank_values_table, totalNumUrls, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
    /* start the core computation of MPI PageRank */
    mpi_pagerank(adjacency_matrix, am_index, numUrls, totalNumUrls, num_iterations, threshold, rank_values_table, MPI_COMM_WORLD);

    free(adjacency_matrix);
    free((*am_index));
    free(am_index);

    timing            = MPI_Wtime();
    computing_timing = timing - computing_timing;

    
    mpi_write(filename, totalNumUrls, rank_values_table, MPI_COMM_WORLD);

    free(rank_values_table);

    if (is_output_timing) {
        double max_io_timing, max_computing_timing;
        io_timing += MPI_Wtime() - timing;
        /* get the max timing measured among all processes */
        MPI_Reduce(&io_timing, &max_io_timing, 1, MPI_DOUBLE,
                   MPI_MAX, 0, MPI_COMM_WORLD);
        MPI_Reduce(&computing_timing, &max_computing_timing, 1, MPI_DOUBLE,
                   MPI_MAX, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            printf("\n  **** MPI PageRank ****\n");
            printf("Num of processes   = %d\n", nproc);
            printf("Input file         = %s\n", filename);
            printf("totalNumUrls       = %d\n", totalNumUrls);
            printf("num_itertaions     = %d\n", num_iterations);
            printf("threshold          = %.9f\n", threshold);
            printf("I/O time           = %10.4f sec\n", max_io_timing);
            printf("Computation timing = %10.4f sec\n", max_computing_timing);
        }
    }

    MPI_Finalize();
    return(0);
}
