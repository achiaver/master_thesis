/*
********************************************************************************

			EXTERNAL VARIABLES AND FUNCTIONS

External global variable and function declarations. 
Variables and functions initialized in file "simboltz.c"
 

********************************************************************************
*/

/*
********************************************************************************

			EXTERNAL VARIABLES AND FUNCTIONS

 Declarations of global variables and functions.

********************************************************************************
*/

#include	<stdio.h>	/* library with standard I/O functions  */

/* #include	<sys/ieeefp.h>	 types and constants for exception handling */
/* #include	<floatingpoint.h>    conforming to ANSI/IEEE standard  */

#include	<math.h>	  /* math functions library declarations */
#include	<string.h>	  /* string functions library declarations */
#include        <stdlib.h>
//#include	<malloc.h>	  /* memory allocation library declarations */
#include 	<time.h>          /* CPU timing functions library declarations */
#include 	<sys/time.h>      /* another CPU timing functions library declarations */
#include	"common.h"        /* Declarations of global variables and functions. */ 
#include	"boltzstru.h"	  /* boltzmann machine data structure definitions */

#define		PERCENT_ERROR_ENERGY	1.E-10  /* infinitesimal used for precision handling */
#define		MIN_PERCENT_WEIGHTS	1.E-6   /* infinitesimal used for weight cut off. 
			 Synaptic weights less than w_max times this value are zeroed.     */

#define		before_clustering  		1
#define		clustering_within_memories  	2
#define		long_range_synapses_in_memories 3
#define		connections_among_memories  	4




/*
********************************************************************************
		 		Global Functions
********************************************************************************
*/
extern	double	pow(), ran3(), Energy();
extern 	void	nerror(), gerror(), check_allocation(), print_time(), print_text();

extern	int	*recover_stored_patterns_BM(), *recover_stored_patterns_GSA();

/*
********************************************************************************
				Global Variables 
********************************************************************************
*/

extern FILE 	*finalparamf, *errorf;

extern long	initial_seconds_count, last_seconds_count; 

extern int	n_nodes, n_memory1, n_memory2, n_symbol_centers, 
                output_to_monitor, BoltzMachine, GSA, iseed,
		recovering_from_check_point, num_found_patterns_between_checkpoints;
extern int	num_trial_initial_patterns, inhibitive_adaptive_learning;
extern double	pi, initial_temperature, initial_seek_local_min_temperature,
		alpha_global, alpha_ass_mem, small, learning_factor, **ww, w_max, w_min,
		sigma2, percent_neighbor_loss, cluster_formation_learning_factor,
		parameter_distance_between_centers, percent_total_synapses,
		percent_synapses_between_memories;

neuron_inf	**neuron;
		
#define	 	w(i,j) ( *(ww + (j) + (i)*n_nodes) ) 
			/* indexing of dynamically allocated weight matrix */


