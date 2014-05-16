/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   MPI PageRank                             								 */
/*   CGL Indiana University                         				 		 */
/*   Author: Hui Li															 */
/*   Email: lihui@indiana.edu											     */
/*   01/13/2011                           									 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "pagerank.h"

int mpi_pagerank(int    	*adjacency_matrix,
				 int       	**am_index,   	/* index array for adjacency matrix */
                 int        numUrls,     	/* num of urls assigned to local machine */
                 int        total_num_urls, /* num of total urls */
                 int 		num_iterations,
                 double      threshold,   	/* predefined threshold value that control the number of iterations */
                 double    	*rank_values_table,    /* double[total_num_urls] */
                 MPI_Comm   comm)        	/* MPI communicator */
{
    int   i, j, rank, index, loop=0, total_numUrls;
    double *local_rank_values_table;
    double *old_rank_values_table;
    extern int _debug;
    double delta;
    double tangling;
    double val;

    MPI_Comm_rank(comm, &rank);

    /* allocat memory and initialize values for local_rank_values_table */
    local_rank_values_table = (double*) calloc(total_num_urls,sizeof(double));
    if (rank == 0){
    	old_rank_values_table = (double*) calloc(total_num_urls,sizeof(double));
    	for (i=0;i<total_num_urls;i++)
    		old_rank_values_table[i] = rank_values_table[i];
    }

    MPI_Allreduce(&numUrls, &total_numUrls, 1, MPI_INT, MPI_SUM, comm);
	if (rank == 0)	
            printf("max_iterations=%d, threshold=%.9f\n",num_iterations,threshold);
    do {
        double cur_time = MPI_Wtime();
        delta = 0.0;
        tangling = 0.0;
        val = 0.0;
        int src_url,target_url;
        int first_index, second_index;
        for (i=0; i<numUrls; i++) {
        	first_index = am_index[i][0]-am_index[0][0];
        	src_url = adjacency_matrix[first_index];

        	for (j=1; j<am_index[i][1]; j++){
				target_url = adjacency_matrix[first_index+j];
				val = rank_values_table[src_url]/(double)(am_index[i][1]-1);
				local_rank_values_table[target_url] += val;
        	}

        	if (am_index[i][1] == 1)
        		tangling += rank_values_table[src_url];
			}

        /*if ((_debug)&&(rank==0)){
        	printf("     rank:%d, total_num_urls:%d, local_rank_values_table\n",rank,total_num_urls);
        	for (i=0;i<total_num_urls;i++){
        		printf("     rank:%d local_rank_values_table[%d]:%f\n",rank,i,local_rank_values_table[i]);
        	}
        }*/

        MPI_Allreduce(local_rank_values_table, rank_values_table, total_num_urls, MPI_DOUBLE, MPI_SUM, comm);
        for (i=0;i<total_num_urls;i++)	local_rank_values_table[i] = 0;

        double sum_tangling;
        MPI_Allreduce(&tangling,&sum_tangling,1,MPI_DOUBLE,MPI_SUM,comm);
	
        /* recaculate the page rank values with tamping factor 0.85 */
        val = sum_tangling/(double)total_num_urls;
        for (i=0; i<total_num_urls; i++) {
		rank_values_table[i] = 0.85*(rank_values_table[i]+val)+0.15*1.0/(double)total_num_urls;
        }

        if (rank == 0){
        	delta = 0.0;
        	double diff = 0.0;
        	for (i=0;i<total_num_urls;i++){
        		diff = old_rank_values_table[i] - rank_values_table[i];
        		delta += diff*diff;
        		old_rank_values_table[i] = rank_values_table[i];
        	}
        }
        MPI_Bcast(&delta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (rank == 0){
        	printf("  ->cur_iteration=%d delta=%.9f\n",loop,delta);
		if (_debug)
        	for (i=0;i<total_num_urls;i++)
        	        		printf("  ->rank_values_table[%d]:%f\n",i,rank_values_table[i]);
        }     
 
        if (_debug) {
            double maxTime;
            cur_time = MPI_Wtime() - cur_time;
            MPI_Reduce(&cur_time, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
            //if (rank == 0) printf("%2d: loop=%d time=%f sec\n",rank,loop,cur_time);
        }
    	} while (delta > threshold && loop++ < num_iterations);
    return 1;
}
