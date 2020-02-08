/*
********************************************************************************
     Code for Simulating Neuroses with a Boltzmann Machine (BM) and 
                   Generalized Simulated Annealing (GSA)

     This program implements the algorithm that simulates Neuroses
     modelled by a Bolztmann Machine. 

                      Version 3.0

     Programmed by: Raul Donangelo and Roseli S. Wedemann
     Started on:    March 23, 2004
     
     
Compiling command: gcc -g simboltz.c boltz_functs.c random.c -o simboltz -lm
	in Linux ./ccsimboltz (this shell script has compiling command)

program name: simboltz


running conditions: initialize data in files inboltz.dat

SPECIAL CARES: 

IMPORTANT OBSERVATION: All coordinates of vectors in this program are ordered
		so that the x-coordinate corresponds to index 2,
		the y-coordinate corresponds to index 1 and 
		the z-coordinate corresponds to index 0. For example, for
	 	vector v, v[2] = v_x, v[1] = v_y and v[0] = v_z.

Memory of nodes [0, n_memory1 - 1] represents (stores) sensorial memory traces,
i.e. representations of sensorial input to the brain. Some of these sensorial 
representations stores and represents repressed (unconscious) memory
traces, i.e. those that can't be spoken of. Memory of nodes 
[n_memory2, n_nodes-1] stores symbolic representations of traces in the
first memory, i.e., symbolic memory.

Neuron states can take values in {0,1}.

functions called or just defined in this file:
    defined in this file:
	initialize_boltzmann_machine_state() <void>:
	recover_neurotic_states_as_associative_memory () <void>:
	simulate_psychoanalytical_working_through() <void>:
	input_data_from_file() <void>:
	recover_stored_patterns_BM () <int>:
	recover_stored_patterns_GSA() <int>:
	print_initial_parameters() <void>:
	print_weight_matrix() <void>:
	nerror(), gerror(), check_allocation(), print_text() <void>:
	log_2() <double>:
	Energy() <double>: total energy of the binary neural network.

    defined in boltz_functs.c:
	find_network_stored_patterns() <extern void>:
	find_network_stored_patterns_use_memory() <extern void>:
	convert_bitstring_into_integer() <extern double>:
        generate_new_state() <extern int>:
	reinforce_connections_to_affected_neuron() <extern bool>:

    defined in random.c: ran3() <double>:

	
simulator global variables read from files:
	n_nodes <int>: number of nodes in Boltzmann Machine.
	n_memory1 <int>: number of nodes in memory No. 1 (Sensorial)
	n_memory2 <int>: number of nodes in memory No. 2 (Symbolic)
	initial_temperature <double>: initial temperature for finding 
		global minimum with simulated annealing.
	initial_seek_local_min_temperature <double>: initial temperature 
		for seeking local minima in net configuration.
	termination_temperature <double>: termination temperature for simulated annealing.
	alpha_global <double>: temperature decrease parameter for simulated 
				annealing with global minimum search.
	alpha_ass_mem <double>: temperature decrease parameter for 
				loose simulated annealing (associative memory).
	n_flop_tries_per_node <int>: parameter for determining no. iterations 
				per temperature in simulated annealing.
	num_trial_initial_patterns <int>: number of times new random patterns are
		tried for finding new minima before giving up, since a previous pattern
                was found.
	small <double>: parameter for initializing intercluster synapses.
	temp_intervals <int>: number of temperature intervals for network 
				associativity testing.
	learning_factor <double>: learning parameter for network reconfiguration.
	num_reinforcement_iterations <int>: number of iterations for reinforcement learning.
	inhibitive_adaptive_learning <int>: use 0 / 1 (enter: 0) 
			or -1 / +1 (enter: 1) for adaptive learning
	w_max <double>: maximum_value_for_synaptic_weights
	dimension_cluster1 <double> : spatial dimension cluster 1 (conscious declarative memory)
	dimension_cluster2 <double> : spatial dimension cluster 2 (unconscious sensorial implicit memory)
	sigma2 <double> : square of width of gaussian distribution of reach of synapses for 
			  on-center/off-surround mechanism in memory topology configuration.
	GSA <int> : Indicates if memory should be modeled 
                   by Generalized Simulated Annealing (GSA) or Boltzmann Machine (BM). 
                   GSA = 1 if GSA and 0 if BM.
        q_A <double> : q parameter for state acceptance probability in Generalized Simulated Annealing 
        q_V <double> : q parameter for state visiting probability in Generalized Simulated Annealing	


simulator global variables:
	delta_temperature <double>: Value of temperature increment for memory 
	      association in recover_neurotic_states_as_associative_memory() and
	      simulate_psychoanalytical_working_through().
	neuron <neighbor_inf>: array of neurons, contains information about respective
                               neuron. Characteristic list_neighs of element i points to 
                               a list of neuron i's neighbors.
	w(i,j) <double>: pointer to weight value of synapse that connects neuron j to neuron i.
        ww <* double>: pointer to element w(0,0).
	iseed <int>: seed for pseudo-random number generator (ran3() in random.c).
		     Actually a parameter that indicates initialization. Should
		     be a negative integer.

****************************************************************************************
*/


#include	<stdio.h>	/* library with standard I/O functions */
#include	<stdlib.h>	/* library with standard functions, exit(), etc... */

/* #include	<system.h>	 library with standard system call functions */
/* #include	<sys/ieeefp.h>	 types and constants for exception handling */
/* #include	<floatingpoint.h>    conforming to ANSI/IEEE standard  */

#include	<math.h>	  /* math functions library declarations */
#include	<string.h>	  /* string functions library declarations */
//#include        "/usr/include/sys/malloc.h"
//#include	<malloc.h>	  /* memory allocation library declarations */
#include 	<time.h>          /* CPU timing functions library declarations */
#include 	<sys/time.h>      /* another CPU timing functions library declarations */
#include	"common.h"	  /* general definitions  */
#include	"boltzstru.h"	  /* boltzmann data structure definitions */

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


extern	double	pow(), ran3();

void	nerror(), gerror(), check_allocation(), print_time(), print_text();

int	*recover_stored_patterns_BM(), *recover_stored_patterns_GSA();

double	log_2(), Energy();



/*
********************************************************************************
			Global Variables
********************************************************************************
*/


FILE 	*finalparamf, *errorf, *weightf;

long	initial_seconds_count, last_seconds_count; 

int	n_nodes, n_memory1, n_memory2, n_symbol_centers, 
        output_to_monitor, store_found_patterns_memory, BoltzMachine, GSA, iseed = -17841,
	recovering_from_check_point, num_found_patterns_between_checkpoints;
int	n_flop_tries_per_node, num_trial_initial_patterns, temp_intervals,
	num_reinforcement_iterations, inhibitive_adaptive_learning;
int	*input_pattern;
double	pi, initial_temperature, initial_seek_local_min_temperature, 
	termination_temperature, delta_temperature, alpha_global,
	alpha_ass_mem, small, learning_factor,
	dimension_cluster1, dimension_cluster2, sigma2, percent_neighbor_loss,
	cluster_formation_learning_factor, parameter_for_network_storage_capacity,
	parameter_distance_between_centers, percent_total_synapses,
	percent_synapses_between_memories, q_A, q_V;
double	**ww, w_max, w_min, infinite_energy;

neuron_inf	**neuron;


#define	 w(i,j) ( *(ww + (j) + (i)*n_nodes) ) 
		/* indexing of dynamically allocated weight matrix */


/*	Main Boltzmann Machine Neuroses Simulator Program (function)	*/

