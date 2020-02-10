/* 
********************************************************************************
               File with simulation functions for Boltzmann Simulator       


IMPORTANT OBSERVATIONS: All coordinates of vectors in this program are ordered
		so that the x-coordinate corresponds to index 2,
		the y-coordinate corresponds to index 1 and 
		the z-coordinate corresponds to index 0. For example, for
	 	vector v, v[2] = v_x, v[1] = v_y and v[0] = v_z.

		When referencing w matrix put right parenthesis next to w
		without blank spaces, for example, w(3,4) not w (3,4).
		The whole variable reference has to be written without blank
		spaces, e.g., w(i,j) not w(i, j). 


global variables read from files:

global variables:


********************************************************************************





********************************************************************************

			EXTERNAL VARIABLES AND FUNCTIONS

 Declarations of shared global variables and functions.

********************************************************************************
*/

#include    "boltzextr.h" /* Declarations of global variables and functions. */

/*
********************************************************************************
			Functions Global to this File
********************************************************************************
*/


/*
********************************************************************************
			Variables and Constants Global to this File
********************************************************************************
*/





/*
********************************************************************************
Performs random distributuion of neurons in bi-dimensional sheet (cortex).

called by:
	initialize_boltzmann_machine_state() (in simboltz.c)
	connectivity_distribution()
arguments:
	dimension <double>: 
functions called:
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void
distribute_neurons_spatially_uniformly_in_memory(positionf, first_neuron, last_neuron, dimension, declarative_mem)
FILE	*positionf;
int	first_neuron, last_neuron, declarative_mem;
double	dimension;
{
	int	i;
	for (i = first_neuron; i <= last_neuron; i++)
	{
		neuron[i]->pos_coord1 = dimension * ran3(&iseed);
		neuron[i]->pos_coord2 = dimension * ran3(&iseed);
		if (declarative_mem) neuron[i]->pos_coord2 = neuron[i]->pos_coord2 + 2 * dimension; 
		fprintf (positionf, " %lf \t %lf \n", neuron[i]->pos_coord1, neuron[i]->pos_coord2);
	}

}    /*  end distribute_neurons_spatially_uniformly_in_memory()     */ 







/*
********************************************************************************
Allocates synapses and weights to sensorial and declarative memories so as to
create clusters representing symbols. A kind of preferential attachment with
conservation of energy.

called by:
	initialize_boltzmann_machine_state() (in simboltz.c)
arguments:
functions called:
	distribute_synapses_in memory_by_gaussian_distribution()
	choose_symbol_centers_in_memory(),
	form_Clusters_in_memory(), 
	distribute_synapses_between_clusters_in_memory(),
	distribute_synapses_between_memories(),
	generate_node_degree_distribution()
	output_neighbor_lists() (in simboltz.c)
	calculate_clustering_coefficient() <double>
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void
allocate_synapses_and_weights_in_memories()
{
	extern void output_neighbor_lists();
	void 	distribute_synapses_in_memory_by_gaussian_distribution(),
		choose_symbol_centers_in_memory(),
		form_Clusters_in_memory(), 
		distribute_synapses_between_clusters_in_memory(),
		distribute_synapses_between_memories(),
		generate_node_degree_distribution();
	double	calculate_clustering_coefficient();

	FILE	*pos_centers_sens_mem, *pos_centers_decl_mem;

	double	diffcoord1, diffcoord2, dist2, weight, clustering_coefficient;
	int	i, j, k, centerneuron, loop_counter, 
		total_num_synapses, num_inter_cluster_synapses,
		num_inter_memory_synapses;
	int 	*symbol_center_sens_mem, *symbol_center_decl_mem; 


/*	if(output_to_monitor)
	  printf("\n\t PERCENT_ERROR_ENERGY: \t %1.12f \t\t MIN_PERCENT_WEIGHTS: %1.12f\n", 
		PERCENT_ERROR_ENERGY, MIN_PERCENT_WEIGHTS);
*/

	for (i = 0; i < n_nodes; i++)
		for (j = i; j < n_nodes; j++)
		{
			w(i,j) = BOLTZNULL;
			w(j,i) = BOLTZNULL;
		}
			


        /*   Gaussian distribution of synapses in sensorial memory    */
	/*     upper-left corner of matrix: sensorial memory  */  

	distribute_synapses_in_memory_by_gaussian_distribution(0, n_memory1-1);

        /*   Gaussian distribution of synapses in declarative memory    */  
	/*     lower-right corner of matrix: declarative memory */

	distribute_synapses_in_memory_by_gaussian_distribution(n_memory1, n_nodes-1);

	output_neighbor_lists(before_clustering);

	if (n_symbol_centers < 1) 
          gerror ("No symbol centers to form clusters in memories. In allocate_synapses_and_weights_in_memories(),\
                     (boltz_functs.c).");


       /*   Choose symbol centers in sensorial memory    */  

        symbol_center_sens_mem = (int *) malloc( n_symbol_centers*sizeof(int) );
        check_allocation( (void *)symbol_center_sens_mem,
                      "allocate_synapses_and_weights_in_memories()", "symbol_center_sens_mem");

	pos_centers_sens_mem = fopen("positions_centers_sensorial_memory", "w");
	choose_symbol_centers_in_memory(symbol_center_sens_mem, 0, n_memory1 - 1, pos_centers_sens_mem);
	fclose(pos_centers_sens_mem);

      /*   Choose symbol centers in declarative memory    */  

        symbol_center_decl_mem = (int *) malloc( n_symbol_centers*sizeof(int) );
        check_allocation( (void *)symbol_center_decl_mem,
                      "allocate_synapses_and_weights_in_memories()", "symbol_center_decl_mem");

	pos_centers_decl_mem = fopen("positions_centers_declarative_memory", "w");
	choose_symbol_centers_in_memory(symbol_center_decl_mem, n_memory1, n_nodes - 1, pos_centers_decl_mem);
	fclose(pos_centers_decl_mem);


      /*   Formation of clusters in sensorial memory    */

	form_Clusters_in_memory(symbol_center_sens_mem);  

     /*   Formation of clusters in declarative memory    */  

	form_Clusters_in_memory(symbol_center_decl_mem);

	output_neighbor_lists(clustering_within_memories);


/*	generate_node_degree_distribution();
*/

	total_num_synapses = 0;

	for ( i = 0; i < n_nodes; i++) 
	  total_num_synapses = total_num_synapses + neuron[i]->num_neighbors;
	total_num_synapses = total_num_synapses / 2;
	num_inter_cluster_synapses = (int) (percent_total_synapses * total_num_synapses);
	num_inter_memory_synapses = (int) (percent_synapses_between_memories * total_num_synapses);


    /*   synapses connecting clusters in sensorial memory    */ 

	distribute_synapses_between_clusters_in_memory(symbol_center_sens_mem, num_inter_cluster_synapses);

    /*   synapses connecting clusters in sensorial memory    */ 

	distribute_synapses_between_clusters_in_memory(symbol_center_decl_mem, num_inter_cluster_synapses);

	output_neighbor_lists(long_range_synapses_in_memories);


    /*   synapses connecting clusters among memories    */ 

	distribute_synapses_between_memories(symbol_center_sens_mem, symbol_center_decl_mem, num_inter_memory_synapses);

	output_neighbor_lists(connections_among_memories);

	clustering_coefficient = calculate_clustering_coefficient();

  	fprintf(finalparamf, "\n\n\t clustering coefficient of initial topology = %f\n\n", 
                 clustering_coefficient);

}    /*  end allocate_synapses_and_weights_in_memories()   */







