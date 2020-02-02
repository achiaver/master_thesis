
#include	<stdio.h>	/* library with standard I/O functions */

/* #include	<sys/ieeefp.h>	 types and constants for exception handling */
/* #include	<floatingpoint.h>    conforming to ANSI/IEEE standard  */

#include	<math.h>	  /* math functions library declarations */
#include	<string.h>	  /* string functions library declarations */
#include	<malloc.h>	  /* memory allocation library declarations */
#include 	<time.h>          /* CPU timing functions library declarations */
#include 	<sys/time.h>      /* another CPU timing functions library declarations */
#include	"common.h"	  /* general definitions  */
#include	"boltzstru.h"	  /* boltzmann data structure definitions */



FILE 	*errorf;

void	check_allocation();

neuron_inf	**neuron;

int	n_nodes = 5;

main()
{

	double *insert_second_neuron_in_neighbor_list_of_first(),
		calculate_clustering_coefficient();

	int	i;
	double	*w, clustering_coefficient;

	errorf = fopen("test_clustering", "w");

	neuron = (neuron_inf **) malloc ( n_nodes*sizeof(neuron_inf *) );
	check_allocation( (void *)neuron, "main()", "neuron");

	for (i = 0; i < n_nodes; i++)
	{
		neuron[i] = (neuron_inf *) malloc ( sizeof(neuron_inf) );
		check_allocation( (void *)neuron[i], "main()", "neuron[i]");
		neuron[i]->num_neighbors = 0;
			/* equivalent to (*neuron[i]).num_neighbors = 0; */
		neuron[i]->list_neighs = NO_NEIGHBOR;
	}

	w = insert_second_neuron_in_neighbor_list_of_first( 0, 1, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 1, 0, 0.0);

	w = insert_second_neuron_in_neighbor_list_of_first( 0, 2, 0.0);	
	w = insert_second_neuron_in_neighbor_list_of_first( 2, 0, 0.0);

	w = insert_second_neuron_in_neighbor_list_of_first( 0, 3, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 3, 0, 0.0);

/*	w = insert_second_neuron_in_neighbor_list_of_first( 0, 5, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 5, 0, 0.0);
*/
	w = insert_second_neuron_in_neighbor_list_of_first( 1, 2, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 2, 1, 0.0);

/*	w = insert_second_neuron_in_neighbor_list_of_first( 1, 3, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 3, 1, 0.0);
*/
	w = insert_second_neuron_in_neighbor_list_of_first( 1, 4, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 4, 1, 0.0);

/*	w = insert_second_neuron_in_neighbor_list_of_first( 1, 5, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 5, 1, 0.0);

	w = insert_second_neuron_in_neighbor_list_of_first( 2, 5, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 5, 2, 0.0);

	w = insert_second_neuron_in_neighbor_list_of_first( 2, 3, 0.0);
	w = insert_second_neuron_in_neighbor_list_of_first( 3, 2, 0.0);
*/
	clustering_coefficient = calculate_clustering_coefficient();

	printf ("clustering coefficient = %f\n", clustering_coefficient );

	fclose(errorf);


}  /*  end main  */

/*
********************************************************************************


called by: connectivity_distribution()
arguments:
functions called:
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
double
calculate_clustering_coefficient()
{
	neighbor_inf 	*np, *np2, *np3;
	double	sum_clust_coeff;
	int i, j, k, num_neighs, possib_conn_neighs, nodes_more_than_one_neigh,
            common_neighbors;

	sum_clust_coeff = 0.0;

	nodes_more_than_one_neigh = 0;

	for (i = 0; i < n_nodes; i++)
	{
	  num_neighs = neuron[i]->num_neighbors;

	  if (num_neighs > 1)
	  {
            common_neighbors = 0.0;

	    possib_conn_neighs = num_neighs * (num_neighs - 1) / 2;

	    for (np = neuron[i]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)
	    {
	      j = np->neigh_ID;

	      for (np2 = neuron[j]->list_neighs; np2 != NO_NEIGHBOR; np2 = np2->next_neigh)
	      {
		k = np2->neigh_ID;

		if (k != i)
		{
		  for (np3 = neuron[k]->list_neighs; np3 != NO_NEIGHBOR; np3 = np3->next_neigh)
		    if (np3->neigh_ID == i) common_neighbors = common_neighbors + 1.0;
		}
	      }
	    }

	    nodes_more_than_one_neigh = nodes_more_than_one_neigh + 1;
          
            sum_clust_coeff = sum_clust_coeff + 0.5 * common_neighbors / possib_conn_neighs;

	  }  /* if node has more than 1 neigh  */
	}  /*  for all nodes  */

	if (nodes_more_than_one_neigh > 1)
		return ( sum_clust_coeff / nodes_more_than_one_neigh );