int main()
{
	FILE		*patternf, *patternf_GSA, *configurationf;

	TIMEVAL		*program_run_time;
	TIMEZONE	*t_zone;

	void	initialize_boltzmann_machine_state(),
	        recover_neurotic_states_as_associative_memory(),
	        simulate_psychoanalytical_working_through(),
		finish_here();
	        
	
	errorf = fopen("simboltz_errors", "w");
	patternf = fopen("init_stored_patterns", "w+");
	patternf_GSA = fopen("init_stored_patterns_GSA", "w+");

	weightf = fopen("weights", "w");
	finalparamf = fopen("final_parameters", "w");
	configurationf = fopen ("neurotic_memory_configuration", "w");


        program_run_time = (TIMEVAL *) malloc(sizeof(TIMEVAL));
        check_allocation( (void *)program_run_time, "main()", "program_run_time");

        t_zone = (TIMEZONE *) malloc(sizeof(TIMEZONE));
        check_allocation( (void *)t_zone, "main()", "t_zone");

	gettimeofday(program_run_time, t_zone);
	initial_seconds_count = last_seconds_count = program_run_time->tv_sec;

	free(program_run_time);
	free(t_zone);

	if(output_to_monitor)
	  printf("\n\t Initial Program Time \n");

	fprintf(finalparamf, "\n\t Initial Program Time \n");
	print_time();

	initialize_boltzmann_machine_state(patternf, patternf_GSA);

	finish_here("Finish after initialize_BM_state.", patternf, patternf_GSA);
	
	recover_neurotic_states_as_associative_memory(configurationf);

	fclose(configurationf);

	simulate_psychoanalytical_working_through(patternf);


	fclose(patternf);
	fclose(patternf_GSA);	
	fclose(weightf);
	fclose(errorf);

	if(output_to_monitor)
	  printf("\n\t Final Program Time \n");
	fprintf(finalparamf, "\n\t Final Program Time \n");
	print_time();

	fclose(finalparamf);

}   /* end main() */



/*
********************************************************************************
Reads input variables from file inboltz.dat.
Initializes connection weights for Boltzmann Machine. 
Determines energy minima for initial network topology.
Uniform random distribution. Should weights be symetric?


called by: main()
functions called: input_data_from_file()
	allocate_synapses_and_weights_in_memories() <void> (in boltz_functs.c) 
	find_network_stored_patterns() <void> (in boltz_functs.c)
	find_network_stored_patterns_use_memory() <void> (in boltz_functs.c)
	check_allocation(), print_weight_matrix() <void>
	distribute_neurons_spatially_uniformly_in_memory() <void> (in boltz_functs.c)
	output_neighbor_lists() <void>
	connectivity_distribution() <void> (in boltz_functs.c)
global variables:
	double	**ww, w_max;
	int	n_nodes, n_memory1, iseed = -17841;
	neuron_inf	neuron[];
local variables:
	long	i, j; 	
I/O conditions: 
********************************************************************************
*/
void
initialize_boltzmann_machine_state(patternf, patternf_GSA)
FILE		*patternf, *patternf_GSA;
{
	extern	void	find_network_stored_patterns(), find_network_stored_patterns_use_memory(), 
			distribute_neurons_spatially_uniformly_in_memory(),
			connectivity_distribution(),
			allocate_synapses_and_weights_in_memories();

	void	input_data_from_file(), print_weight_matrix(), test_minimum_energy(),		
		output_neighbor_lists();

	FILE	*pos_sens_mem, *pos_decl_mem, *freq_visited_pattern_BM, *freq_visited_pattern_GSA;

	double	*insert_second_neuron_in_neighbor_list_of_first(); 
	long	i, j;
	double	weight; 
	

	input_data_from_file();
	
        delta_temperature = (initial_temperature - termination_temperature) / temp_intervals;

	infinite_energy = n_nodes * n_nodes * w_max * 1.0E5;

	w_min = MIN_PERCENT_WEIGHTS * w_max;

	pi = 4.0 * atan(1.0);

/*	if ( (2 * n_symbol_centers) > (parameter_for_network_storage_capacity * 0.138 * n_nodes) ) 
	 gerror ("Number of symbol centers greater than network storage capacity. In initialize_boltzmann_machine_state(), (simboltz.c).");
*/          /* Hopfield model estimates storage capacity ~ (0.138 * n_nodes). 
		I phenomenologically assumed this value parameter_for_network_storage_capacity times larger.  */

/*	sigma2 = 1.0 / (parameter_for_network_storage_capacity * 0.138 * pi);   */  
          /* Hopfield model estimates 1/(0.138 pi). I phenomenologically assumed this value
             parameter_for_network_storage_capacity times smaller  */

	sigma2 = (dimension_cluster1 * dimension_cluster1) / 
                        (parameter_for_network_storage_capacity * 0.138 * n_nodes * pi *0.5);  

	fprintf(finalparamf, "\n\t Square of width of Gaussian Distribution sigma2 = %lf \n", sigma2);

	fprintf(finalparamf, "\n\t Width of Gaussian Distribution sigma = %lf \n", sqrt(sigma2) );

/*	sigma2 = (dimension_cluster1 * dimension_cluster1) / (0.138 * n_nodes * pi * 0.5);  */

	neuron = (neuron_inf **) malloc ( n_nodes*sizeof(neuron_inf *) );
	check_allocation( (void *)neuron,
			     "initialize_boltzmann_machine_state()", "neuron");

	for (i = 0; i < n_nodes; i++)
	{
		neuron[i] = (neuron_inf *) malloc ( sizeof(neuron_inf) );
		check_allocation( (void *)neuron[i],
			     "initialize_boltzmann_machine_state()", "neuron[i]");
		neuron[i]->num_neighbors = 0;
			/* equivalent to (*neuron[i]).num_neighbors = 0; */
		neuron[i]->list_neighs = NO_NEIGHBOR;
	}


	ww = (double **) malloc(n_nodes*n_nodes*sizeof(double *));
	check_allocation( (void *)ww,
			     "initialize_boltzmann_machine_state()", "ww");

	connectivity_distribution(dimension_cluster1, dimension_cluster2); 

	pos_sens_mem = fopen("positions_sensorial_memory", "w");
	pos_decl_mem = fopen("positions_declarative_memory", "w");
 	distribute_neurons_spatially_uniformly_in_memory(pos_sens_mem, 0, n_memory1-1, dimension_cluster1, false);
	distribute_neurons_spatially_uniformly_in_memory(pos_decl_mem, n_memory1, n_nodes-1, dimension_cluster2, true);
	fclose(pos_sens_mem);
	fclose(pos_decl_mem);

	allocate_synapses_and_weights_in_memories();
		     
/*	for (i = 0; i < n_memory1; i++) */  /* upper-left corner of matrix  */
/*	{
		w(i,i) = BOLTZNULL;
		for (j = i+1; j < n_memory1; j++)
		{
			weight = w_max * ( 1.0 - 2.0 * ran3(&iseed) );
			w(i,j) = insert_second_neuron_in_neighbor_list_of_first(i, j, weight);
			w(j,i) = insert_second_neuron_in_neighbor_list_of_first(j, i, weight);
		}
	}

	for (i = n_memory1; i < n_nodes; i++) */  /* lower-right corner of matrix  */
/*	{
		w(i,i) = BOLTZNULL;
		for (j = i+1; j < n_nodes; j++)
		{
			weight = w_max * ( 1.0 - 2.0 * ran3(&iseed) );
			w(i,j) = insert_second_neuron_in_neighbor_list_of_first(i, j, weight);
			w(j,i) = insert_second_neuron_in_neighbor_list_of_first(j, i, weight);
		}
	}
	
	for (i = 0; i < n_memory1; i++) */ /* upper-right and lower-left corners of matrix  */
/*		for (j = n_memory1; j < n_nodes; j++)
		{
			weight = small * w_max * ( 1.0 - 2.0 * ran3(&iseed) );
			w(i,j) = insert_second_neuron_in_neighbor_list_of_first(i, j, weight);
			w(j,i) = insert_second_neuron_in_neighbor_list_of_first(j, i, weight);
		}
*/

	input_pattern = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)input_pattern,
           "initialize_boltzmann_machine_state()", "input_pattern");

        for (j = 0; j < n_nodes; j++)
			input_pattern[j] = (int) (ran3(&iseed) + 0.5);

	print_weight_matrix();