/*
********************************************************************************
Function that detects stored patterns (minimum energy global network states)
in the initial network topology (synapses and corresponding weights) generated 
in function initialize_boltzmann_machine_state(). Stores found patterns in FILE!!

called by:  initialize_boltzmann_machine_state()
arguments:
functions called:
	convert_bitstring_into_integer() <double>

     defined in file simboltz.c
	recover_stored_patterns_BM() <int *>
	recover_stored_patterns_GSA() <int *>
	check_allocation() <void>
     defined in random.c
	ran3()   <double>
     defined in check_point_found_patterns.c
	check_point_intermediate_found_patterns_file()  <void>  
	retrieve_intermediate_found_patterns_file()  <void>  

math lib function: fmod(x,y) <double> returns the remainder of x/y, with the sign of x.
           x, y and the returned value are of type double.
           If y is 0, the result depends on the C language implementation.

The expression "x % y" produces the remainder of x/y, and equals zero if y divides x
  exactly. x and y must be integer or long (not float or double).

global variables:
	num_trial_initial_patterns <double>: number of times new random patterns are
		tried for finding new minima before giving up, since a previous pattern
                was found.
	initial_seek_local_min_temperature <double>:
	alpha_ass_mem <double>:
	iseed <double>: 
local variables:
I/O conditions:
   The w+ option in the fopen function opens the file in read/write mode
     (opens as a text file) .
   function feof() != 0 if the end of the specified file is reached.
        It refers to the last line that was accessed by a read function (scanf() ).
   function rewind() sets file position pointer to initial position.
********************************************************************************
*/
void
find_network_stored_patterns(patternf_BM, patternf_GSA, freq_visited_pattern_BM, freq_visited_pattern_GSA)
FILE		*patternf_BM, *patternf_GSA, *freq_visited_pattern_BM, *freq_visited_pattern_GSA;
{
	
	double		convert_bitstring_into_integer();
	extern void  	check_point_intermediate_found_patterns_file(),
			retrieve_intermediate_found_patterns_file();

	extern int	num_trial_initial_patterns;

	int		i, j, k, *trial_pattern, *stored_pattern_BM, *stored_pattern_GSA,
			*allready_found_pattern, 
			num_tried_patterns = 0, equal_BM, equal_GSA, end_of_file_BM, end_of_file_GSA;
	long		num_patterns_BM, num_patterns_GSA, i_pattern, 
			max_patterns_in_file_BM = 1000, max_patterns_in_file_GSA = 1000;
	long		total_trial_patterns = 0;
	double  	*freq_pattern_BM, *freq_pattern_GSA, *freq_pattern_tmp, total_patterns;
	double 		double_nothing, nothing;


	if(recovering_from_check_point) 
		retrieve_intermediate_found_patterns_file(patternf_BM, patternf_GSA, &freq_pattern_BM, &freq_pattern_GSA,
		  &num_patterns_BM, &num_patterns_GSA, &max_patterns_in_file_BM, &max_patterns_in_file_GSA,
		  &total_trial_patterns);
	else
	{
		freq_pattern_BM = (double *) malloc( max_patterns_in_file_BM*sizeof(double) );
        	check_allocation( (void *)freq_pattern_BM,
                      "find_network_stored_patterns()", "freq_pattern_BM");

		freq_pattern_GSA = (double *) malloc( max_patterns_in_file_GSA*sizeof(double) );
	        check_allocation( (void *)freq_pattern_GSA,
                      "find_network_stored_patterns()", "freq_pattern_GSA");

		for (j = 0; j < max_patterns_in_file_GSA; j++)
		{
			freq_pattern_BM[j] = 0.0;
			freq_pattern_GSA[j] = 0.0;
		}

		num_patterns_BM = 0;
		num_patterns_GSA = 0;

	}    /* end else if (recovering_from_check_point) */


	if (BoltzMachine)
	{
	  rewind (patternf_BM);
	  fscanf(patternf_BM, "%lf", &nothing);	
	  end_of_file_BM = feof(patternf_BM);
	  rewind (patternf_BM);
	}
	
	if (GSA)
	{
	  rewind (patternf_GSA);
	  fscanf(patternf_GSA, "%lf", &nothing);	
	  end_of_file_GSA = feof(patternf_GSA);
	  rewind (patternf_GSA);
	}

	trial_pattern = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)trial_pattern,
                      "find_network_stored_patterns()", "trial_pattern");

        allready_found_pattern = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)allready_found_pattern,
                      "find_network_stored_patterns()", "allready_found_pattern");


	while ( num_tried_patterns < num_trial_initial_patterns )
	{
		for (j = 0; j < n_nodes; j++)
				trial_pattern[j] = (int) (ran3(&iseed) + 0.5);

		total_trial_patterns = total_trial_patterns + 1;
				
/*		fprintf(errorf, "\n\n\t %d trial_pattern: ", num_tried_patterns);
        	for (j = 0; j < n_nodes; j++)
        		fprintf (errorf, "\t %d", trial_pattern[j]);
        		
        	fprintf(errorf, "\n\t num_trial_pattern = %lf", 
        	    convert_bitstring_into_integer(trial_pattern, n_nodes) );
*/

		if (BoltzMachine)
		  stored_pattern_BM = recover_stored_patterns_BM(trial_pattern, 
				   initial_seek_local_min_temperature, alpha_ass_mem);
		if (GSA)
		  stored_pattern_GSA = recover_stored_patterns_GSA(trial_pattern, 
				   initial_seek_local_min_temperature, alpha_ass_mem);
				   
/*		fprintf(errorf, "\n\t stored_pattern: ");
        	for (j = 0; j < n_nodes; j++)
        		fprintf (errorf, "\t %d", stored_pattern[j]);

        	fprintf(errorf, "\n\t num_stored_pattern = %lf", 
        	    convert_bitstring_into_integer(stored_pattern, n_nodes) );
*/

		if (BoltzMachine)
		{
		  equal_BM = false;

		  i_pattern = 0;

		  while ( (end_of_file_BM == 0) && (equal_BM == false) )
		  {
			fscanf(patternf_BM, "%*c%lf", &nothing);
			end_of_file_BM = feof(patternf_BM);
			
			if (end_of_file_BM == 0)
			{
			   for (k = 0; k < n_nodes; k++)
			   {
				fscanf(patternf_BM, "%*c%d", &allready_found_pattern[k]);
			   }
			   fscanf(patternf_BM, "%*c%lf", &double_nothing);
                           fscanf(patternf_BM, "%*c");
			   equal_BM = true;
			   j = 0;

			  while (equal_BM == true && j < n_nodes)			
			  {
			    if ( stored_pattern_BM[j] == allready_found_pattern[j] )
				j = j + 1;
			    else equal_BM = false;
			  }  /*  end while for individual pattern in file  */

			  if (equal_BM) freq_pattern_BM[i_pattern] = freq_pattern_BM[i_pattern] + 1.0;
			  i_pattern = i_pattern + 1;
			
			}  /*  end if  */
			
		  }   /*  end while compares with patterns in file  */
		
		  if (equal_BM == false)
		  {
		    fprintf(patternf_BM, "\t%lf", convert_bitstring_into_integer(stored_pattern_BM, n_nodes) );
		    for (i = 0; i < n_nodes; i++)
		    { 
		    	fprintf(patternf_BM, " %d", stored_pattern_BM[i]);
		    }
	            fprintf(patternf_BM, " %lf", Energy (stored_pattern_BM));
                    fprintf(patternf_BM, "\n");

		    if(i_pattern >= max_patterns_in_file_BM)
		    {
			freq_pattern_tmp = (double *) malloc( (max_patterns_in_file_BM + 10)*sizeof(double) );
        		check_allocation( (void *)freq_pattern_tmp,
                      		"find_network_stored_patterns() BM", "freq_pattern_tmp");

			for (j = 0; j < max_patterns_in_file_BM; j++)
				freq_pattern_tmp[j] = freq_pattern_BM[j];

			for (j = max_patterns_in_file_BM; j < max_patterns_in_file_BM + 10; j++)
				freq_pattern_tmp[j] = 0.0;

			free (freq_pattern_BM);
			
			freq_pattern_BM  = freq_pattern_tmp;
			max_patterns_in_file_BM = max_patterns_in_file_BM + 10;

		    } /* end if  */

		    freq_pattern_BM[i_pattern] = freq_pattern_BM[i_pattern] + 1.0;
		    num_patterns_BM = num_patterns_BM + 1;
		    num_tried_patterns = 0;
		  }  /* end if  */

		}  /* end if BoltzMachine  */


		if (GSA)
		{
		  equal_GSA = false;

		  i_pattern = 0;

		  while ( (end_of_file_GSA == 0) && (equal_GSA == false) )
		  {
			fscanf(patternf_GSA, "%*c%lf", &nothing);
			end_of_file_GSA = feof(patternf_GSA);
			
			if (end_of_file_GSA == 0)
			{
			   for (k = 0; k < n_nodes; k++)
			   {
				fscanf(patternf_GSA, "%*c%d", &allready_found_pattern[k]);
			   }
			   fscanf(patternf_GSA, "%*c%lf", &double_nothing);
                           fscanf(patternf_GSA, "%*c");

			   equal_GSA = true;
			   j = 0;

			   while (equal_GSA == true && j < n_nodes)			
			   {
			     if ( stored_pattern_GSA[j] == allready_found_pattern[j] )
				j = j + 1;
			     else equal_GSA = false;
			   }  /*  end while for individual pattern in file  */

			   if (equal_GSA) freq_pattern_GSA[i_pattern] = freq_pattern_GSA[i_pattern] + 1.0;
			   i_pattern = i_pattern + 1;

			}  /*  end if  */

			
		  }   /*  end while compares with patterns in file  */
		
		  if (equal_GSA == false)
		  {
		    fprintf (patternf_GSA, "\t%lf", convert_bitstring_into_integer(stored_pattern_GSA, n_nodes) );
		    for (i = 0; i < n_nodes; i++)
		    { 
		    	fprintf (patternf_GSA, " %d", stored_pattern_GSA[i]);
		    }
	            fprintf (patternf_GSA, " %lf", Energy (stored_pattern_GSA));
                    fprintf (patternf_GSA, "\n");

		    if(i_pattern >= max_patterns_in_file_GSA)
		    {
			freq_pattern_tmp = (double *) malloc( (max_patterns_in_file_GSA + 10)*sizeof(double) );
        		check_allocation( (void *)freq_pattern_tmp,
                      		"find_network_stored_patterns() GSA", "freq_pattern_tmp");

			for (j = 0; j < max_patterns_in_file_GSA; j++)
				freq_pattern_tmp[j] = freq_pattern_GSA[j];

			for (j = max_patterns_in_file_GSA; j < max_patterns_in_file_GSA + 10; j++)
				freq_pattern_tmp[j] = 0.0;

			free (freq_pattern_GSA);
			
			freq_pattern_GSA  = freq_pattern_tmp;
			max_patterns_in_file_GSA = max_patterns_in_file_GSA + 10;

		    } /* end if  */

		    freq_pattern_GSA[i_pattern] = freq_pattern_GSA[i_pattern] + 1.0;
		    num_patterns_GSA = num_patterns_GSA + 1;

/*                    if (fmod((double)num_patterns_GSA, (double)num_found_patterns_between_checkpoints) == 0.0)  */
                    if ( (num_patterns_GSA % num_found_patterns_between_checkpoints) == 0)
                      check_point_intermediate_found_patterns_file(patternf_BM, patternf_GSA, freq_pattern_BM, freq_pattern_GSA,
                            num_patterns_BM, num_patterns_GSA, max_patterns_in_file_BM, max_patterns_in_file_GSA,
			    total_trial_patterns);

		    num_tried_patterns = 0;
		  }  /* end if  */

		}  /* end if GSA  */


		if (equal_BM && equal_GSA) num_tried_patterns = num_tried_patterns + 1;
		
		rewind (patternf_BM);
		end_of_file_BM = feof(patternf_BM);
		rewind (patternf_GSA);
		end_of_file_GSA = feof(patternf_GSA);

		free(stored_pattern_BM);
		free(stored_pattern_GSA);

	} /*  end while search criterion (can't find new patterns)  */

	total_patterns = 0.0;
	if (BoltzMachine)
	{ 
	  for (j = 0; j < max_patterns_in_file_BM; j++)
	  {
		total_patterns = total_patterns + freq_pattern_BM[j];
 		fprintf(freq_visited_pattern_BM, "\t%d\t%.20lf\n", j, freq_pattern_BM[j] / (double) total_trial_patterns);
	  }
	  fprintf(freq_visited_pattern_BM, "\t total_trial_patterns: %ld\t total expected patterns: %lf\t num_patterns_BM: %ld\n", 
                       total_trial_patterns, total_patterns, num_patterns_BM);
	}

	total_patterns = 0.0;
	if (GSA)
	{ 
	  for (j = 0; j < max_patterns_in_file_GSA; j++)
	  {
		total_patterns = total_patterns + freq_pattern_GSA[j];
		fprintf(freq_visited_pattern_GSA, "\t%d\t%.20lf\n", j, freq_pattern_GSA[j] / (double) total_trial_patterns);
	  }
	  fprintf(freq_visited_pattern_GSA, "\t total_trial_patterns: %ld\t total expected patterns: %lf\t num_patterns_GSA: %ld\n", 
                       total_trial_patterns, total_patterns, num_patterns_GSA);
	}


