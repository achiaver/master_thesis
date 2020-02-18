/* 
********************************************************************************
               File with functions for saving intermediate results 
               for Boltzmann Simulator in case process is stopped undesirably,
               as in an electricity power failure.


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
#include    "randextr.h" /* Declarations of global variables from random number generator. */

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
Function that saves process state every time the number of found patterns
is equal to num_found_patterns_between_checkpoints, for recovery in case 
of unexpected system failure.


called by:  
	find_network_stored_patterns()  <void> (in boltz_functs.c)
arguments:
functions called:
	standard_IO
	
global variables:
	
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
check_point_intermediate_found_patterns_file(patternf_BM, patternf_GSA, freq_pattern_BM, freq_pattern_GSA,
	num_patterns_BM, num_patterns_GSA, max_patterns_in_file_BM, max_patterns_in_file_GSA,
	total_trial_patterns)
FILE		*patternf_BM, *patternf_GSA;
double  	*freq_pattern_BM, *freq_pattern_GSA;
long		num_patterns_BM, num_patterns_GSA, 
		max_patterns_in_file_BM, max_patterns_in_file_GSA, total_trial_patterns;
{

	FILE		*temp_find_pattern_parameters, *temp_patternf_BM, *temp_patternf_GSA, 
			*temp_freq_visited_pattern_BM, *temp_freq_visited_pattern_GSA;

	int		k, i_pattern, end_of_file_BM, end_of_file_GSA, pattern_bit;

	double		pattern_ID, pattern_energy;



	temp_find_pattern_parameters = fopen("temp_find_stored_patterns_parameters", "w");
	temp_patternf_BM = fopen("temp_init_stored_patterns", "w");
	temp_patternf_GSA = fopen("temp_init_stored_patterns_GSA", "w");
	temp_freq_visited_pattern_BM = fopen("temp_frequency_visited_stored_pattern_BM", "w");
	temp_freq_visited_pattern_GSA = fopen("temp_frequency_visited_stored_pattern_GSA", "w");


	fprintf(temp_find_pattern_parameters, "\t%ld\t%ld\n",  num_patterns_BM, num_patterns_GSA);
	fprintf(temp_find_pattern_parameters, "\t%ld\t%ld\n",  max_patterns_in_file_BM, max_patterns_in_file_GSA);
	fprintf(temp_find_pattern_parameters, "\t%ld\t%d\n",  total_trial_patterns, iseed);
	fprintf(temp_find_pattern_parameters, "\t%d\t%d\t%d\n",  inext, inextp, iff);
	for (k = 0; k < 56; k++)
		fprintf(temp_find_pattern_parameters, "\t%ld\n", ma[k]);


	fclose(temp_find_pattern_parameters);


	i_pattern = 0;

	if (BoltzMachine)
	{
		rewind (patternf_BM);
	  	fscanf(patternf_BM, "%lf", &pattern_ID);	
	  	end_of_file_BM = feof(patternf_BM);
	  	rewind (patternf_BM);

		while ( end_of_file_BM == 0 )
		{
			fscanf(patternf_BM, "%*c%lf", &pattern_ID);
			end_of_file_BM = feof(patternf_BM);
			
			if (end_of_file_BM == 0)
			{
		    		fprintf(temp_patternf_BM, "\t%lf", pattern_ID );
			   	for (k = 0; k < n_nodes; k++)
			   	{
					fscanf(patternf_BM, "%*c%d", &pattern_bit);
		    			fprintf(temp_patternf_BM, " %d", pattern_bit);
			   	}
			   	fscanf(patternf_BM, "%*c%lf", &pattern_energy);
                          	fscanf(patternf_BM, "%*c");

	            		fprintf(temp_patternf_BM, " %lf", pattern_energy);
                   		fprintf(temp_patternf_BM, "\n");

			  	i_pattern = i_pattern + 1;
			
			}  /*  end if  */
			
		}   /*  end while copies patterns between files  */

	  	for (k = 0; k < max_patterns_in_file_BM; k++)
 		  fprintf(temp_freq_visited_pattern_BM, "\t%d\t%lf\n", k, freq_pattern_BM[k] );

	}  /*   end if(BoltzMachine)   */


	i_pattern = 0;
	
	if (GSA)
	{
		rewind (patternf_GSA);
	  	fscanf(patternf_GSA, "%lf", &pattern_ID);	
	 	end_of_file_GSA = feof(patternf_GSA);
	  	rewind (patternf_GSA);

		while ( end_of_file_GSA == 0 )
		{
			fscanf(patternf_GSA, "%*c%lf", &pattern_ID);
			end_of_file_GSA = feof(patternf_GSA);
			
			if (end_of_file_GSA == 0)
			{
		    		fprintf(temp_patternf_GSA, "\t%lf", pattern_ID );
			   	for (k = 0; k < n_nodes; k++)
			   	{
					fscanf(patternf_GSA, "%*c%d", &pattern_bit);
		    			fprintf(temp_patternf_GSA, " %d", pattern_bit);
			   	}
			   	fscanf(patternf_GSA, "%*c%lf", &pattern_energy);
                          	fscanf(patternf_GSA, "%*c");

	            		fprintf(temp_patternf_GSA, " %lf", pattern_energy);
                   		fprintf(temp_patternf_GSA, "\n");

			  	i_pattern = i_pattern + 1;
			
			}  /*  end if  */
			
		}   /*  end while copies patterns between files  */

	  	for (k = 0; k < max_patterns_in_file_GSA; k++)
 		  fprintf(temp_freq_visited_pattern_GSA, "\t%d\t%lf\n", k, freq_pattern_GSA[k] );

	}   /* end if (GSA)   */


	fclose(temp_patternf_BM);
	fclose(temp_patternf_GSA);
	fclose(temp_freq_visited_pattern_BM);
	fclose(temp_freq_visited_pattern_GSA);


}  /*  end check_point_intermediate_found_patterns_file()  */
  