/*	output_neighbor_lists();

	if(output_to_monitor)
  	  printf("\n\t Will test patterns for minimum energy. \n");
	fprintf(finalparamf, "\n\t Will test patterns for minimum energy. \n");
	print_time();

	test_minimum_energy();

	if(output_to_monitor)
	  printf("\n\t Tested patterns for minimum energy. \n");
	fprintf(finalparamf, "\n\t Tested patterns for minimum energy. \n");
	print_time();
*/

	if(output_to_monitor)
	  printf("\n\t Will find initial network stored patterns. \n");
	fprintf(finalparamf, "\n\t Will find initial network stored patterns. \n");
	print_time();

	freq_visited_pattern_BM = fopen("frequency_visited_stored_pattern_BM", "w");
	freq_visited_pattern_GSA = fopen("frequency_visited_stored_pattern_GSA", "w");

	if(store_found_patterns_memory)
	  find_network_stored_patterns_use_memory(patternf, patternf_GSA, freq_visited_pattern_BM, freq_visited_pattern_GSA);
	else find_network_stored_patterns(patternf, patternf_GSA, freq_visited_pattern_BM, freq_visited_pattern_GSA);

	fclose(freq_visited_pattern_BM);
	fclose(freq_visited_pattern_GSA);

	if(output_to_monitor)
	  printf("\n\t Found initial network stored patterns. \n");
	fprintf(finalparamf, "\n\t Found initial network stored patterns. \n");
	print_time();


} /* end initialize_boltzmann_machine_state() */






/*
********************************************************************************
Function that tests recovering capabilities of Boltzmann Machine as a function
of temperature.

called by:	main()
arguments:	configurationf <FILE *>
functions called:
	recover_stored_patterns_BM() <int *>
	Energy() <double>: total energy of a given state of the binary neural network.
	convert_bitstring_into_integer() <double>
global variables: input_pattern	<int *>: stores a randomly generated 
                        (in function initialize_boltzmann_machine_state())
			pattern for testing Boltzmann machine recovery capability.
local variables: 
I/O conditions:
********************************************************************************
*/
void
recover_neurotic_states_as_associative_memory(configurationf)
FILE	*configurationf;
{
	extern	double	convert_bitstring_into_integer();	

	double 	temperature, energy_stored_pattern;
	int 	j, *stored_pattern;
	double	num_pattern, num_input_pattern;


        		
        num_input_pattern = convert_bitstring_into_integer (input_pattern, n_nodes);
        
/*	
	if(output_to_monitor)
        {
          printf("\n\n\n\t In recover_neurotic_states_as_associative_memory, input_pattern: ");
          for (j = 0; j < n_nodes; j++)
        		printf(" %d", input_pattern[j]);        
          printf("\n\t input_pattern = %lf \n", num_input_pattern);
	}
*/        
	for (temperature = termination_temperature; temperature <= initial_temperature; 
		temperature = temperature + delta_temperature)
	{
		stored_pattern = recover_stored_patterns_BM(input_pattern, 
					temperature, alpha_ass_mem);

		energy_stored_pattern = Energy (stored_pattern);

                num_pattern = convert_bitstring_into_integer (stored_pattern, n_nodes);

		fprintf(configurationf, " temperature = %f, pattern = %lf, energy = %f\n", 
			 temperature, num_pattern, energy_stored_pattern);

		free (stored_pattern);

	}

	fprintf(configurationf, " input_pattern = %lf\n", num_input_pattern);
	
/*
	if(output_to_monitor)	
	  printf("\n\n\n\t Will exit recover_neurotic_states_as_associative_memory().\n\n");  */

}  /*  end recover_neurotic_states_as_associative_memory()  */




/*
********************************************************************************
Function that simulates adaptive learning envolved in working-through process.

called by: main()
arguments:
functions called: find_network_stored_patterns()  (in boltz_functs.c)
                  find_network_stored_patterns_use_memory() (in boltz_functs.c)
		  recover_stored_patterns_BM()
		  gerror()
		  generate_new_state()  (in boltz_functs.c)
		  check_allocation()
		  reinforce_connections_to_affected_neuron() (in boltz_functs.c)
		  recover_neurotic_states_as_associative_memory()
		  convert_bitstring_into_integer()  (in boltz_functs.c)
global variables:
	errorf <pointer>: pointer to file structure for file "simboltz_errors"
			  where error messages are placed.
local variables:
	changed <int>:  
	changed_in_all_iterations <int>: 
I/O conditions:
   The w+ option in the fopen function opens the file in read/write mode
     (opens as a text file).
   function feof() != 0 if the end of the specified file is reached.
   function rewind() sets file position pointer to initial position.
********************************************************************************
*/
void
simulate_psychoanalytical_working_through(patternf)
FILE	*patternf;
{
	extern	void	find_network_stored_patterns(), find_network_stored_patterns_use_memory();
	extern	int	generate_new_state();
	extern	bool	reinforce_connections_to_affected_neuron();
	extern	double	convert_bitstring_into_integer(), calculate_clustering_coefficient();
	void		print_weight_matrix(), test_minimum_energy();

	FILE	*new_patternf, *new_patternf_GSA, *new_freq_visited_pattern_BM,
		*new_freq_visited_pattern_GSA, *new_patternf_ass_mem;

	int	i, j, k, m, *old_neurotic_state, *neurotic_state, *associated_pattern, 
		*analysts_stimulus, *state_changed, new_state, 
		end_of_file, changed, changed_in_all_iterations, sign;
	double	temperature, double_nothing, nothing, clustering_coefficient;
	

        old_neurotic_state = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)old_neurotic_state,
                   "simulate_psychoanalytical_working_through()", "old_neurotic_state");

        analysts_stimulus = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)analysts_stimulus,
                   "simulate_psychoanalytical_working_through()", "analysts_stimulus");

        state_changed = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)state_changed,
                 "simulate_psychoanalytical_working_through()", "state_changed");
                      
        rewind (patternf);
	fscanf(patternf, "%lf", &nothing);
	end_of_file = feof(patternf);
	rewind (patternf);
	
	if (end_of_file == 0)
	{
		fscanf(patternf, "%*c%lf", &nothing);
		for (k = 0; k < n_nodes; k++)
		{
			fscanf(patternf, "%*c%d", &old_neurotic_state[k]);
			
			analysts_stimulus[k] = old_neurotic_state[k];

		}  /*  end read first pattern from file  */
                fscanf(patternf, "%*c%lf", &double_nothing);
		fscanf(patternf, "%*c");

		neurotic_state = old_neurotic_state;
	}
	else gerror("Net stores no patterns. In simulate_psychoanalytical_working_through(), (simboltz.c).");

	fprintf(errorf, "\n\n\t neurotic_state: ");
        	for (j = 0; j < n_nodes; j++)
        	  fprintf(errorf, " %d", neurotic_state[j]);
        		
        fprintf(errorf, "\n\t num_neurotic_state = %lf", 
        	         convert_bitstring_into_integer(neurotic_state, n_nodes) );


	changed_in_all_iterations = false;
	
	temperature = termination_temperature;

	while ( (temperature <= initial_temperature) && (!changed_in_all_iterations) )
	{
		fprintf(errorf, "\n\n ***************************************************");
		fprintf(errorf, "\n ***************************************************");
		fprintf(errorf, "\n\n\t\t Temperature: %lf", temperature);
		fprintf(errorf, "\n\n ***************************************************");
		fprintf(errorf, "\n ***************************************************\n\n");

		if(output_to_monitor)
		  printf("\n\t\t Temperature: %lf \n", temperature);

	  for (m = 0; m < num_reinforcement_iterations; m++)
	  {
	    j = 0;
	  
	    for (k = 0; k < n_memory1; k++) state_changed[k] = 0;
	    
	    changed = false;
	    
	    while ( (!changed) && (j < n_memory1) )   /* unconscious memory */
	    {
	       i = generate_new_state(neurotic_state, state_changed, 
	            &sign, &new_state, 0, n_memory1 - 1);

	       analysts_stimulus[i] = new_state;

/*	       fprintf(errorf, "\n\n\t tried change %d nodes,\t analysts_stimulus: ", (j - n_memory1 + 1));
               for (k = 0; k < n_nodes; k++)
        	 fprintf(errorf, " %d", analysts_stimulus[k]);
        		
               fprintf(errorf, "\n\t num_analysts_stimulus = %lf", 
        	 convert_bitstring_into_integer(analysts_stimulus, n_nodes) );
*/

	       associated_pattern = recover_stored_patterns_BM(analysts_stimulus, 
					temperature, alpha_ass_mem);

/*	       fprintf(errorf, "\n\n\t associated_pattern: ");
               for (k = 0; k < n_nodes; k++)
        	 fprintf (errorf, " %d", associated_pattern[k]);
        		
        	fprintf(errorf, "\n\t num_associated_pattern = %lf", 
        	  convert_bitstring_into_integer(associated_pattern, n_nodes) );
*/

	       for(k = n_memory1; k < n_nodes; k++)
		 if ( associated_pattern[k] != neurotic_state[k] )
		  changed = reinforce_connections_to_affected_neuron(analysts_stimulus, i, k, changed);	

	       if (!changed) analysts_stimulus[i] = neurotic_state[i];

	       free (associated_pattern);

	       j = j + 1;

	    }  /* end while through neurotic nodes  */
 
	    if (changed)
	    {
	    	old_neurotic_state = neurotic_state;
	    	neurotic_state = recover_stored_patterns_BM(old_neurotic_state, 
					temperature, alpha_ass_mem);
	    	free (old_neurotic_state);
	    	for (k = 0; k < n_nodes; k++)
			analysts_stimulus[k] = neurotic_state[k];
		fprintf(finalparamf, " \n\t learned in iteration = %d \n", m);
	    }

	    fprintf(errorf, "\n\n ***************************************************");
	    fprintf(errorf, "\n\n Reinforcement interation = %d \n", m);
	    fprintf(errorf, "\n\n\t analysts_stimulus after reinforcement: ");
        	    for (k = 0; k < n_nodes; k++)
        		fprintf(errorf, " %d", analysts_stimulus[k]);
        		
        	    fprintf(errorf, "\n\t num_analysts_stimulus = %lf", 
        	      convert_bitstring_into_integer(analysts_stimulus, n_nodes) );
	    fprintf(errorf, "\n\n *************************************************** \n\n");
	    
	    if (!changed_in_all_iterations) changed_in_all_iterations = changed;

	  }    /* end for reinforcement learning repetitions  */
		
	  if (!changed_in_all_iterations) temperature = temperature + delta_temperature;

	}  /*  end while through temperature  */

	print_weight_matrix();

	fprintf(finalparamf, " \n\n\t final temperature after working through = %lf,   learned = %d \n\n", 
                                temperature, changed_in_all_iterations);

	clustering_coefficient = calculate_clustering_coefficient();

  	fprintf(finalparamf, "\n\n\t clustering coefficient of final topology = %f\n\n", 
                 clustering_coefficient);

	if(output_to_monitor)
	  printf("\n\n\t Will find final network stored patterns. \n");
	fprintf(finalparamf, "\n\t Will find final network stored patterns. \n");
	print_time();

	new_patternf = fopen("new_stored_patterns", "w+");
	new_patternf_GSA = fopen("new_stored_patterns_GSA", "w+");
	new_freq_visited_pattern_BM = fopen("", "w+");
	new_freq_visited_pattern_GSA = fopen("", "w+");

	if(store_found_patterns_memory)
	  find_network_stored_patterns_use_memory(new_patternf, new_patternf_GSA, 
                  new_freq_visited_pattern_BM, new_freq_visited_pattern_GSA);
	else find_network_stored_patterns(new_patternf, new_patternf_GSA, 
                  new_freq_visited_pattern_BM, new_freq_visited_pattern_GSA);

	if(output_to_monitor)
	  printf("\n\n\t Found final network stored patterns. \n");
	fprintf(finalparamf, "\n\t Found final network stored patterns. \n");
	print_time();
	
	new_patternf_ass_mem = fopen ("new_memory_configurations", "w");
	
	recover_neurotic_states_as_associative_memory(new_patternf_ass_mem);

	fclose (new_patternf);	
	fclose (new_patternf_GSA);	
	fclose (new_freq_visited_pattern_BM);	
	fclose (new_freq_visited_pattern_GSA);
	
	fclose (new_patternf_ass_mem);

	free(neurotic_state);
	free(analysts_stimulus);
	free(state_changed);