/*	print_text ("Found network stored patterns!!!", 0.0, 0);
	
	getchar();
*/

	free(trial_pattern);
	free(allready_found_pattern);
	free(freq_pattern_BM);
	free(freq_pattern_GSA);

}  /*  end find_network_stored_patterns()  */






/*
********************************************************************************
Function that detects stored patterns (minimum energy global network states)
in the initial network topology (synapses and corresponding weights) generated 
in function initialize_boltzmann_machine_state(). Stores found patterns in MEMORY!!

called by:  initialize_boltzmann_machine_state()
arguments:
functions called:
	convert_bitstring_into_integer() <double>
     defined in file simboltz.c
	recover_stored_patterns_BM() <int *>
	recover_stored_patterns_GSA() <int *>
	check_allocation() <void>
     defined in check_point_found_patterns.c
	check_point_intermediate_found_patterns_memory()  <void>
	retrieve_intermediate_found_patterns_memory()   <void>
     defined in random.c
	ran3()   <double>
     defined in check_point_found_patterns.c
	check_point_intermediate_found_patterns_memory()  <void>  
	retrieve_intermediate_found_patterns_memory()  <void>  

math lib function: fmod(x,y) <double> returns the remainder of x/y, with the sign of x.
           x, y and the returned value are of type double.
           If y is 0, the result depends on the C language implementation.


The expression "x % y" produces the remainder of x/y, and equals zero if y divides x
  exactly. x and y must be integer or long (not float or double).


global variables:
	num_trial_initial_patterns <double>: number of times new random patterns are
		tried for finding new minima before giving up, since a previous pattern
                was found.
	initial_seek_local_min_temperature <double>:
	alpha_ass_mem <double>:
	iseed <double>: 
local variables:
I/O conditions:
   The w+ option in the fopen function opens the file in read/write mode
     (opens as a text file) .
   function feof() != 0 if the end of the specified file is reached.
   function rewind() sets file position pointer to initial position.
********************************************************************************
*/
void
find_network_stored_patterns_use_memory(patternf_BM, patternf_GSA, freq_visited_pattern_BM, freq_visited_pattern_GSA)
FILE		*patternf_BM, *patternf_GSA, *freq_visited_pattern_BM, *freq_visited_pattern_GSA;
{
	
	double		convert_bitstring_into_integer();
	extern void  	check_point_intermediate_found_patterns_memory(),
			retrieve_intermediate_found_patterns_memory();

	extern int	num_trial_initial_patterns;

	int		i, j, *trial_pattern, *stored_pattern_BM, *stored_pattern_GSA, 
			num_tried_patterns = 0, equal_BM, equal_GSA;
	long		num_patterns_BM, num_patterns_GSA, i_pattern, 
			max_patterns_in_file_BM = 1000, max_patterns_in_file_GSA = 1000;
	long		total_trial_patterns = 0;
	double  	*freq_pattern_BM, *freq_pattern_GSA, *freq_pattern_tmp, total_patterns;
        pattern_list_inf  *BM_pattern_list, *GSA_pattern_list, 
                          *current_pattern, *previous_pattern, *new_pattern;


	trial_pattern = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)trial_pattern,
                      "find_network_stored_patterns_use_memory()", "trial_pattern");

	if(recovering_from_check_point) 
		retrieve_intermediate_found_patterns_memory(&BM_pattern_list, &GSA_pattern_list, &freq_pattern_BM, &freq_pattern_GSA,
		  &num_patterns_BM, &num_patterns_GSA, &max_patterns_in_file_BM, &max_patterns_in_file_GSA,
		  &total_trial_patterns);
	else
	{
		freq_pattern_BM = (double *) malloc( max_patterns_in_file_BM*sizeof(double) );
        	check_allocation( (void *)freq_pattern_BM,
                      "find_network_stored_patterns_use_memory()", "freq_pattern_BM");

		freq_pattern_GSA = (double *) malloc( max_patterns_in_file_GSA*sizeof(double) );
        	check_allocation( (void *)freq_pattern_GSA,
                      "find_network_stored_patterns_use_memory()", "freq_pattern_GSA");

		for (j = 0; j < max_patterns_in_file_GSA; j++)
		{
			freq_pattern_BM[j] = 0.0;
			freq_pattern_GSA[j] = 0.0;
		}

		num_patterns_BM = 0;
		num_patterns_GSA = 0;

		BM_pattern_list = NO_PATTERN; 
        	GSA_pattern_list = NO_PATTERN;

	}    /* end else if (recovering_from_check_point) */


	while ( num_tried_patterns < num_trial_initial_patterns )
	{
		for (j = 0; j < n_nodes; j++)
				trial_pattern[j] = (int) (ran3(&iseed) + 0.5);

		total_trial_patterns = total_trial_patterns + 1;
				
/*		fprintf(errorf, "\n\n\t %d trial_pattern: ", num_tried_patterns);
        	for (j = 0; j < n_nodes; j++)
        		fprintf (errorf, "\t %d", trial_pattern[j]);
        		
        	fprintf(errorf, "\n\t num_trial_pattern = %lf", 
        	    convert_bitstring_into_integer(trial_pattern, n_nodes) );
*/

		if (BoltzMachine)
		  stored_pattern_BM = recover_stored_patterns_BM(trial_pattern, 
				   initial_seek_local_min_temperature, alpha_ass_mem);
		if (GSA)
		  stored_pattern_GSA = recover_stored_patterns_GSA(trial_pattern, 
				   initial_seek_local_min_temperature, alpha_ass_mem);
				   
/*		fprintf(errorf, "\n\t stored_pattern: ");
        	for (j = 0; j < n_nodes; j++)
        		fprintf(errorf, "\t %d", stored_pattern[j]);

        	fprintf(errorf, "\n\t num_stored_pattern = %lf", 
        	    convert_bitstring_into_integer(stored_pattern, n_nodes) );
*/

		if (BoltzMachine)
		{
		  equal_BM = false;

		  i_pattern = 0;

		  current_pattern = BM_pattern_list;

		  previous_pattern = BM_pattern_list;

		  while ( (current_pattern != NO_PATTERN) && (equal_BM == false) )
		  {
			 equal_BM = true;
			 j = 0;

			 while (equal_BM == true && j < n_nodes)			
			 {
			    if ( stored_pattern_BM[j] == current_pattern->pattern->pattern_bits[j] )
				j = j + 1;
			    else equal_BM = false;
			 }  /*  end while for individual found pattern */

			 if (equal_BM) freq_pattern_BM[i_pattern] = freq_pattern_BM[i_pattern] + 1.0;
			 i_pattern = i_pattern + 1;

                         previous_pattern = current_pattern;

			 current_pattern = current_pattern->next_pattern;

		  }   /*  end while compares with found patterns  */

		  if (equal_BM == false)
		  {

		    new_pattern = (pattern_list_inf *) malloc( sizeof(pattern_list_inf) );
                    check_allocation( (void *)new_pattern,
                                      "find_network_stored_patterns_use_memory() BM", "new_pattern");

		    new_pattern->pattern = (pattern_inf *) malloc( sizeof(pattern_inf) );
                    check_allocation( (void *)new_pattern->pattern,
                                      "find_network_stored_patterns_use_memory() BM", "new_pattern->pattern");

		    new_pattern->pattern->pattern_bits = (int *) malloc( n_nodes*sizeof(int) );
                    check_allocation( (void *)new_pattern->pattern->pattern_bits,
                                      "find_network_stored_patterns_use_memory() BM", "new_pattern->pattern->pattern_bits");

		    new_pattern->pattern->pattern_ID = convert_bitstring_into_integer(stored_pattern_BM, n_nodes);
		    for (i = 0; i < n_nodes; i++)
			new_pattern->pattern->pattern_bits[i] = stored_pattern_BM[i];
		    new_pattern->pattern->energy_pattern = Energy(stored_pattern_BM);

		    new_pattern->next_pattern = NO_PATTERN;

		    if (BM_pattern_list == NO_PATTERN) BM_pattern_list = new_pattern;
		    else previous_pattern->next_pattern = new_pattern;

		    if(i_pattern >= max_patterns_in_file_BM)
		    {
			freq_pattern_tmp = (double *) malloc( (max_patterns_in_file_BM + 10)*sizeof(double) );
        		check_allocation( (void *)freq_pattern_tmp,
                      		"find_network_stored_patterns_use_memory() BM", "freq_pattern_tmp");

			for (j = 0; j < max_patterns_in_file_BM; j++)
				freq_pattern_tmp[j] = freq_pattern_BM[j];

			for (j = max_patterns_in_file_BM; j < max_patterns_in_file_BM + 10; j++)
				freq_pattern_tmp[j] = 0.0;

			free (freq_pattern_BM);
			
			freq_pattern_BM  = freq_pattern_tmp;
			max_patterns_in_file_BM = max_patterns_in_file_BM + 10;

		    } /* end if  */

		    freq_pattern_BM[i_pattern] = freq_pattern_BM[i_pattern] + 1.0;
		    num_patterns_BM = num_patterns_BM + 1;
		    num_tried_patterns = 0;

		  }  /* end if (equal_BM == false) */ 


		}  /* end if BoltzMachine  */


		if (GSA)
		{
		  equal_GSA = false;

		  i_pattern = 0;

		  current_pattern = GSA_pattern_list;

		  previous_pattern = GSA_pattern_list;

		  while ( (current_pattern != NO_PATTERN) && (equal_GSA == false) )
		  {
			equal_GSA = true;
			j = 0;

			while (equal_GSA == true && j < n_nodes)			
			{
			     if ( stored_pattern_GSA[j] == current_pattern->pattern->pattern_bits[j] )
				j = j + 1;
			     else equal_GSA = false;
			}  /*  end while for individual found pattern */

			if (equal_GSA) freq_pattern_GSA[i_pattern] = freq_pattern_GSA[i_pattern] + 1.0;
			i_pattern = i_pattern + 1;

                        previous_pattern = current_pattern;

			current_pattern = current_pattern->next_pattern;
			
		  }   /*  end while compares with found patterns */
		
		  if (equal_GSA == false)
		  {
		    new_pattern = (pattern_list_inf *) malloc( sizeof(pattern_list_inf) );
                    check_allocation( (void *)new_pattern,
                                      "find_network_stored_patterns_use_memory() GSA", "new_pattern");

		    new_pattern->pattern = (pattern_inf *) malloc( sizeof(pattern_inf) );
                    check_allocation( (void *)new_pattern->pattern,
                                      "find_network_stored_patterns_use_memory() GSA", "new_pattern->pattern");

		    new_pattern->pattern->pattern_bits = (int *) malloc( n_nodes*sizeof(int) );
                    check_allocation( (void *)new_pattern->pattern->pattern_bits,
                                      "find_network_stored_patterns_use_memory() GSA", "new_pattern->pattern->pattern_bits");

		    new_pattern->pattern->pattern_ID = convert_bitstring_into_integer(stored_pattern_GSA, n_nodes);
		    for (i = 0; i < n_nodes; i++)
			new_pattern->pattern->pattern_bits[i] = stored_pattern_GSA[i];
		    new_pattern->pattern->energy_pattern = Energy(stored_pattern_GSA);

		    new_pattern->next_pattern = NO_PATTERN;

		    if (GSA_pattern_list == NO_PATTERN) GSA_pattern_list = new_pattern;
		    else previous_pattern->next_pattern = new_pattern;


		    if(i_pattern >= max_patterns_in_file_GSA)
		    {
			freq_pattern_tmp = (double *) malloc( (max_patterns_in_file_GSA + 10)*sizeof(double) );
        		check_allocation( (void *)freq_pattern_tmp,
                      		"find_network_stored_patterns_use_memory() GSA", "freq_pattern_tmp");

			for (j = 0; j < max_patterns_in_file_GSA; j++)
				freq_pattern_tmp[j] = freq_pattern_GSA[j];

			for (j = max_patterns_in_file_GSA; j < max_patterns_in_file_GSA + 10; j++)
				freq_pattern_tmp[j] = 0.0;

			free (freq_pattern_GSA);
			
			freq_pattern_GSA  = freq_pattern_tmp;
			max_patterns_in_file_GSA = max_patterns_in_file_GSA + 10;

		    } /* end if  */

		    freq_pattern_GSA[i_pattern] = freq_pattern_GSA[i_pattern] + 1.0;
		    num_patterns_GSA = num_patterns_GSA + 1;

/*                    if (fmod((double)num_patterns_GSA, (double)num_found_patterns_between_checkpoints) == 0.0)  */
                    if ( (num_patterns_GSA % num_found_patterns_between_checkpoints) == 0)
                          check_point_intermediate_found_patterns_memory(BM_pattern_list, GSA_pattern_list, freq_pattern_BM, freq_pattern_GSA,
                            num_patterns_BM, num_patterns_GSA, max_patterns_in_file_BM, max_patterns_in_file_GSA,
			    total_trial_patterns);

		    num_tried_patterns = 0;

		  }  /* end if (equal_GSA == false) */

		}  /* end if GSA  */


		if (equal_BM && equal_GSA) num_tried_patterns = num_tried_patterns + 1;

		free(stored_pattern_BM);
		free(stored_pattern_GSA);


	} /*  end while search criterion (can't find new patterns)  */

	total_patterns = 0.0;
	if (BoltzMachine)
	{ 
	  for (j = 0, current_pattern = BM_pattern_list; current_pattern != NO_PATTERN; j++)
	  {
		fprintf(patternf_BM, "\t%lf", current_pattern->pattern->pattern_ID );
		for (i = 0; i < n_nodes; i++)
		{ 
		    fprintf(patternf_BM, " %d", current_pattern->pattern->pattern_bits[i]);
		}
	        fprintf(patternf_BM, " %lf", current_pattern->pattern->energy_pattern);
                fprintf(patternf_BM, "\n");

		total_patterns = total_patterns + freq_pattern_BM[j];
 		fprintf(freq_visited_pattern_BM, "\t%d\t%.20lf\n", j, freq_pattern_BM[j] / (double) total_trial_patterns);

		previous_pattern = current_pattern;

		current_pattern = current_pattern->next_pattern;

		free(previous_pattern->pattern->pattern_bits);
		free(previous_pattern->pattern);
		free(previous_pattern);
	  }
	  fprintf(freq_visited_pattern_BM, "\t total_trial_patterns: %ld\t total expected patterns: %lf\t num_patterns_BM: %ld\n", 
                       total_trial_patterns, total_patterns, num_patterns_BM);
	}

	total_patterns = 0.0;
	if (GSA)
	{ 
	  for (j = 0, current_pattern = GSA_pattern_list; current_pattern != NO_PATTERN; j++)
	  {
		fprintf(patternf_GSA, "\t%lf", current_pattern->pattern->pattern_ID );
		for (i = 0; i < n_nodes; i++)
		{ 
		    fprintf(patternf_GSA, " %d", current_pattern->pattern->pattern_bits[i]);
		}
	        fprintf(patternf_GSA, " %lf", current_pattern->pattern->energy_pattern);
                fprintf(patternf_GSA, "\n");

		total_patterns = total_patterns + freq_pattern_GSA[j];
		fprintf(freq_visited_pattern_GSA, "\t%d\t%.20lf\n", j, freq_pattern_GSA[j] / (double) total_trial_patterns);

		previous_pattern = current_pattern;

		current_pattern = current_pattern->next_pattern;

		free(previous_pattern->pattern->pattern_bits);
		free(previous_pattern->pattern);
		free(previous_pattern);
	  }
	  fprintf(freq_visited_pattern_GSA, "\t total_trial_patterns: %ld\t total expected patterns: %lf\t num_patterns_GSA: %ld\n", 
                       total_trial_patterns, total_patterns, num_patterns_GSA);
	}