/*
********************************************************************************
Function that saves process state every time the number of found patterns
is equal to num_found_patterns_between_checkpoints, for recovery in case 
of unexpected system failure.


called by:  
	find_network_stored_patterns_use_memory()  <void> (in boltz_functs.c)
arguments:
functions called:
	standard_IO
	
global variables:
	
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
check_point_intermediate_found_patterns_memory(BM_pattern_list, GSA_pattern_list, freq_pattern_BM, freq_pattern_GSA,
	num_patterns_BM, num_patterns_GSA, max_patterns_in_file_BM, max_patterns_in_file_GSA,
	total_trial_patterns)
pattern_list_inf *BM_pattern_list, *GSA_pattern_list;
double  	 *freq_pattern_BM, *freq_pattern_GSA;
long		 num_patterns_BM, num_patterns_GSA, 
		 max_patterns_in_file_BM, max_patterns_in_file_GSA, total_trial_patterns;
{
	FILE		*temp_find_pattern_parameters, *temp_patternf_BM, *temp_patternf_GSA, 
			*temp_freq_visited_pattern_BM, *temp_freq_visited_pattern_GSA;

	int		i, j, i_pattern;

        pattern_list_inf  *current_pattern;


	temp_find_pattern_parameters = fopen("temp_find_stored_patterns_parameters", "w");
	temp_patternf_BM = fopen("temp_init_stored_patterns", "w");
	temp_patternf_GSA = fopen("temp_init_stored_patterns_GSA", "w");
	temp_freq_visited_pattern_BM = fopen("temp_frequency_visited_stored_pattern_BM", "w");
	temp_freq_visited_pattern_GSA = fopen("temp_frequency_visited_stored_pattern_GSA", "w");


	fprintf(temp_find_pattern_parameters, "\t%ld\t%ld\n",  num_patterns_BM, num_patterns_GSA);
	fprintf(temp_find_pattern_parameters, "\t%ld\t%ld\n",  max_patterns_in_file_BM, max_patterns_in_file_GSA);
	fprintf(temp_find_pattern_parameters, "\t%ld\t%d\n",  total_trial_patterns, iseed);
	fprintf(temp_find_pattern_parameters, "\t%d\t%d\t%d\n",  inext, inextp, iff);
	for (j = 0; j < 56; j++)
		fprintf(temp_find_pattern_parameters, "\t%ld\n", ma[j]);


	fclose(temp_find_pattern_parameters);


	i_pattern = 0;

	if (BoltzMachine)
	{
	  for (j = 0, current_pattern = BM_pattern_list; current_pattern != NO_PATTERN; j++)
	  {
		fprintf(temp_patternf_BM, "\t%lf", current_pattern->pattern->pattern_ID );
		for (i = 0; i < n_nodes; i++)
		{ 
		    fprintf(temp_patternf_BM, " %d", current_pattern->pattern->pattern_bits[i]);
		}
	        fprintf(temp_patternf_BM, " %lf", current_pattern->pattern->energy_pattern);
                fprintf(temp_patternf_BM, "\n");

 		fprintf(temp_freq_visited_pattern_BM, "\t%d\t%lf\n", j, freq_pattern_BM[j]);

		i_pattern = i_pattern + 1;

		current_pattern = current_pattern->next_pattern;

	  }

	}  /*   end if(BoltzMachine)   */


	i_pattern = 0;

	if (GSA)
	{
	  for (j = 0, current_pattern = GSA_pattern_list; current_pattern != NO_PATTERN; j++)
	  {
		fprintf(temp_patternf_GSA, "\t%lf", current_pattern->pattern->pattern_ID );
		for (i = 0; i < n_nodes; i++)
		{ 
		    fprintf(temp_patternf_GSA, " %d", current_pattern->pattern->pattern_bits[i]);
		}
	        fprintf(temp_patternf_GSA, " %lf", current_pattern->pattern->energy_pattern);
                fprintf(temp_patternf_GSA, "\n");

 		fprintf(temp_freq_visited_pattern_GSA, "\t%d\t%lf\n", j, freq_pattern_GSA[j]);

		i_pattern = i_pattern + 1;

		current_pattern = current_pattern->next_pattern;

	  }

	}  /*   end if(GSA)   */


	fclose(temp_patternf_BM);
	fclose(temp_patternf_GSA);
	fclose(temp_freq_visited_pattern_BM);
	fclose(temp_freq_visited_pattern_GSA);


}  /*  end check_point_intermediate_found_patterns_memory()  */