/*	test_minimum_energy();   */
		
}  /* end simulate_psychoanalytical_working_through()  */





/*
********************************************************************************
Input initialization data from files.

called by: initialize_boltzmann_machine_state()
functions called: 
	stdio library
	print_initial_parameters() <void>:
	
global variables:


local variables:
	fp <pointer>: pointer to file structure for inboltz.dat

I/O conditions: initialize data in inboltz.dat
********************************************************************************
*/
void
input_data_from_file()
{
	void 	print_initial_parameters();

	FILE 	*fp;

	fp = fopen("inboltz.dat", "r");

	fscanf(fp, "%*s%d%*s %*s%d%*s %*s%d%*s %*s%d%*s %*s%d%*s %*s%d%*s %*s%d%*s %*s%d%*s %*s%lf%*s\
		    %*s%lf%*s %*s%lf%*s %*s%lf%*s %*s%lf%*s %*s%d%*s %*s%d%*s %*s%lf%*s\
		    %*s%d%*s %*s%lf%*s %*s%d%*s %*s%d%*s %*s%lf%*s %*s%lf%*s\
		    %*s%lf%*s %*s%lf%*s %*s%lf%*s %*s%lf%*s %*s%lf%*s %*s%lf%*s\
		    %*s%lf%*s %*s%d%*s %*s%d%*s %*s%lf%*s %*s%lf%*s",
               &output_to_monitor, &store_found_patterns_memory, 
	       &recovering_from_check_point, &num_found_patterns_between_checkpoints,
               &n_nodes, &n_memory1, &n_memory2, &n_symbol_centers,  
               &initial_temperature, &initial_seek_local_min_temperature, &termination_temperature, 
	       &alpha_global, &alpha_ass_mem, &n_flop_tries_per_node, 
	       &num_trial_initial_patterns, &small, &temp_intervals, &learning_factor,
	       &num_reinforcement_iterations, &inhibitive_adaptive_learning,
	       &w_max, &dimension_cluster1, &dimension_cluster2, 
               &percent_neighbor_loss, &cluster_formation_learning_factor,
	       &parameter_for_network_storage_capacity, &parameter_distance_between_centers,
	       &percent_total_synapses, &percent_synapses_between_memories, 
               &BoltzMachine, &GSA, &q_A, &q_V);

        fclose(fp);

	print_initial_parameters();

} /* end input_data_from_file()  */