/*	print_text ("Found network stored patterns!!!", 0.0, 0);
	
	getchar();
*/

	free(trial_pattern);
	free(freq_pattern_BM);
	free(freq_pattern_GSA);

}  /*  end find_network_stored_patterns_use_memory()  */




/*
********************************************************************************
Generates a network state S' (a pattern vector), from an original state S, by 
changing (flipping) a randomly chosen neuron's state, whose state has not yet 
been changed.

called by: recover_stored_patterns_BM()			(simboltz.c)
	   recover_stored_patterns_GSA()                (simboltz.c)
	   simulate_psychoanalytical_working_through()	(simboltz.c)
arguments:
	int *old_state, *state_changed, *sign, *new_state, first_node, last_node;
functions called: none
global variables:
local variables: k <int>: integer counter
I/O conditions: returns identity (position in vector) of neuron whose state 
                changed.
********************************************************************************
*/
int
generate_new_state(old_state, state_changed, sign, 
		   new_state, first_node, last_node)
int	*old_state, *state_changed, *sign, *new_state, first_node, last_node;
{
	int	k, num_nodes;

	num_nodes = last_node - first_node + 1;

/*	k = (int)( first_node + ran3(&iseed) * (last_node - first_node) + 0.5 );
*/
	k = first_node + (int)( ran3(&iseed) * num_nodes );
	if (k == last_node + 1) k = last_node;

	while (state_changed[k] == 1)
	{
		k = first_node + (int)( ran3(&iseed) * num_nodes );
	  	if (k == last_node + 1) k = last_node;
	}
/*	  k = (int)( first_node + ran3(&iseed) * (last_node - first_node) + 0.5 );
*/
	state_changed[k] = 1;

	if(old_state[k] == 1) 
	{	
		*new_state = 0;
		*sign = +1;
	}
        else 
	{
		*new_state = 1;
		*sign = -1;
	}

	return(k);

}  /*  end generate_new_state()  */