/*
********************************************************************************
Function that retrieves process state for recovery in case 
of unexpected system failure. Patterns are stored in files.


called by:  
	find_network_stored_patterns()  <void> (in boltz_functs.c)
arguments:
functions called:
	standard_IO
	
global variables:
	
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
retrieve_intermediate_found_patterns_file(patternf_BM, patternf_GSA, freq_pattern_BM, freq_pattern_GSA,
	addr_num_patterns_BM, addr_num_patterns_GSA, addr_max_patterns_in_file_BM, addr_max_patterns_in_file_GSA,
	addr_total_trial_patterns)
FILE		*patternf_BM, *patternf_GSA;
double  	**freq_pattern_BM, **freq_pattern_GSA;
long		*addr_num_patterns_BM, *addr_num_patterns_GSA, 
		*addr_max_patterns_in_file_BM, *addr_max_patterns_in_file_GSA, *addr_total_trial_patterns;
{

	FILE		*temp_find_pattern_parameters, *temp_patternf_BM, *temp_patternf_GSA, 
			*temp_freq_visited_pattern_BM, *temp_freq_visited_pattern_GSA;

	int		j, k, i_pattern, end_of_file_BM, end_of_file_GSA, pattern_bit;

	double		pattern_ID, pattern_energy, *vec_freq_BM, *vec_freq_GSA;


	temp_find_pattern_parameters = fopen("temp_find_stored_patterns_parameters", "r");
	temp_patternf_BM = fopen("temp_init_stored_patterns", "r");
	temp_patternf_GSA = fopen("temp_init_stored_patterns_GSA", "r");
	temp_freq_visited_pattern_BM = fopen("temp_frequency_visited_stored_pattern_BM", "r");
	temp_freq_visited_pattern_GSA = fopen("temp_frequency_visited_stored_pattern_GSA", "r");


	fscanf(temp_find_pattern_parameters, "%*c%ld%*c%ld%*c",  addr_num_patterns_BM, addr_num_patterns_GSA);
	fscanf(temp_find_pattern_parameters, "%*c%ld%*c%ld%*c",  addr_max_patterns_in_file_BM, addr_max_patterns_in_file_GSA);
	fscanf(temp_find_pattern_parameters, "%*c%ld%*c%d%*c",  addr_total_trial_patterns, &iseed);
	fscanf(temp_find_pattern_parameters, "%*c%d%*c%d%*c%d%*c",  &inext, &inextp, &iff);
	for (j = 0; j < 56; j++)
		fscanf(temp_find_pattern_parameters, "%*c%ld%*c", &(ma[j]) );


	vec_freq_BM = (double *) malloc( (*addr_max_patterns_in_file_BM)*sizeof(double) );
        check_allocation( (void *)vec_freq_BM,
                      "retrieve_intermediate_found_patterns_file()", "vec_freq_BM");

	vec_freq_GSA = (double *) malloc( (*addr_max_patterns_in_file_GSA)*sizeof(double) );
        check_allocation( (void *)vec_freq_GSA,
                      "retrieve_intermediate_found_patterns_file()", "vec_freq_GSA");


	i_pattern = 0;

	if (BoltzMachine)
	{
		rewind (temp_patternf_BM);
	  	fscanf(temp_patternf_BM, "%lf", &pattern_ID);	
	  	end_of_file_BM = feof(temp_patternf_BM);
	  	rewind (temp_patternf_BM);

		while ( end_of_file_BM == 0 )
		{
			fscanf(temp_patternf_BM, "%*c%lf", &pattern_ID);
			end_of_file_BM = feof(temp_patternf_BM);
			
			if (end_of_file_BM == 0)
			{
		    		fprintf(patternf_BM, "\t%lf", pattern_ID );
			   	for (k = 0; k < n_nodes; k++)
			   	{
					fscanf(temp_patternf_BM, "%*c%d", &pattern_bit);
		    			fprintf(patternf_BM, " %d", pattern_bit);
			   	}
			   	fscanf(temp_patternf_BM, "%*c%lf", &pattern_energy);
                          	fscanf(temp_patternf_BM, "%*c");

	            		fprintf(patternf_BM, " %lf", pattern_energy);
                   		fprintf(patternf_BM, "\n");

			  	i_pattern = i_pattern + 1;
			
			}  /*  end if  */
			
		}   /*  end while copies patterns between files  */

	  	for (k = 0; k < *addr_max_patterns_in_file_BM; k++)
 		  fscanf(temp_freq_visited_pattern_BM, "%*c%d%*c%lf%*c", &j, &(vec_freq_BM[k]) );

		*freq_pattern_BM = vec_freq_BM;

	}  /*   end if(BoltzMachine)   */


	i_pattern = 0;
	
	if (GSA)
	{
		rewind (temp_patternf_GSA);
	  	fscanf(temp_patternf_GSA, "%lf", &pattern_ID);	
	 	end_of_file_GSA = feof(temp_patternf_GSA);
	  	rewind (temp_patternf_GSA);

		while ( end_of_file_GSA == 0 )
		{
			fscanf(temp_patternf_GSA, "%*c%lf", &pattern_ID);
			end_of_file_GSA = feof(temp_patternf_GSA);
			
			if (end_of_file_GSA == 0)
			{
		    		fprintf(patternf_GSA, "\t%lf", pattern_ID );
			   	for (k = 0; k < n_nodes; k++)
			   	{
					fscanf(temp_patternf_GSA, "%*c%d", &pattern_bit);
		    			fprintf(patternf_GSA, " %d", pattern_bit);
			   	}
			   	fscanf(temp_patternf_GSA, "%*c%lf", &pattern_energy);
                          	fscanf(temp_patternf_GSA, "%*c");

	            		fprintf(patternf_GSA, " %lf", pattern_energy);
                   		fprintf(patternf_GSA, "\n");

			  	i_pattern = i_pattern + 1;
			
			}  /*  end if  */
			
		}   /*  end while copies patterns between files  */

	  	for (k = 0; k < *addr_max_patterns_in_file_GSA; k++)
 		  fscanf(temp_freq_visited_pattern_GSA, "%*c%d%*c%lf%*c", &j, &(vec_freq_GSA[k]) );

		*freq_pattern_GSA = vec_freq_GSA;

	}   /* end if (GSA)   */


	fclose(temp_find_pattern_parameters);
	fclose(temp_patternf_BM);
	fclose(temp_patternf_GSA);
	fclose(temp_freq_visited_pattern_BM);
	fclose(temp_freq_visited_pattern_GSA);


}   /*  retrieve_intermediate_found_patterns_file()  */