/*
********************************************************************************
This function prints to the standard output the initial parameters for
the Boltzmann Machine Simulator.

called by:
	input_data_from_file()

arguments: none
functions called:
	stdio
global variables: all (see main header at top of file)
local variables: all (see main header at top of file)
I/O conditions: 
********************************************************************************
*/
void
print_initial_parameters()
{
	 fprintf(finalparamf, "\n\
         should output runtime information to monitor:\t\t\t\t\t\t %d\n\
         should store found patterns in memory:\t\t\t\t\t\t\t %d\n\
	 recovering from check point:\t\t\t\t\t\t\t\t\t %d\n\
	 number of found patterns between checkpoints:\t\t\t\t\t\t %d\n\
	 number of nodes in Boltzmann Machine:\t\t\t\t\t\t\t %d\n\
	 number of nodes in memory No. 1:\t\t\t\t\t\t\t %d\n\
	 number of nodes in memory No. 2:\t\t\t\t\t\t\t %d\n\
	 number of symbol centers in each memory:\t\t\t\t\t\t %d\n\
	 initial temperature for finding global minimum with simulated annealing:\t\t %f\n\
	 initial temperature for seeking local minima in net configuration: \t\t\t %f\n\
	 termination temperature for simulated annealing:\t\t\t\t\t %f\n\
	 temperature decrease parameter for simulated annealing with global minimum search:\t %f\n\
	 temperature decrease parameter for loose simulated annealing (associative memory):\t %f\n\
	 parameter for determining no. iterations per temperature in simulated annealing:\t %d\n\
	 number trial patterns for network stored pattern detection:\t\t\t\t %d\n\
         parameter for initializing intercluster synapses:\t\t\t\t\t %f\n\
         number of temperature intervals for network associativity testing:\t\t\t %d\n\
	 learning parameter for network reconfiguration:\t\t\t\t\t %f\n\
	 number of iterations for reinforcement learning:\t\t\t\t\t %d\n\
	 use 0 / 1 (enter: 0) or -1 / +1 (enter: 1) for adaptive learning:\t\t\t %d\n\
	 maximum value for synaptic weights:\t\t\t\t\t\t\t %f\n\
	 spatial dimension cluster 1 (conscious declarative memory):\t\t\t\t %f\n\
	 spatial dimension cluster 2 (unconscious sensorial implicit memory):\t\t\t %f\n\
	 percentage of loss of neighbors during cluster formation in memories:\t\t\t %f\n\
	 learning parameter for formation of initial clusters:\t\t\t\t\t %f\n\
	 phenomenological parameter for network storage capacity: \t\t\t\t %f\n\
	 parameter for calculating min distance between symbol centers: \t\t\t %f\n\
	 percentage of synapses that connect symbol centers in memory: \t\t\t\t %f\n\
	 percentage of synapses that connect symbol centers between memories: \t\t\t %f\n\
         indication for using Boltzmann Machine:\t\t\t\t\t\t %d\n\
         indication for using Generalized Simulated Annealing:\t\t\t\t\t %d\n\
         q_A_for_Generalized_Simulated_Annealing:\t\t\t\t\t\t %f\n\
         q_v_for_Generalized_Simulated_Annealing:\t\t\t\t\t\t %f\n",	
             output_to_monitor, store_found_patterns_memory,
	     recovering_from_check_point, num_found_patterns_between_checkpoints,
	     n_nodes, n_memory1, n_memory2, n_symbol_centers, 
	     initial_temperature, initial_seek_local_min_temperature, termination_temperature,
	     alpha_global, alpha_ass_mem, n_flop_tries_per_node, 
	     num_trial_initial_patterns, small, temp_intervals, 
	     learning_factor, num_reinforcement_iterations, inhibitive_adaptive_learning,
	     w_max, dimension_cluster1, dimension_cluster2,
	     percent_neighbor_loss, cluster_formation_learning_factor, 
	     parameter_for_network_storage_capacity, parameter_distance_between_centers,
	     percent_total_synapses, percent_synapses_between_memories, 
             BoltzMachine, GSA, q_A, q_V);

}   /* end print_initial_parameters() */






/*
********************************************************************************
Given an input pattern, this function determines a pattern corresponding 
to an energy minima for a given network topology (values of wij's). 
It stabilizes onto an energy state by simulated annealing in a Boltzmann machine.
If annealing is very slow and gradual, this should be a global minimum. 
Energy and energy variation calculations are only valid if synaptic weights 
are symmetric (*w(i,j) = *w(j,i)).

Thermalization: Thermal equilibrium occurs when, for a certain temperature,
T, energy fluctuates no more.

Cristalization: Occurs when, for a phase change (decreasce of temperature)
energy does not decreasce.

See algorithm in: Barbosa V. C., "Massively Parallel Models of Computation",
		  Ellis Horwood, 1993. 

called by:
	find_network_stored_patterns()  (boltz_functs.c)
	find_network_stored_patterns_use_memory() (boltz_functs.c)
	recover_neurotic_states_as_associative_memory()
	simulate_psychoanalytical_working_through()

arguments: 
	state <int *>: initial network state from which to anneal.
	start_temperature <double>: temperature from which to anneal.
	alpha <double>: annealing schedule, temperature decrease 
                            parameter for simulated annealing.
functions called:
     defined in this file
	Energy() <double>: Energy value of pattern passed by argument
	check_allocation() <void>:
     defined in file boltz_functs.c
	generate_new_state() <int>: Generates a network state 
				    S' (a pattern vector), from an original 
				    state S, by changing (flipping) a randomly 
				    chosen neuron's state, whose state has not 
			 	    yet been changed.
global variables:
	n_flop_tries_per_node <int>: parameter for determining no. iterations 
                                     per temperature in simulated annealing.
	termination_temperature <double>: termination temperature 
						for simulated annealing.
	n_nodes <int>: number of nodes in Boltzmann Machine.
	errorf <pointer>: pointer to file structure for file "simboltz_errors"
			  where error messages are placed.

local variables:
	n_iterations <int>: total number of iterations per temperature value.
	iteration <int>: Value of current iteration for a value of annealing 
	                  temperature.
	new_state <int>: Value of chosen neuron's flipped state for energy 
			minimization testing.
	stored_state <* int>: pointer to vector that stores
		network state during annealing. After annealing stores minimum
		energy state. This is this function's output value.
	state_changed <* int>: vector whose elements say if a neuron's state has
		been changed for energy minimization testing in each iteration.
	temperature <double>: Value of current annealing temperature. 
	delta_energy,  energy_before, energy_after, d_energy <double>: energy 
 		parameters network states.
	probability <double>:
	cristalized <int>: Variable that indicates convergence (stabilization of
		energy) over any initial temperature value. 
		Convergence to global minimum. Result of annealing process.
	thermalized <int>: Variable that indicates convergence (stabilization of
		energy) for a given a certain initial temperature value. 
		Convergence to local minimum.
	mean_energy <double>: mean value of energy for convergence tests.
	last_mean_energy <double>: latest last mean value of energy 
			for convergence tests.
	last_temp_mean_energy <double>: mean value of energy at last 
		temperature value for convergence tests.
I/O conditions:
********************************************************************************
*/
int	* 
recover_stored_patterns_BM(state, start_temperature, alpha) 
int	*state;
double	start_temperature, alpha;
{
	int	new_state, *stored_state, *state_changed, 
		iteration, n_iterations, n_iterations_mean_energy, sign, i, j, k, i_mean;
	int	cristalized, thermalized;
	double	temperature, delta_energy, probability, energy_before, energy_after,
		mean_energy, last_temp_mean_energy, last_mean_energy, convergence_error;
	double	d_energy;
	neighbor_inf	*np;
	
	extern	int	generate_new_state();

                    
/*
	if(output_to_monitor)
	{
	  printf("\n\n\t state: ");
          for (j = 0; j < n_nodes; j++)
          {	
        	printf("\t %d", state[j]);
        	if (j == (n_nodes - 1) ) printf("\n\n");
          }
	} 
*/

        stored_state = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)stored_state,
                      "recover_stored_patterns_BM()", "stored_state");

        state_changed = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)state_changed,
                      "recover_stored_patterns_BM()", "state_changed");

	for (i = 0; i < n_nodes; i++)
		stored_state[i] = state[i];

	n_iterations_mean_energy = n_flop_tries_per_node * n_nodes;

	n_iterations = n_nodes;

	cristalized = false;
	temperature = start_temperature;
	last_temp_mean_energy = infinite_energy;

	while ( !cristalized && (temperature >= termination_temperature) )
	{
             iteration = 0;
             thermalized = false;
	     last_mean_energy = infinite_energy;

	     while ( !thermalized && (iteration < n_iterations) )
	     {
	       mean_energy = 0.0;

	       for (i_mean = 0; i_mean < n_flop_tries_per_node; i_mean++)
	       {
		  for (k = 0; k < n_nodes; k++)
			state_changed[k] = 0;

             	  for (k = 0; k < n_nodes; k++)
	     	  {
			i = generate_new_state(stored_state, state_changed, 
				&sign, &new_state, 0, n_nodes-1);

			delta_energy = 0.0;

	  		for (np = neuron[i]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)
	    		  delta_energy = delta_energy + np->wij * stored_state[np->neigh_ID];

			delta_energy = sign * delta_energy;

/*         delta_energy = (energy_after - energy_before) = d_energy   (see definition in Barbosa)   */
			
			energy_before = Energy(stored_state);

/*			if(output_to_monitor)
			  printf("\n\t iteration: %d \t i_mean: %d \t k: %d \t \n\t 
				changed_neuron: %d \t stored_state[i]: %d \t energy_before: %lf\n", 
				iteration, i_mean, k, i, stored_state[i], energy_before);	*/

			probability = 1.0 / ( 1.0 + exp(delta_energy / temperature) );

			if ( ran3(&iseed) <= probability ) 
				stored_state[i] = new_state;

/*			energy_after = Energy(stored_state);
			if (energy_after > energy_before)
			{
				d_energy = energy_after - energy_before;
				if(output_to_monitor)
				{
				  printf("\n\t iteration: %d \t i_mean: %d \t k: %d \t \n 
				    changed_neuron: %d \t stored_state[i]: %d \t temperature: %lf \t energy_before: %lf\n", 
				    iteration, i_mean, k, i, stored_state[i], temperature, energy_before);	
				  printf("\t new_state: %d \t energy_after: %lf \t d_energy: %lf 
				    \t delta_energy: %lf \t prob: %lf\n", 
			            new_state, energy_after, d_energy, delta_energy, probability);
				}
			}
*/
			mean_energy = mean_energy + Energy(stored_state);

	     	  }   /* end for over all nodes  (k) */


		}   /*  end for over loop for evaluation of mean energy (i_mean) */

		mean_energy = mean_energy / n_iterations_mean_energy;
		convergence_error = (mean_energy - last_mean_energy) / mean_energy;
		convergence_error = fabs ( convergence_error );
		if ( convergence_error < PERCENT_ERROR_ENERGY ) thermalized = true;
		else last_mean_energy = mean_energy;

		iteration = iteration + 1;

	     }  /*  end while iterations for one temperature  */

	     if (mean_energy >= last_temp_mean_energy) cristalized = true;
	     else 
	     {
		last_temp_mean_energy = mean_energy;
		temperature = alpha * temperature;
	     }

	} /* end while over temperatures */
	