/*
********************************************************************************
Function that changes synaptic weights in Hebbian adaptive learning process.

called by: simulate_psychoanalytical_working_through
arguments: neuron_state, i, j, changed
functions called:
	insert_second_neuron_in_neighbor_list_of_first() <double *>  (simboltz.c)
	remove_second_neuron_from_neighbor_list_of_first() (simboltz.c)
global variables:
   inhibitive_adaptive_learning <int = 0 or 1>: if equal to 1, allows 
           learning even when neuron states are in {0,1} representation.
   w(i,j) <double>: pointer to weight value of synapse that connects neuron j to neuron i.
local variables:
I/O conditions:
********************************************************************************
*/
bool
reinforce_connections_to_affected_neuron(neuron_state, i, j, changed)
int	*neuron_state, i, j, changed;
{
   extern void remove_second_neuron_from_neighbor_list_of_first();
   extern double *insert_second_neuron_in_neighbor_list_of_first();


   double	delta_w = 0.0, w_ij, abs_wij, sign;
   int		neuron_i, neuron_j;
   neighbor_inf *np, *current_np;
   bool		changed_now = false;


   if (inhibitive_adaptive_learning)
   {
     if (neuron_state[i] == 0) neuron_i = -1;
     else neuron_i = 1;

     if (neuron_state[j] == 0) neuron_j = -1;
     else neuron_j = 1;
   }
   else
   {
     neuron_i = neuron_state[i];
     neuron_j = neuron_state[j];
   }

   delta_w = learning_factor * neuron_i * neuron_j * w_max;
   if (w(i,j) == BOLTZNULL) w_ij = delta_w;
   else w_ij = *w(i,j) + delta_w;

   if (w_ij < 0.0) sign = -1.0;
   else sign = 1.0;

   abs_wij = fabs(w_ij);

   if (w(i,j) == BOLTZNULL)
   {
     if (abs_wij > w_min)
     {
	w(i,j) = insert_second_neuron_in_neighbor_list_of_first(i, j, w_ij);
	w(j,i) = insert_second_neuron_in_neighbor_list_of_first(j, i, w_ij);
	changed_now = true;
     }
     else changed_now = false;
   }
   else   /*  (w(i,j) != BOLTZNULL)   */
   { 
     if ( abs_wij < w_min )
     {
       np = neuron[i]->list_neighs;
       while ( np != NO_NEIGHBOR )
       {
	 current_np = np;
	 np = np->next_neigh;
	 if (current_np->neigh_ID == j)
	 {
		remove_second_neuron_from_neighbor_list_of_first(i, j, current_np);
		break;
	 }
       }  /* end while */ 

       np = neuron[j]->list_neighs;
       while ( np != NO_NEIGHBOR )
       {
	 current_np = np;
	 np = np->next_neigh;
	 if (current_np->neigh_ID == i)
	 {
		remove_second_neuron_from_neighbor_list_of_first(j, i, current_np);
		break;
	 }
       }  /* end while */
     }  /*  end if  */
     else
     {
       if (abs_wij < w_max) *w(i,j) = w_ij;
       else *w(i,j) = sign * w_max;

       *w(j,i) = *w(i,j);
     }
     changed_now = true;
   }  /* end else  */


   if ( !changed_now && !changed) return(false); 
   else return(true);
   
}  /*  end reinforce_connections_to_affected_neuron()  */




/*
********************************************************************************
Allocates synapses with respective weights among pairs of neurons according
to Gaussian distribution of spatial distance between the pair.

called by:  allocate_synapses_and_weights_in_memories()
	    connectivity_distribution()
arguments:
functions called:
	insert_second_neuron_in_neighbor_list_of_first() <double *> (in simboltz.c)
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void
distribute_synapses_in_memory_by_gaussian_distribution(first_neuron, last_neuron)
int	first_neuron, last_neuron;
{

	extern double *insert_second_neuron_in_neighbor_list_of_first();

	int	i, j;
	double	diffcoord1, diffcoord2, dist2, gaussprob, weight;


	last_neuron = last_neuron + 1;

	for (i = first_neuron; i < last_neuron; i++)  
	{
		w(i,i) = BOLTZNULL;
		for (j = i+1; j < last_neuron; j++)
		{
/*		   w(i,j) = BOLTZNULL;
		   w(j,i) = BOLTZNULL;      */
		   diffcoord1 = neuron[i]->pos_coord1 - neuron[j]->pos_coord1;
		   diffcoord2 = neuron[i]->pos_coord2 - neuron[j]->pos_coord2;
		   dist2 = (diffcoord1 * diffcoord1) + (diffcoord2 * diffcoord2);
		   gaussprob = ( 1/(2*pi*sigma2) ) * exp ((-dist2) / (2*sigma2));
		   if (ran3(&iseed) < gaussprob)
		   {
		     weight = gaussprob * w_max;
		     if (ran3(&iseed) < 0.5) weight = -weight;
		     if (fabs(weight) > w_min)
		     {
		     	w(i,j) = insert_second_neuron_in_neighbor_list_of_first(i, j, weight);
		     	w(j,i) = insert_second_neuron_in_neighbor_list_of_first(j, i, weight);
		     }
		   }
		}   /*  end for  */
	}   /*  end for  */

}   /*   end distribute_synapses_in_memory_by_gaussian_distribution()   */