/*
********************************************************************************
Function that retrieves process state for recovery in case 
of unexpected system failure. Patterns are stored in memory.


called by:  
	find_network_stored_patterns()  <void> (in boltz_functs.c)
arguments:
functions called:
	standard_IO
	
global variables:
	
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
retrieve_intermediate_found_patterns_memory(addr_BM_pattern_list, addr_GSA_pattern_list, freq_pattern_BM, freq_pattern_GSA,
	addr_num_patterns_BM, addr_num_patterns_GSA, addr_max_patterns_in_file_BM, addr_max_patterns_in_file_GSA,
	addr_total_trial_patterns)
pattern_list_inf **addr_BM_pattern_list, **addr_GSA_pattern_list;
double  	 **freq_pattern_BM, **freq_pattern_GSA;
long		*addr_num_patterns_BM, *addr_num_patterns_GSA, 
		*addr_max_patterns_in_file_BM, *addr_max_patterns_in_file_GSA, *addr_total_trial_patterns;
{

	FILE		*temp_find_pattern_parameters, *temp_patternf_BM, *temp_patternf_GSA, 
			*temp_freq_visited_pattern_BM, *temp_freq_visited_pattern_GSA;

	int		j, k, i_pattern, end_of_file_BM, end_of_file_GSA;

	double		pattern_ID, *vec_freq_BM, *vec_freq_GSA;

	pattern_list_inf  *current_pattern, *new_pattern, *BM_pattern_list, *GSA_pattern_list;



	temp_find_pattern_parameters = fopen("temp_find_stored_patterns_parameters", "r");
	temp_patternf_BM = fopen("temp_init_stored_patterns", "r");
	temp_patternf_GSA = fopen("temp_init_stored_patterns_GSA", "r");
	temp_freq_visited_pattern_BM = fopen("temp_frequency_visited_stored_pattern_BM", "r");
	temp_freq_visited_pattern_GSA = fopen("temp_frequency_visited_stored_pattern_GSA", "r");


    printf("temp %d\n", temp_find_pattern_parameters);
	fscanf(temp_find_pattern_parameters, "%*c%ld%*c%ld%*c",  addr_num_patterns_BM, addr_num_patterns_GSA);
	fscanf(temp_find_pattern_parameters, "%*c%ld%*c%ld%*c",  addr_max_patterns_in_file_BM, addr_max_patterns_in_file_GSA);
	fscanf(temp_find_pattern_parameters, "%*c%ld%*c%d%*c",  addr_total_trial_patterns, &iseed);
	fscanf(temp_find_pattern_parameters, "%*c%d%*c%d%*c%d%*c",  &inext, &inextp, &iff);
	for (j = 0; j < 56; j++)
		fscanf(temp_find_pattern_parameters, "%*c%ld%*c", &(ma[j]) );


	vec_freq_BM = (double *) malloc( (*addr_max_patterns_in_file_BM)*sizeof(double) );
        check_allocation( (void *)vec_freq_BM,
                      "retrieve_intermediate_found_patterns_memory()", "vec_freq_BM");

	vec_freq_GSA = (double *) malloc( (*addr_max_patterns_in_file_GSA)*sizeof(double) );
        check_allocation( (void *)vec_freq_GSA,
                      "retrieve_intermediate_found_patterns_memory()", "vec_freq_GSA");

	for (j = 0; j < *addr_max_patterns_in_file_BM; j++)
		vec_freq_BM[j] = 0.0;

	for (j = 0; j < *addr_max_patterns_in_file_GSA; j++)
		vec_freq_GSA[j] = 0.0;

	i_pattern = 0;

	if (BoltzMachine)
	{
		rewind (temp_patternf_BM);
	  	fscanf(temp_patternf_BM, "%lf", &pattern_ID);	
	  	end_of_file_BM = feof(temp_patternf_BM);
	  	rewind (temp_patternf_BM);

		current_pattern = BM_pattern_list = NO_PATTERN;

		while ( end_of_file_BM == 0 )
		{
			fscanf(temp_patternf_BM, "%*c%lf", &pattern_ID);
			end_of_file_BM = feof(temp_patternf_BM);
			
			if (end_of_file_BM == 0)
			{
		    		new_pattern = (pattern_list_inf *) malloc( sizeof(pattern_list_inf) );
                    		check_allocation( (void *)new_pattern,
                                      "retrieve_intermediate_found_patterns_memory() BM", "new_pattern");

		    		new_pattern->pattern = (pattern_inf *) malloc( sizeof(pattern_inf) );
                    		check_allocation( (void *)new_pattern->pattern,
                                      "retrieve_intermediate_found_patterns_memory() BM", "new_pattern->pattern");

		    		new_pattern->pattern->pattern_bits = (int *) malloc( n_nodes*sizeof(int) );
                    		check_allocation( (void *)new_pattern->pattern->pattern_bits,
                                      "retrieve_intermediate_found_patterns_memory() BM", "new_pattern->pattern->pattern_bits");

		    		new_pattern->pattern->pattern_ID = pattern_ID;

			   	for (k = 0; k < n_nodes; k++)
					fscanf(temp_patternf_BM, "%*c%d", &(new_pattern->pattern->pattern_bits[k]) );

			   	fscanf(temp_patternf_BM, "%*c%lf", &(new_pattern->pattern->energy_pattern) );
                          	fscanf(temp_patternf_BM, "%*c");

		    		new_pattern->next_pattern = NO_PATTERN;

		    		if (BM_pattern_list == NO_PATTERN)
				{ 
					BM_pattern_list = new_pattern;
					current_pattern = BM_pattern_list;
				}
		    		else 
				{
					current_pattern->next_pattern = new_pattern;
					current_pattern = new_pattern;
				}

			  	i_pattern = i_pattern + 1;
			
			}  /*  end if  */
			
		}   /*  end while copies patterns from temp file to pattern list  */

		*addr_BM_pattern_list = BM_pattern_list;

	  	for (k = 0; k < *addr_num_patterns_BM; k++)
 		  fscanf(temp_freq_visited_pattern_BM, "%*c%d%*c%lf%*c", &j, &(vec_freq_BM[k]) );

		*freq_pattern_BM = vec_freq_BM;


	}  /*   end if(BoltzMachine)   */


	i_pattern = 0;

	if (GSA)
	{
		rewind (temp_patternf_GSA);
	  	fscanf(temp_patternf_GSA, "%lf", &pattern_ID);	
	  	end_of_file_GSA = feof(temp_patternf_GSA);
	  	rewind (temp_patternf_GSA);

		current_pattern = GSA_pattern_list = NO_PATTERN;

		while ( end_of_file_GSA == 0 )
		{
			fscanf(temp_patternf_GSA, "%*c%lf", &pattern_ID);
			end_of_file_GSA = feof(temp_patternf_GSA);
			
			if (end_of_file_GSA == 0)
			{
		    		new_pattern = (pattern_list_inf *) malloc( sizeof(pattern_list_inf) );
                    		check_allocation( (void *)new_pattern,
                                      "retrieve_intermediate_found_patterns_memory() GSA", "new_pattern");

		    		new_pattern->pattern = (pattern_inf *) malloc( sizeof(pattern_inf) );
                    		check_allocation( (void *)new_pattern->pattern,
                                      "retrieve_intermediate_found_patterns_memory() GSA", "new_pattern->pattern");

		    		new_pattern->pattern->pattern_bits = (int *) malloc( n_nodes*sizeof(int) );
                    		check_allocation( (void *)new_pattern->pattern->pattern_bits,
                                      "retrieve_intermediate_found_patterns_memory() GSA", "new_pattern->pattern->pattern_bits");

		    		new_pattern->pattern->pattern_ID = pattern_ID;

			   	for (k = 0; k < n_nodes; k++)
					fscanf(temp_patternf_GSA, "%*c%d", &(new_pattern->pattern->pattern_bits[k]));

			   	fscanf(temp_patternf_GSA, "%*c%lf", &(new_pattern->pattern->energy_pattern) );
                          	fscanf(temp_patternf_GSA, "%*c");

		    		new_pattern->next_pattern = NO_PATTERN;

		    		if (GSA_pattern_list == NO_PATTERN)
				{ 
					GSA_pattern_list = new_pattern;
					current_pattern = GSA_pattern_list;
				}
		    		else 
				{
					current_pattern->next_pattern = new_pattern;
					current_pattern = new_pattern;
				}

			  	i_pattern = i_pattern + 1;
			
			}  /*  end if  */
			
		}   /*  end while copies patterns from temp file to pattern list  */

		*addr_GSA_pattern_list = GSA_pattern_list;

	  	for (k = 0; k < *addr_num_patterns_GSA; k++)
 		  fscanf(temp_freq_visited_pattern_GSA, "%*c%d%*c%lf%*c", &j, &(vec_freq_GSA[k]) );

		*freq_pattern_GSA = vec_freq_GSA;


	}  /*   end if(GSA)   */


	fclose(temp_find_pattern_parameters);
	fclose(temp_patternf_BM);
	fclose(temp_patternf_GSA);
	fclose(temp_freq_visited_pattern_BM);
	fclose(temp_freq_visited_pattern_GSA);


}   /*  retrieve_intermediate_found_patterns_memory()  */