/*	print_text ("Recovered a pattern!!!", temperature, iteration);
	
	getchar();
*/
	free(state_changed);

	return(stored_state);

}    /* end recover_stored_patterns_BM() */




/*
********************************************************************************
Given an input pattern, this function determines a pattern corresponding 
to an energy minima for a given network topology (values of wij's). 
It stabilizes onto an energy state by Generalized Simulated Annealing.
If annealing is very slow and gradual, this should be a global minimum. 
Energy and energy variation calculations are only valid if synaptic weights 
are symmetric (*w(i,j) = *w(j,i)). Annealing process visits states in a sequence 
different than in the Boltzmann Machine.

Thermalization: Thermal equilibrium occurs when, for a certain temperature,
T, energy fluctuates no more.

Cristalization: Occurs when, for a phase change (decreasce of temperature)
energy does not decreasce.

See algorithm in: Tsallis C. and Stariolo, D. A., "Generalized Simulated Annealing",
		  Physica A 233 (1996) 395-406. 

called by:
	find_network_stored_patterns()  (boltz_functs.c)
	find_network_stored_patterns_use_memory() (boltz_functs.c)
	recover_neurotic_states_as_associative_memory()
	simulate_psychoanalytical_working_through()

arguments: 
	state <int *>: initial network state from which to anneal.
	start_temperature <double>: temperature from which to anneal.
	alpha <double>: annealing schedule, temperature decrease 
                            parameter for simulated annealing.
functions called:
     defined in this file
	Energy() <double>: Energy value of pattern passed by argument
	check_allocation() <void>:
     defined in file boltz_functs.c
	generate_new_state() <int>: Generates a network state 
				    S' (a pattern vector), from an original 
				    state S, by changing (flipping) a randomly 
				    chosen neuron's state, whose state has not 
			 	    yet been changed.
global variables:
	n_flop_tries_per_node <int>: parameter for determining no. iterations 
                                     per temperature in simulated annealing.
	termination_temperature <double>: termination temperature 
						for simulated annealing.
	n_nodes <int>: number of nodes in Boltzmann Machine.
	errorf <pointer>: pointer to file structure for file "simboltz_errors"
			  where error messages are placed.

local variables:
	n_iterations <int>: total number of iterations per temperature value.
	iteration <int>: Value of current iteration for a value of annealing 
	                  temperature.
	new_state <int>: Value of chosen neuron's flipped state for energy 
			minimization testing.
	stored_state <* int>: pointer to vector that stores
		network state during annealing. After annealing stores minimum
		energy state. This is this function's output value.
	state_changed <* int>: vector whose elements say if a neuron's state has
		been changed for energy minimization testing in each iteration.
	temperature <double>: Value of current annealing temperature. 
	delta_energy,  energy_before, energy_after, d_energy <double>: energy 
 		parameters network states.
	probability <double>:
	cristalized <int>: Variable that indicates convergence (stabilization of
		energy) over any initial temperature value. 
		Convergence to global minimum. Result of annealing process.
	thermalized <int>: Variable that indicates convergence (stabilization of
		energy) for a given a certain initial temperature value. 
		Convergence to local minimum.
	mean_energy <double>: mean value of energy for convergence tests.
	last_mean_energy <double>: latest last mean value of energy 
			for convergence tests.
	last_temp_mean_energy <double>: mean value of energy at last 
		temperature value for convergence tests.
I/O conditions:
********************************************************************************
*/
int	* 
recover_stored_patterns_GSA(state, start_temperature, alpha) 
int	*state;
double	start_temperature, alpha;
{
	int	new_state, *stored_state, *state_changed, *trial_state,
		iteration, n_iterations, n_iterations_mean_energy, sign, i, j, k, i_mean;
	int	cristalized, thermalized;
	double	temperature, delta_energy, probability, energy_before, energy_after,
		mean_energy, last_temp_mean_energy, last_mean_energy, convergence_error;
	double	d_energy, q_A_minus_1, one_over_q_A_minus_1, prob_term;
	neighbor_inf	*np;
	
	extern	int	generate_new_state();

                    
/*	if(output_to_monitor)
	{
	  printf("\n\n\t state: ");
          for (j = 0; j < n_nodes; j++)
          {	
        	printf("\t %d", state[j]);
        	if (j == (n_nodes - 1) ) printf("\n\n");
          }
	}
*/

	q_A_minus_1 = q_A - 1.0;

	one_over_q_A_minus_1 = 1.0 / q_A_minus_1;

        stored_state = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)stored_state,
                      "recover_stored_patterns_GSA()", "stored_state");

        trial_state = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)trial_state,
                      "recover_stored_patterns_GSA()", "trial_state");

        state_changed = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)state_changed,
                      "recover_stored_patterns_GSA()", "state_changed");

	for (i = 0; i < n_nodes; i++)
	{
		stored_state[i] = state[i];
		trial_state[i] = state[i];
	}

	n_iterations_mean_energy = n_flop_tries_per_node * n_nodes;

	n_iterations = n_nodes;

	cristalized = false;
	temperature = start_temperature;
	last_temp_mean_energy = infinite_energy;

	while ( !cristalized && (temperature >= termination_temperature) )
	{
             iteration = 0;
             thermalized = false;
	     last_mean_energy = infinite_energy;

	     while ( !thermalized && (iteration < n_iterations) )
	     {
	       mean_energy = 0.0;

	       for (i_mean = 0; i_mean < n_flop_tries_per_node; i_mean++)
	       {
		  for (k = 0; k < n_nodes; k++)
			state_changed[k] = 0;

             	  for (k = 0; k < n_nodes; k++)
	     	  {
			i = generate_new_state(stored_state, state_changed, 
				&sign, &new_state, 0, n_nodes-1);

			trial_state[i] = new_state;

/*			delta_energy = 0.0;

	  		for (np = neuron[i]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)
	    		  delta_energy = delta_energy + np->wij * stored_state[np->neigh_ID];

			delta_energy = sign * delta_energy;                       */

			energy_before = Energy(stored_state);

			energy_after = Energy(trial_state);

           		delta_energy = (energy_after - energy_before); 

/*			if(output_to_monitor)
			  printf("\n\t iteration: %d \t i_mean: %d \t k: %d \t \n\t\
				changed_neuron: %d \t stored_state[i]: %d \t energy_before: %lf \t energy_after: %lf\n", 
				iteration, i_mean, k, i, stored_state[i], energy_before, energy_after);
*/
			if (delta_energy <= 0.0)
			{
				stored_state[i] = new_state;
			}
			else
			{
			  prob_term = 1.0 + (q_A_minus_1*delta_energy / temperature);

			  if ( (q_A < 1.0) && (prob_term < 0.0) ) probability = 0.0;

			  else probability = 1.0 / pow ( prob_term, one_over_q_A_minus_1 ) ;


			  if ( ran3(&iseed) <= probability ) 
				stored_state[i] = new_state;
			  else trial_state [i] = stored_state[i];
			}

			energy_after = Energy(stored_state);

/*			if (energy_after > energy_before)
			{
			  d_energy = energy_after - energy_before;
			  if(output_to_monitor)
			  {
			    printf("\n\t iteration: %d \t i_mean: %d \t k: %d \t \n\
				  changed_neuron: %d \t stored_state[i]: %d \t temperature: %lf \t new_state: %d\n", 
				  iteration, i_mean, k, i, stored_state[i], temperature, new_state);	
			    printf("\t energy_before: %lf \t energy_after: %lf \t d_energy: %lf \t delta_energy: %lf \t prob: %lf\n", 
			          energy_before, energy_after, d_energy, delta_energy, probability);
			  }
			}
*/
			mean_energy = mean_energy + energy_after;

	     	  }   /* end for over all nodes  (k) */


		}   /*  end for over loop for evaluation of mean energy (i_mean) */

		mean_energy = mean_energy / n_iterations_mean_energy;
		convergence_error = (mean_energy - last_mean_energy) / mean_energy;
		convergence_error = fabs ( convergence_error );
		if ( convergence_error < PERCENT_ERROR_ENERGY ) thermalized = true;
		else last_mean_energy = mean_energy;

		iteration = iteration + 1;

	     }  /*  end while iterations for one temperature  */

	     if (mean_energy >= last_temp_mean_energy) cristalized = true;
	     else 
	     {
		last_temp_mean_energy = mean_energy;
		temperature = alpha * temperature;
	     }

	} /* end while over temperatures */
	