/*
********************************************************************************
Chooses spatial coordinates of centers of image of symbols stored in memories.
It is supposed that these are originally uniformly distributed and formed by a
self-organizing map. 

called by:
	allocate_synapses_and_weights_in_memories()
	connectivity_distribution()
arguments:
functions called:
	ran3()
	gerror()

global variables:
	parameter_distance_between_centers <double>: multiplicative factor to 
         adjust mean distance between cluster centers, calculated based on sigma
         obtained from network storage capacity. This is phenomenological and 
         artificial. Ideal value should be 1.0.
local variables:
I/O conditions:  
********************************************************************************
*/
void
choose_symbol_centers_in_memory(symbol_center, first_neuron, last_neuron, positionf)
int 	*symbol_center, first_neuron, last_neuron;
FILE	*positionf;
{
     double	diffcoord1, diffcoord2, dist2, least_dist2;
     int	i, j, k, center_neuron, loop_counter, max_counter, choose_new_center,
		number_symbol_set_choice_trials, num_neurons;


     least_dist2 = parameter_distance_between_centers * parameter_distance_between_centers * sigma2;

     max_counter = (last_neuron - first_neuron + 1) * 1000;

     num_neurons = last_neuron - first_neuron + 1;

     choose_new_center = true;

     number_symbol_set_choice_trials = 0;

     while (choose_new_center && number_symbol_set_choice_trials < max_counter)
     {
        for (i = 0; i < n_symbol_centers; i++) symbol_center[i] = -1;

	k = first_neuron + (int)( ran3(&iseed) * num_neurons);
	if (k == last_neuron + 1) k = last_neuron;
	
	symbol_center[0] = k;
	choose_new_center = false;

	for (i = 1; i < n_symbol_centers && !choose_new_center; i++)
	{
	     loop_counter = 0;

	     while (symbol_center[i] == -1 && loop_counter < max_counter)
	     {
		  k = first_neuron + (int)( ran3(&iseed) * num_neurons);
		  if (k == last_neuron + 1) k = last_neuron;

		  choose_new_center = false;
		  for (j = 0; j < i && !choose_new_center; j++)
		  {
			center_neuron = symbol_center[j];
			if (k == center_neuron) choose_new_center = true;
			else
			{
			  diffcoord1 = neuron[k]->pos_coord1 - neuron[center_neuron]->pos_coord1;
			  diffcoord2 = neuron[k]->pos_coord2 - neuron[center_neuron]->pos_coord2;
			  dist2 = (diffcoord1 * diffcoord1) + (diffcoord2 * diffcoord2);
			  if (dist2 < least_dist2) choose_new_center = true;
			} 
		  }
		  if (!choose_new_center) symbol_center[i] = k;

		  loop_counter = loop_counter + 1;
		  
	     }   /*  end while   */

	}   /*  end for  */

        number_symbol_set_choice_trials = number_symbol_set_choice_trials + 1;

     }   /*  end while   */

     for (i = 0; i < n_symbol_centers; i++)
     {
	if (symbol_center[i] == -1) 
	 gerror ("Could not choose all symbol centers requested. In choose_symbol_centers_in_memory(), (boltz_functs.c).");
	else fprintf(positionf, " %lf \t %lf \n", 
			neuron[symbol_center[i]]->pos_coord1, neuron[symbol_center[i]]->pos_coord2);
     }

}    /*   end choose_symbol_centers_in_memory()   */






/*
********************************************************************************
Function which redistributes weight intensities in such a way as to form
clusters in each memory. Takes synaptic weights from weak (more distant)
synapses and adds to strong (closer) synapses. This is a kind of preferential
attachment with conservation of energy. 

called by:
	allocate_synapses_and_weights_in_memories()
	connectivity_distribution()
arguments:
functions called:
	remove_second_neuron_from_neighbor_list_of_first() (in simboltz.c)
	nint(x) is defined by nint(x) = i, where i is an integer and x 
		is in the interval [i-0.5, i+0.5].
global variables:
	n_symbol_centers <int>:
	percent_neighbor_loss <double>: 
	cluster_formation_learning_factor <double>: 
local variables:
I/O conditions:  
********************************************************************************
*/
void
form_Clusters_in_memory(symbol_center)
int 	*symbol_center;
{
	extern void remove_second_neuron_from_neighbor_list_of_first();

/*	extern int	nint();  */

	int 	i, j, l, m, center_neuron, *clustered, not_clustered, all_clustered,
		min_num_neighbors, not_removed_neighbor;
	double 	sum_neigh_weights, sum_neigh_weights2, weight_proportion, delta, delta_neigh,
		sign;
	neighbor_inf 	*np, *np2, *prev_np, *np3, *current_np3;

	not_clustered = true;

        clustered = (int *) malloc( n_symbol_centers*sizeof(int) );
        check_allocation( (void *)clustered,
                      "form_Clusters_in_memory()", "clustered");

	for (i = 0; i < n_symbol_centers; i++) clustered[i] = false;

	while (not_clustered)
	{
	  for (i = 0; i < n_symbol_centers; i++)
	  {
	     center_neuron = symbol_center[i];

	     if (neuron[center_neuron]->num_neighbors <= 1 || clustered[i]) clustered[i] = true;
	     else    /*  redistribute synapse strengths and form clusters */
	     {	
		min_num_neighbors = neuron[center_neuron]->num_neighbors - 
			( (int) (percent_neighbor_loss * neuron[center_neuron]->num_neighbors + 0.5) );

		if ( min_num_neighbors == neuron[center_neuron]->num_neighbors ) clustered[i] = true;
		else
	 	{
			sum_neigh_weights = 0.0;
			for (np = neuron[center_neuron]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)
	    			sum_neigh_weights = sum_neigh_weights + fabs(np->wij);
		}

		for (np = neuron[center_neuron]->list_neighs; np != NO_NEIGHBOR && !clustered[i]; np = np->next_neigh)
		{
		  l = np->neigh_ID;
		  weight_proportion = fabs(*w(center_neuron,l)) / sum_neigh_weights;
		  delta = cluster_formation_learning_factor * weight_proportion;
		  if (ran3(&iseed) < weight_proportion)
		  {
			sum_neigh_weights2 = sum_neigh_weights - fabs(*w(center_neuron,l));
			np2 = neuron[center_neuron]->list_neighs;
			while ( np2 != NO_NEIGHBOR && !clustered[i])
			{
			  m = np2->neigh_ID;
			  if (m != l)
			  {
				if ( neuron[center_neuron]->num_neighbors == 2 ) 
					delta_neigh = delta;
				else delta_neigh = ( 1.0 - (fabs(*w(center_neuron,m)) / sum_neigh_weights2) ) * delta;

				if (*w(center_neuron,m) < 0.0) sign = -1.0;
				else sign = 1.0;

				if ( delta_neigh > fabs(*w(center_neuron,m) ) ) *w(center_neuron,m) = sign * 0.1 * w_min;
				else *w(center_neuron,m) = *w(center_neuron,m) - sign * delta_neigh;

				*w(m,center_neuron) = *w(center_neuron,m);

				if (fabs(*w(center_neuron,m)) < w_min)
				{
				  if (*w(center_neuron,l) < 0.0) sign = -1.0;
				  else sign = 1.0;
				  *w(center_neuron,l) = *w(center_neuron,l) + sign * fabs(*w(center_neuron,m));
				  
				  if (fabs(*w(center_neuron,l)) > w_max)
					 *w(center_neuron,l) = sign * w_max;

				  *w(l,center_neuron) = *w(center_neuron,l);

				  prev_np = np2;
				  np2 = np2->next_neigh;
				  remove_second_neuron_from_neighbor_list_of_first(center_neuron, m, prev_np);
				  np3 = neuron[m]->list_neighs;
				  not_removed_neighbor = true;
				  while ( np3 != NO_NEIGHBOR && not_removed_neighbor )
				  {
				     current_np3 = np3;
				     np3 = np3->next_neigh;
				     if (current_np3->neigh_ID == center_neuron)
				     {
					remove_second_neuron_from_neighbor_list_of_first(m, center_neuron, current_np3);
				        not_removed_neighbor = false;
				     }
				  }
				  if ( neuron[center_neuron]->num_neighbors <= min_num_neighbors ) clustered[i] = true;

				}  /*  end if   synapse < minimum */

				else np2 = np2->next_neigh;

			  }    /*  end if  (m != l) */

			  else np2 = np2->next_neigh;

			}  /*  end while through other neighbors  */

			if (*w(center_neuron,l) < 0.0) sign = -1.0;
			else sign = 1.0;
			*w(center_neuron,l) = *w(center_neuron,l) + sign * delta;
			
			if (fabs(*w(center_neuron,l)) > w_max) 
					*w(center_neuron,l) = sign * w_max;
			*w(l,center_neuron) = *w(center_neuron,l);

		  }  /*  end if probability   */

		}   /* end for through neighbors of centers */ 

	     }  /*  end else redistribute synapse strengths and form clusters */

	  }   /*  end for through symbol centers  */

	  all_clustered = true;
	  for (i = 0; i < n_symbol_centers; i++) 
		if ( !clustered[i]) all_clustered = false;
	  if (all_clustered) not_clustered = false;

	}   /*  end while for cluster learning iterations  */


}   /*   end form_Clusters_in_memory()   */








/*
********************************************************************************
Converts a string of bits into a unique integer value. 

called by:
	recover_neurotic_states_as_associative_memory()
arguments:
functions called:
	pow() <double>
global variables:
local variables:
I/O conditions:  Long bitstrings may overflow long integer representation.
********************************************************************************
*/
double
convert_bitstring_into_integer (bitstring, num_bits)
int	*bitstring, num_bits;
{
	int  i;
	double integer_num;

	integer_num = 0;

	for (i = 0; i < num_bits; i++)
		integer_num = integer_num + (double) bitstring[i] * pow(2.0, (double) i);

	return (integer_num);

} /* end of convert_bitstring_into_integer */