/*	else
		gerror ("No nodes with more than one neighbor\ 
                         Clustering coefficient undefined In 
                         calculate_clustering_coefficient(), (boltz_functs.c).");
*/	

} /*   end calculate_clustering_coefficient()    */




/*
********************************************************************************
Inserts neighbor i in neuron i's list of neighbors. Inserts always at the head of 
the list. The list is doubly linked because neighbors in any position may 
be removed at distinct times.

called by:
	initialize_boltzmann_machine_state()
	reinforce_connections_to_affected_neuron() <void> (in boltz_functs.c)
	distribute_synapses_in_memory_by_gaussian_distribution() <void> (in boltz_functs.c)
	distribute_synapses_between_clusters_in_memory() <void> (in boltz_functs.c)
	distribute_synapses_between_memories() <void> (in boltz_functs.c)

arguments:
	i <int>: identity of neuron in whose list neuron j will be inserted.
	j <int>: identity of neuron to be inserted in list of neuron i.
	weight <double>: value of weight of synapse (edge) from j to i.
functions called:  check_allocation()
external variables:
local variables:
	neigh_pt <neighbor_inf *>: pointer to structure containing information
		regarding neuron j to be inserted in list of neighbors.
return value: pointer to memory position that stores weight value of synapse 
		between neurons i and j.

********************************************************************************
*/
double *
insert_second_neuron_in_neighbor_list_of_first(i, j, weight)
int	i, j;
double	weight;
{
	neighbor_inf 	*neigh_pt;	

	neigh_pt = (neighbor_inf *)malloc(sizeof(neighbor_inf));
 	check_allocation((void *)neigh_pt, 
              "insert_second_neuron_in_neighbor_list_of_first()", "neigh_pt");

	neigh_pt->neigh_ID = j;
	neigh_pt->wij = weight;
	neigh_pt->next_neigh = neuron[i]->list_neighs;
	neigh_pt->prev_neigh = NO_NEIGHBOR;

	if (neuron[i]->list_neighs != NO_NEIGHBOR) neuron[i]->list_neighs->prev_neigh = neigh_pt;

	neuron[i]->list_neighs = neigh_pt;
	neuron[i]->num_neighbors = neuron[i]->num_neighbors + 1;

	return ( &(neigh_pt->wij) );

}   /* 	end insert_second_neuron_in_neighbor_list_of_first();  */




/*
********************************************************************************
Memory allocation error handler. Checks if malloc returned a NULL pointer.

called by:
arguments:
	pt <void *>: pointer to allocated memory block that is to be checked.
	in_funct <char *>: string that is the function's name.
	in_var <char *>: string that is the variable's name
functions called:
		standard I/O functions
global variables:
	errorf <pointer>: pointer to file structure for file "simboltz_errors"
			  where error messages are placed.

local variables:
I/O conditions: The pointer to be checked must be passed with type void.
		For this, one should use a cast in the calling function as 
		follows. To pass the pointer struct_pt do
		check_allocation ((void *)struct_pt, "function_name", 
				"variable_name");
********************************************************************************
*/
void
check_allocation (pt, in_funct, in_var)
void	*pt;
char	in_funct[];
char	in_var[];
{
  if (pt == NULL)
  {
    fprintf(errorf,"\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    fprintf(errorf, "\n Run-time memory allocation error...\n");
    fprintf(errorf, "In function: %s, variable: %s\n", in_funct, in_var);
    fprintf(errorf, "...now exiting to system...\n\n");
    fprintf(errorf, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n");
    exit(0);
  }

}   /*  check_allocation()   */