/*	print_text ("Recovered a pattern!!!", temperature, iteration);
	
	getchar();
*/
	free(state_changed);

	return(stored_state);

}    /* end recover_stored_patterns_GSA() */




/*
********************************************************************************
This function calculates the total energy of the binary neural network. It
is valid if the weight matrix is symmetric (*w(i,j) = *w(j,i)).
See definitions in "Introduction to the Theory of Neural Networks" 
by Hertz, et al. (equation 7.3) and "Massively Parallel Models of Computation"
by Valmir C. Barbosa (equation 9.11).

called by:
	recover_neurotic_states_as_associative_memory()
	recover_stored_patterns_BM()
	recover_stored_patterns_GSA()
	test_minimum_energy()
arguments:
functions called:
global variables:
local variables:
I/O conditions:
********************************************************************************
*/
double
Energy(neuron_states)
int	*neuron_states;
{
	double	total_energy = 0.0;
	int	i, j;
	neighbor_inf	*np;

	for ( i = 0; i < n_nodes; i++)
	{
	  for (np = neuron[i]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)
	    total_energy = total_energy - np->wij * neuron_states[i] * neuron_states[np->neigh_ID];
	}  /*  end for  */

	total_energy = total_energy / 2.0;

	return (total_energy);

}  /*  end Energy()  */





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


called by:
	form_Clusters_in_memory()  (boltz_functs.c)
arguments:
functions called:
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void
remove_second_neuron_from_neighbor_list_of_first(i, j, np)
int	i, j;
neighbor_inf 	*np;
{

	neighbor_inf	*previous, *next;

	if (np == NO_NEIGHBOR) 
	  gerror ("Removing nothing from event list!!\
		Function: remove_second_neuron_from_neighbor_list_of_first(), (in simboltz.c).\n");

	previous = np->prev_neigh;
	next = np->next_neigh;

	if (neuron[i]->list_neighs == np) neuron[i]->list_neighs = next;

	if (next != NO_NEIGHBOR) next->prev_neigh = previous;
	if (previous != NO_NEIGHBOR) previous->next_neigh = next;

	neuron[i]->num_neighbors = neuron[i]->num_neighbors - 1;

	w(i,j) = BOLTZNULL;

	free (np);  	

}   /*  end remove_second_neuron_from_neighbor_list_of_first()   */







/*
*******************************************************************************
Calculates the base 2 logarithm of a double precision number

called by:
arguments:
	x <double>: function will return base 2 logarithm of x
functions called:
global variables:
local variables:
I/O conditions: x must be a double precision number.
********************************************************************************
*/
double
log_2(x)
double	x;
{
	return (log(x)/log(2.0));

} /* end log_2()  */





/*
********************************************************************************
Prints to standard output and to file indicated by pointer finalparamf
the total elapsed time and elapsed time since last call to this function.
Also prints normal time of day.

called by:
	main()
	simulate_psychoanalytical_working_through()
arguments:
functions called:
global variables:
local variables:
I/O conditions:
********************************************************************************
*/
void
print_time()
{
	time_t		*time_program;
	TIMEVAL		*program_run_time;
	TIMEZONE	*t_zone;
	long		global_clock, elapsed_seconds;


        program_run_time = (TIMEVAL *) malloc(sizeof(TIMEVAL));
        check_allocation( (void *)program_run_time, "print_time()", "program_run_time");

        t_zone = (TIMEZONE *) malloc(sizeof(TIMEZONE));
        check_allocation( (void *)t_zone, "print_time()", "t_zone");

        time_program = (time_t *) malloc(sizeof(time_t));
        check_allocation( (void *)time_program, "print_time()", "time_program");

	time(time_program);
	gettimeofday(program_run_time, t_zone);

	global_clock = program_run_time->tv_sec - initial_seconds_count;

	elapsed_seconds = program_run_time->tv_sec - last_seconds_count;
	last_seconds_count = program_run_time->tv_sec;

	if(output_to_monitor)
	  printf(" \n\t Time = %s \t elapsed: %ld seconds, \t total elapsed seconds: %ld  \n\n", 
			ctime(time_program), elapsed_seconds, global_clock);

	fprintf(finalparamf, " \n\t Time = %s \t elapsed: %ld seconds, \t total elapsed seconds: %ld \n\n", 
			ctime(time_program), elapsed_seconds, global_clock);

	free(program_run_time);
	free(t_zone);
	free(time_program);

}  /*  end print_time()   */




/*
********************************************************************************
General standard error handler

called by:
	
arguments:
	error_text <string>: error message
functions called:
	standard I/O
global variables:
local variables:
I/O conditions:
********************************************************************************
*/
void
print_text(error_text, temperature, iteration)
char	error_text[];
double	temperature;
long	iteration;
{
	if(output_to_monitor)
	{
	  printf("\n\n\n\n\n");
	  printf("\t\t%s\n", error_text);
	  printf("   \t\t temperature = %f, iteration = %ld\n\n\n", 
	         temperature, iteration);
	}

}   /*    end print_text()   */