/*
********************************************************************************
Distributes synapses between clusters in a memory module. These can be 
long-range synapses.

called by:
	allocate_synapses_and_weights_in_memories()
	connectivity_distribution()
arguments:
functions called:
  insert_second_neuron_in_neighbor_list_of_first() <double *> (in simboltz.c)
  choose_neuron_in_cluster_with_weight_distribution() <int>
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void
distribute_synapses_between_clusters_in_memory(symbol_center, num_inter_cluster_synapses)
int 	*symbol_center, num_inter_cluster_synapses;
{
	extern double *insert_second_neuron_in_neighbor_list_of_first();
	int	    choose_neuron_in_cluster_with_weight_distribution();

	int	i, first_cluster, second_cluster, center_neuron1, center_neuron2,
		ni, nj, same;

	double	weight, synapse;
	

	for (i = 0; i < num_inter_cluster_synapses; i++)
	{
	  first_cluster = (int)( ran3(&iseed) * n_symbol_centers);
		if (first_cluster == n_symbol_centers) first_cluster = n_symbol_centers - 1;

	  center_neuron1 = symbol_center[first_cluster];

          same = true;
	  while (same)
	  {
	    second_cluster = (int)( ran3(&iseed) * n_symbol_centers);
		if (second_cluster == n_symbol_centers) second_cluster = n_symbol_centers - 1;

	    center_neuron2 = symbol_center[second_cluster];
            if (center_neuron2 != center_neuron1) same = false;
          }

	  ni = choose_neuron_in_cluster_with_weight_distribution(center_neuron1);

          same = true;
	  while (same)
	  {
	  	nj = choose_neuron_in_cluster_with_weight_distribution(center_neuron2);
            	if (nj != ni) same = false;
          }

	  weight = w_max * ( 1.0 - 2.0 * ran3(&iseed) );
	  while (fabs(weight) < w_min)
	  	weight = w_max * ( 1.0 - 2.0 * ran3(&iseed) );
	  if (w(ni,nj) == BOLTZNULL)
	  {
	  	w(ni,nj) = insert_second_neuron_in_neighbor_list_of_first(ni, nj, weight);
	  	w(nj,ni) = insert_second_neuron_in_neighbor_list_of_first(nj, ni, weight);
	  }
	  else
	  {
		synapse = *w(ni,nj); 
		if (*w(ni,nj) >= 0.0) *w(ni,nj) = *w(ni,nj) + fabs(weight);
		else *w(ni,nj) = *w(ni,nj) - fabs(weight);
		if ( fabs(*w(ni,nj)) >= w_max ) *w(ni,nj) =  ( *w(ni,nj) / fabs(*w(ni,nj)) ) * w_max;
		*w(nj,ni) = *w(ni,nj);
		synapse = *w(ni,nj);
	  }

	}  /*   end for allocating all synapses    */

}  /*   end distribute_synapses_between_clusters_in_memory()   */








/*
********************************************************************************
Distributes synapses between clusters in different memory modules. These can be 
long-range synapses.

called by:
	allocate_synapses_and_weights_in_memories()
	connectivity_distribution()
arguments:
functions called:
  insert_second_neuron_in_neighbor_list_of_first() <double *> (in simboltz.c)
  choose_neuron_in_cluster_with_weight_distribution() <int>
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void
distribute_synapses_between_memories(symbol_center_sens_mem, symbol_center_decl_mem, num_inter_memory_synapses)
int	*symbol_center_sens_mem, *symbol_center_decl_mem, num_inter_memory_synapses;
{
	extern double *insert_second_neuron_in_neighbor_list_of_first();
	int	    choose_neuron_in_cluster_with_weight_distribution();

	int	i, first_cluster, second_cluster, center_neuron1, center_neuron2,
		ni, nj, counter, max_counter;

	double	weight, synapse;
	
	max_counter = n_nodes * 100;

	for (i = 0; i < num_inter_memory_synapses; i++)
	{
	  first_cluster = (int)( ran3(&iseed) * n_symbol_centers);
		if (first_cluster == n_symbol_centers) first_cluster = n_symbol_centers - 1;

	  center_neuron1 = symbol_center_sens_mem[first_cluster];

	  second_cluster = (int)( ran3(&iseed) * n_symbol_centers);
		if (second_cluster == n_symbol_centers) second_cluster = n_symbol_centers - 1;

	  center_neuron2 = symbol_center_decl_mem[second_cluster];

	  ni = choose_neuron_in_cluster_with_weight_distribution(center_neuron1);
	  counter = 0;
          while ( (ni >= n_memory1) && (counter < max_counter) )
	  {
		ni = choose_neuron_in_cluster_with_weight_distribution(center_neuron1);
		counter = counter + 1;
	  }
	  if ( (counter == max_counter) && (ni >= n_memory1) )
	    gerror ("Could not choose neuron in sensorial memory.\
                       In distribute_synapses_between_memories(), (boltz_functs.c).");

	  counter = 0;
	  nj = choose_neuron_in_cluster_with_weight_distribution(center_neuron2);
          while ( (nj < n_memory1) && (counter < max_counter) )
	  {
		nj = choose_neuron_in_cluster_with_weight_distribution(center_neuron2);
		counter = counter + 1;
	  }
	  if ( (counter == max_counter) && (nj < n_memory1) )
	    gerror ("Could not choose neuron in declarative memory.\
                       In distribute_synapses_between_memories(), (boltz_functs.c).");

	  weight = small * w_max * ( 1.0 - 2.0 * ran3(&iseed) );
	  while (fabs(weight) < w_min)
	  	weight = small * w_max * ( 1.0 - 2.0 * ran3(&iseed) );
	  if (w(ni,nj) == BOLTZNULL)
	  {
	  	w(ni,nj) = insert_second_neuron_in_neighbor_list_of_first(ni, nj, weight);
	  	w(nj,ni) = insert_second_neuron_in_neighbor_list_of_first(nj, ni, weight);
	  }
	  else
	  {
		synapse = *w(ni,nj);
		if (*w(ni,nj) >= 0.0) *w(ni,nj) = *w(ni,nj) + fabs(weight);
		else *w(ni,nj) = *w(ni,nj) - fabs(weight);
		if ( fabs(*w(ni,nj)) >= w_max ) *w(ni,nj) =  ( *w(ni,nj) / fabs(*w(ni,nj)) ) * w_max;
		*w(nj,ni) = *w(ni,nj);
		synapse = *w(ni,nj);
	  }

	}  /*   end for allocating all synapses    */


}  /*  end distribute_synapses_between_memories()  */









/*
********************************************************************************
Chooses a neuron in cluster whose center is center_neuron to connect to
another neuron in another cluster.

called by:
	distribute_synapses_between_clusters_in_memory()
	distribute_synapses_between_memories()
arguments:
functions called: ran3() <double> (in random.c)
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
int
choose_neuron_in_cluster_with_weight_distribution(center_neuron)
int	center_neuron;
{
	neighbor_inf	*np, *np2;
	int		j, neighbor_neuron, num_neurons, not_found_neuron, num_neighbor;
	double		cluster_sum_weights, sum_neuron_weights;

	if (neuron[center_neuron]->num_neighbors == 0) neighbor_neuron = center_neuron;
	else
	{  
	  cluster_sum_weights = 0.0;
	  for (np = neuron[center_neuron]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)
	  {
	    cluster_sum_weights = cluster_sum_weights + fabs(np->wij);
            neighbor_neuron = np->neigh_ID;
	    for (np2 = neuron[neighbor_neuron]->list_neighs; np2 != NO_NEIGHBOR; np2 = np2->next_neigh)  
	  	cluster_sum_weights = cluster_sum_weights + fabs(np2->wij);
	  }

	  num_neurons = neuron[center_neuron]->num_neighbors + 1;  /*  center + its neighbors   */
	  not_found_neuron = true;
	  while (not_found_neuron)
	  {	    
	    num_neighbor = (int)( ran3(&iseed) * num_neurons);
	    if (num_neighbor == num_neurons) num_neighbor = num_neurons - 1;

 	    if (num_neighbor == (num_neurons - 1)) neighbor_neuron = center_neuron;    
	    else
	    {
		np = neuron[center_neuron]->list_neighs; 
		for (j = 0; j <= num_neighbor; j++)
            	 {
	        	neighbor_neuron = np->neigh_ID;
			np = np->next_neigh; 
	    	 }
	    }

            sum_neuron_weights = 0.0;
	    for (np2 = neuron[neighbor_neuron]->list_neighs; np2 != NO_NEIGHBOR; np2 = np2->next_neigh)  
	  	sum_neuron_weights = sum_neuron_weights + fabs(np2->wij);

	    if ( ran3(&iseed) < (sum_neuron_weights / cluster_sum_weights) )
			not_found_neuron = false;
	  } /*  end while   */

	}  /*  end else center_neuron has neighbors  */

	return (neighbor_neuron);

}  /* choose_neuron_in_cluster_with_weight_distribution()  */