/*
********************************************************************************
Numerical Recipes standard error handler

called by:
	rtnewt(), rtsafe(), rtstep() 
arguments:
	error_text <string>: error message
functions called:
	standard I/O
global variables:
	errorf <pointer>: pointer to file structure for file "simboltz_errors"
			  where error messages are placed.

local variables:
I/O conditions:
********************************************************************************
*/
void
nerror(error_text)
char	error_text[];
{
	fprintf(errorf,"\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	fprintf(errorf, "\n Numerical Recipies run-time error...\n");
	fprintf(errorf, "%s\n", error_text);
	fprintf(errorf, "...now exiting to system...\n\n");
	fprintf(errorf, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n");
	exit(0);

}   /*    end nerror()   */




/*
********************************************************************************
General standard error handler

called by:
	
arguments:
	error_text <string>: error message
functions called:
	standard I/O
global variables:
	errorf <pointer>: pointer to file structure for file "simboltz_errors"
			  where error messages are placed.

local variables:
I/O conditions:
********************************************************************************
*/
void
gerror(error_text)
char	error_text[];
{
	if(output_to_monitor)
	{
	  printf("\n\t General run-time error...\n");
	  printf("\t%s\n", error_text);
	  printf("\t...now exiting to system...\n\n");
	}

	fprintf(errorf,"\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	fprintf(errorf, "\n\t General run-time error...\n");
	fprintf(errorf, "\t%s\n", error_text);
	fprintf(errorf, "\t...now exiting to system...\n\n");
	fprintf(errorf, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n");
	exit(0);

}   /*    end gerror()   */



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
    fprintf(errorf, "\n\t Run-time memory allocation error...\n");
    fprintf(errorf, "\t In function: %s, variable: %s\n", in_funct, in_var);
    fprintf(errorf, "\t ...now exiting to system...\n\n");
    fprintf(errorf, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n");
    exit(0);
  }

}   /*  check_allocation()   */




/*
********************************************************************************
Function for program termination.

called by:  main()
	
arguments:
	error_text <string>: error message
functions called:
	standard I/O
global variables:
	errorf <pointer>: pointer to file structure for file "simboltz_errors"
			  where error messages are placed.

local variables:
I/O conditions:
********************************************************************************
*/
void
finish_here(finish_text, patternf, patternf_GSA)
char	finish_text[];
FILE	*patternf, *patternf_GSA;
{
	if(output_to_monitor)
	{
	  printf("\n\t Reached termination point. \n");
	  printf("\t%s\n", finish_text);
	  printf("\t...now exiting to system...\n\n");
	}

	fprintf(errorf,"\n\n\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	fprintf(errorf, "\n\t Reached termination point. \n");
	fprintf(errorf, "\t%s\n", finish_text);
	fprintf(errorf, "\t...now exiting to system...\n\n");
	fprintf(errorf, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n\n\n");


	fclose(patternf);
	fclose(patternf_GSA);	
	fclose(weightf);
	fclose(errorf);

	if(output_to_monitor)
	  printf("\n\t Exiting Program Time \n");

	fprintf(finalparamf, "\n\t Exiting Program Time \n");
	print_time();

	fclose(finalparamf);

	exit(0);

}   /*    end finish_here()   */






/*
********************************************************************************


called by:
arguments:
functions called:
global variables:
local variables:
I/O conditions:
********************************************************************************
*/
void
print_weight_matrix()
{
	int i, j;

	fprintf(weightf, "\n\n\n");
	for (i = 0; i < n_nodes; i++)
	{
		fprintf(weightf, "\n\t i = %d", i);
		for (j = 0; j < n_nodes; j++)
		{
/*			if (j == i) fprintf(weightf, "\t %f", 0.0 );  */
			if (w(i,j) == BOLTZNULL) fprintf(weightf, "\t %f", 0.0 ); 
			else fprintf(weightf, "\t %f", *w(i,j) );
		}
	}

}  /*   end print_weight_matrix  */





/*
********************************************************************************
Tests for 3 minimum energy values for all possible patterns for a given 
topology of synaptic weights. Does an exaustive search.

called by:
arguments:
functions called:
	Energy() <double>: total energy of a given state of the binary neural network.
global variables:
local variables:
I/O conditions:  the "a" parameter in the fopen function indicates that the file
   should be opened for appending (not to be overwritten).
********************************************************************************
*/
void
test_minimum_energy()
{
	FILE		*minenergyf;

	double		i, i_max;
	long 		k;
	double	min_energy, min_energy1, min_energy2, pattern_energy;
	int	*test_pattern;

	void	add_one_to_bit_pattern();
	extern	double	convert_bitstring_into_integer();

	i_max = pow(2.0, (double) n_nodes);

        test_pattern = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)test_pattern,
                      "test_minimum_energy()", "test_pattern");

	for (k = 0; k < n_nodes; k++) test_pattern[k] = 0;

	min_energy = min_energy1 = min_energy2 = 0.0;

	for (i = 0.0; i < i_max - 1; i++)
	{
		add_one_to_bit_pattern(test_pattern);

/*		if(output_to_monitor)
		{
	          printf("\n\n\t next bit pattern: ");
        	       for (k = 0; k < n_nodes; k++)
        		printf(" %d", test_pattern[k]);
		}
*/

		pattern_energy = Energy(test_pattern);

/*		if(output_to_monitor)
		  printf("\n\t num_test_pattern = %lf, test_pattern_Energy = %lf", 
        	         convert_bitstring_into_integer(test_pattern, n_nodes), pattern_energy);
*/
		if (pattern_energy < min_energy)
		{
			min_energy2 = min_energy1;
			min_energy1 = min_energy;
			min_energy = pattern_energy;
		}
		else 
		{
		     if (pattern_energy < min_energy1)
		     {
			min_energy2 = min_energy1;
			min_energy1 = pattern_energy;
                     }
		     else if (pattern_energy < min_energy2) min_energy2 = pattern_energy;
		}

/*		if(output_to_monitor)
		  printf("\n\n \t minimum energies for network configuration: %lf,  %lf, %lf\n\n",
				 min_energy, min_energy1, min_energy2);  */

	}   /*  end for  */


	minenergyf = fopen("minimum_energies", "a");

	fprintf(minenergyf, "\n\n \t minimum energies for network configuration: %lf,  %lf, %lf\n\n",
				 min_energy, min_energy1, min_energy2);
	fclose (minenergyf);

	free (test_pattern);

}  /*  end test_minimum_energy()  */






/*
********************************************************************************


called by:
arguments:
functions called:
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void
add_one_to_bit_pattern(bit_pattern)
int	*bit_pattern;
{
	int 	changed, i;

	changed = i = 0;
	
	while (!changed && i < n_nodes)
	{
		if(bit_pattern[i] == 0)
		{
			bit_pattern[i] = 1;
			changed = 1;
		}
		else bit_pattern[i] = 0;

		i = i + 1;	
	}

}    /*   end add_one_to_bit_pattern()    */ 





/*
********************************************************************************
Function for verifying neighbor lists

called by:  initialize_boltzmann_machine_state()
	    allocate_synapses_and_weights_in_memories()  (in boltz_functs.c)
arguments:
functions called:  I/O
external variables:
local variables:
I/O conditions: 
********************************************************************************
*/
void
output_neighbor_lists(file_type)
int	file_type;
{
	FILE		*neighborf, *linksf;

	neighbor_inf	*np;
	int		i;
	double		deltax, deltay;



	switch (file_type)
	{
		case before_clustering:
		neighborf = fopen("neighbors", "w");
		linksf = fopen("synapses", "w");
		break;

		case clustering_within_memories:
		neighborf = fopen("clustered_neighbors_in_memories", "w");
		linksf = fopen("clustered_synapses_in_memory", "w");
		break;

		case long_range_synapses_in_memories:
		neighborf = fopen("long_range_neighbors_in_memories", "w");
		linksf = fopen("long_range_synapses_in_memory", "w");
		break;

		case connections_among_memories:
		neighborf = fopen("neighbors_among_memories", "w");
		linksf = fopen("synapses_connecting_memories", "w");
		break;

	}   /*   end switch   */


	for ( i = 0; i < n_nodes; i++)
	{

		fprintf(neighborf, "\n\n neuron[%d], num_neighbors = %d, neighbor list:\n ", i, neuron[i]->num_neighbors);

		for (np = neuron[i]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)
		{	   
	  	  fprintf(neighborf, "-> neigh = %d, wij = %f  ", np->neigh_ID, np->wij);
		  deltax = neuron[np->neigh_ID]->pos_coord1 - neuron[i]->pos_coord1;
		  deltay = neuron[np->neigh_ID]->pos_coord2 - neuron[i]->pos_coord2;
		  fprintf(linksf, "%lf \t %lf \t %lf \t %lf\n", neuron[i]->pos_coord1, neuron[i]->pos_coord2,
			deltax, deltay);

		}  /*  end for  */

	}  /*  end for  */

	fprintf(neighborf, "\n\n");

	fclose (neighborf);
	fclose (linksf);

}   /* end output_neighbor_lists() */
	