/*
********************************************************************************


called by: allocate_synapses_and_weights_in_memories()
arguments:
functions called:
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void
generate_node_degree_distribution()
{
	int	i, num_links, *num_nodes_with_n_links;
	FILE	*link_distribution;

        num_nodes_with_n_links = (int *) malloc( n_nodes * sizeof(int) );
        check_allocation( (void *)num_nodes_with_n_links,
                   "generate_node_degree_distribution()", "num_nodes_with_n_links");

	for (i = 0; i < n_nodes; i++) num_nodes_with_n_links[i] = 0;

	for (i = 0; i < n_nodes; i++)
	{
		num_links = neuron[i]->num_neighbors;
		num_nodes_with_n_links[num_links] = num_nodes_with_n_links[num_links] + 1;
	}

	link_distribution = fopen("node_degree_distribution", "w");

	for (i = 0; i < n_nodes; i++)
		  fprintf(link_distribution, " %d \t %d\n", i, num_nodes_with_n_links[i]);

	fclose (link_distribution);

}   /*  end generate_node_degree_distribution()  */





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

	if (nodes_more_than_one_neigh > 0)
		return ( sum_clust_coeff / nodes_more_than_one_neigh );
	else
		gerror ("No nodes with more than one neighbor\n\
                         Clustering coefficient undefined.\n\
                         In calculate_clustering_coefficient(), (boltz_functs.c).");
	
        return 0;
} /*   end calculate_clustering_coefficient()    */








/*
********************************************************************************


called by:  initialize_boltzmann_machine_state()  <void> (in simboltz.c)
arguments:
functions called:
	distribute_neurons_spatially_uniformly_in_memory() <void>
	remove_second_neuron_from_neighbor_list_of_first() <void> (in simboltz.c)
	distribute_synapses_in_memory_by_gaussian_distribution() <void>
	choose_symbol_centers_in_memory() <void>
	form_Clusters_in_memory() <void>
	calculate_clustering_coefficient() <double>
	distribute_synapses_between_clusters_in_memory() <void>
	distribute_synapses_between_memories() <void>
global variables:
local variables:
I/O conditions:  
********************************************************************************
*/
void 
connectivity_distribution(dimension1, dimension2)
double 	dimension1, dimension2;
{
	extern void 	output_neighbor_lists(),
			remove_second_neuron_from_neighbor_list_of_first();
	void 	distribute_synapses_in_memory_by_gaussian_distribution(),
		choose_symbol_centers_in_memory(),
		form_Clusters_in_memory(),
		distribute_neurons_spatially_uniformly_in_memory(),
		distribute_synapses_between_clusters_in_memory(),
		distribute_synapses_between_memories();

	int	i, j, k, num_links, *num_nodes_with_n_links_Gauss, *num_nodes_with_n_links, num_inter_cluster_synapses, 
		num_inter_memory_synapses, iterations_for_average = 10000;
 
	double	total_num_synapses, clustering_coefficient, calculate_clustering_coefficient();

	int 	*symbol_center_sens_mem, *symbol_center_decl_mem; 
	FILE	*pos_centers_sens_mem, *pos_centers_decl_mem;

	FILE	*link_distribution, *link_distribution_Gauss;
	FILE	*pos_sens_mem, *pos_decl_mem;

	neighbor_inf 	*np;

  num_nodes_with_n_links_Gauss = (int *) malloc( n_nodes * sizeof(int) );
  check_allocation( (void *)num_nodes_with_n_links_Gauss,
                   "connectivity_distribution()", "num_nodes_with_n_links_Gauss");

  num_nodes_with_n_links = (int *) malloc( n_nodes * sizeof(int) );
  check_allocation( (void *)num_nodes_with_n_links,
                   "connectivity_distribution()", "num_nodes_with_n_links");

  for (i = 0; i < n_nodes; i++) num_nodes_with_n_links_Gauss[i] = 0;

  for (i = 0; i < n_nodes; i++) num_nodes_with_n_links[i] = 0;

  clustering_coefficient = 0.0;

  symbol_center_sens_mem = (int *) malloc( n_symbol_centers*sizeof(int) );
  check_allocation( (void *)symbol_center_sens_mem,
                      "connectivity_distribution()", "symbol_center_sens_mem");

  symbol_center_decl_mem = (int *) malloc( n_symbol_centers*sizeof(int) );
  check_allocation( (void *)symbol_center_decl_mem,
                      "connectivity_distribution()", "symbol_center_decl_mem");

  for (k = 0; k < iterations_for_average; k++)
  {
	pos_sens_mem = fopen("positions_sensorial_memory", "w");
	pos_decl_mem = fopen("positions_declarative_memory", "w");
 	distribute_neurons_spatially_uniformly_in_memory(pos_sens_mem, 0, n_memory1-1, dimension1, false);
	distribute_neurons_spatially_uniformly_in_memory(pos_decl_mem, n_memory1, n_nodes-1, dimension2, true);
	fclose(pos_sens_mem);
	fclose(pos_decl_mem);

	if (k > 0)
	for (i = 0; i < n_nodes; i++)
	{
		for (np = neuron[i]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)  
		  remove_second_neuron_from_neighbor_list_of_first(i, np->neigh_ID, np);
	}


	for (i = 0; i < n_nodes; i++)
        { 
		for (j = i; j < n_nodes; j++)
		{
			w(i,j) = BOLTZNULL;
			w(j,i) = BOLTZNULL;
		}
        }


        /*   Gaussian distribution of synapses in sensorial memory    */
	/*     upper-left corner of matrix: sensorial memory  */  

	distribute_synapses_in_memory_by_gaussian_distribution(0, n_memory1-1);


        /*   Gaussian distribution of synapses in declarative memory    */  
	/*     lower-right corner of matrix: declarative memory */

	distribute_synapses_in_memory_by_gaussian_distribution(n_memory1, n_nodes-1);

	if (n_symbol_centers < 1) 
          gerror ("No symbol centers to form clusters in memories. In connectivity_distribution(), (boltz_functs.c).");

	for (i = 0; i < n_nodes; i++)
	{
		num_links = neuron[i]->num_neighbors;
		num_nodes_with_n_links_Gauss[num_links] = num_nodes_with_n_links_Gauss[num_links] + 1;
	}

/*	if (k == 78 && output_to_monitor)
		printf("Will choose centers for k = 78");    */

       /*   Choose symbol centers in sensorial memory    */  

	pos_centers_sens_mem = fopen("positions_centers_sensorial_memory", "w");
	choose_symbol_centers_in_memory(symbol_center_sens_mem, 0, n_memory1 - 1, pos_centers_sens_mem);
	fclose(pos_centers_sens_mem);

        /*   Choose symbol centers in declarative memory    */  

	pos_centers_decl_mem = fopen("positions_centers_declarative_memory", "w");
	choose_symbol_centers_in_memory(symbol_center_decl_mem, n_memory1, n_nodes - 1, pos_centers_decl_mem);
	fclose(pos_centers_decl_mem);


       /*   Formation of clusters in sensorial memory    */

	form_Clusters_in_memory(symbol_center_sens_mem);  

     /*   Formation of clusters in declarative memory    */  

	form_Clusters_in_memory(symbol_center_decl_mem);

	total_num_synapses = 0.0;

	for ( i = 0; i < n_nodes; i++) 
	  total_num_synapses = total_num_synapses + neuron[i]->num_neighbors;
	total_num_synapses = total_num_synapses / 2.0;

/*	clustering_coefficient = clustering_coefficient + calculate_clustering_coefficient();
*/
	num_inter_cluster_synapses = (int) (percent_total_synapses * total_num_synapses);
	num_inter_memory_synapses = (int) (percent_synapses_between_memories * total_num_synapses);

	distribute_synapses_between_clusters_in_memory(symbol_center_sens_mem, num_inter_cluster_synapses);

	distribute_synapses_between_clusters_in_memory(symbol_center_decl_mem, num_inter_cluster_synapses);

	distribute_synapses_between_memories(symbol_center_sens_mem, symbol_center_decl_mem, num_inter_memory_synapses);


	clustering_coefficient = clustering_coefficient + calculate_clustering_coefficient();

	for (i = 0; i < n_nodes; i++)
	{
		num_links = neuron[i]->num_neighbors;
		num_nodes_with_n_links[num_links] = num_nodes_with_n_links[num_links] + 1;
	}

/*	if(output_to_monitor)	
	  printf("k =  %d\n", k);    */
  }

  link_distribution_Gauss = fopen("node_degree_distribution_Gaussian", "w");

  for (i = 0; i < n_nodes; i++)
	fprintf(link_distribution_Gauss, " %d \t %f\n", i, 
                     num_nodes_with_n_links_Gauss[i] / (double) iterations_for_average );

  fclose (link_distribution_Gauss);


  link_distribution = fopen("node_degree_distribution", "w");

  for (i = 0; i < n_nodes; i++)
	fprintf(link_distribution, " %d \t %f\n", i, num_nodes_with_n_links[i] / (double) iterations_for_average );

  fclose (link_distribution);


  fprintf(finalparamf, "\n\n\t average clustering coefficient of ensemble of %d initial topologies = %f\n\n", 
                 iterations_for_average, clustering_coefficient / (double) iterations_for_average);

  for (i = 0; i < n_nodes; i++)
  {
	for (np = neuron[i]->list_neighs; np != NO_NEIGHBOR; np = np->next_neigh)  
	  remove_second_neuron_from_neighbor_list_of_first(i, np->neigh_ID, np);
  }


  free (symbol_center_sens_mem);
  free (symbol_center_decl_mem);

}   /*  connectivity_distribution    */
