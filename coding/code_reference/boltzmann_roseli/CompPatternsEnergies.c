/*
****************************************************************************************


****************************************************************************************
*/


#include	<stdio.h>	  /* library with standard I/O functions */
#include	<math.h>	  /* math functions library declarations */
#include	<string.h>	  /* string functions library declarations */
#include        <stdlib.h>
//#include	<malloc.h>	  /* memory allocation library declarations */


#define		false	0
#define		true	1

/*
********************************************************************************
Structure for storage of different energy values found in the network.
	
********************************************************************************
*/	
typedef struct ENERGY_lIST_ELEM	{
					double			Energy;
					struct ENERGY_lIST_ELEM	*next_energy;
/*					struct ENERGY_lIST_ELEM	*prev_energy;  */
			 	} energy_list_elem;		

#define	NO_ENERGY	(energy_list_elem *)NULL	



/*
********************************************************************************
			 Global Functions
********************************************************************************
*/

void	check_allocation();


/*
********************************************************************************
			Global Variables
********************************************************************************
*/
FILE 	*errorf, *report_patternsf;




/*	Main function of Program that compares Initial and Final (after learning) 
        stored patterns of Boltzmann Machine Psychoanalytic working through. */

int main()
{
	void		sort_insert_in_Energy_list(), print_energies_of_stored_patterns();

	FILE		*init_patternf, *new_patternf;

	int		n_nodes, j, k,
			*init_found_pattern, *new_found_pattern,
			equal, init_end_of_file, new_end_of_file,
			num_init_energies = 0, num_new_energies = 0,
			num_equal_patterns = 0;

	energy_list_elem	*init_energy_list, *new_energy_list;

	double 		pattern_ID, init_Energy, new_Energy, 
			num_new_patterns = 0.0, num_init_patterns = 0.0;


        init_patternf = fopen("init_stored_patterns", "r");

	new_patternf = fopen("new_stored_patterns", "r");

	report_patternsf = fopen("report_stored_patterns", "w+");

	errorf = fopen("report_patterns_errors", "w");

	printf ("\n\n\n Print total number of neurons in memory network. \n\n");

	scanf("%d", &n_nodes);

	fprintf (report_patternsf, "\n\n\n Stored patterns that did not change during learning process. \n\n");


	fscanf(init_patternf, "%lf", &pattern_ID);
	init_end_of_file = feof(init_patternf);
	rewind (init_patternf);	

	fscanf(new_patternf, "%lf", &pattern_ID);	
	new_end_of_file = feof(new_patternf);
	rewind (new_patternf);


        init_found_pattern = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)init_found_pattern,
                      "main()", "init_found_pattern");

        new_found_pattern = (int *) malloc( n_nodes*sizeof(int) );
        check_allocation( (void *)new_found_pattern,
                      "main()", "new_found_pattern");

	init_energy_list = NO_ENERGY;
	new_energy_list = NO_ENERGY;


	while ( init_end_of_file == 0 )
	{
		fscanf(init_patternf, "%*c%lf", &pattern_ID);
		init_end_of_file = feof(init_patternf);
			
		if (init_end_of_file == 0)
		{
			for (k = 0; k < n_nodes; k++)
			{
				fscanf(init_patternf, "%*c%d", &init_found_pattern[k]);
			}
			fscanf(init_patternf, "%*c%lf", &init_Energy);
                        fscanf(init_patternf, "%*c");

			sort_insert_in_Energy_list(&init_energy_list, init_Energy, &num_init_energies);
		   	num_init_patterns = num_init_patterns + 1.0;
		}
	}   /*  while through initial stored pattern file for counting  */

	
        while ( new_end_of_file == 0 )
        {
		fscanf(new_patternf, "%*c%lf", &pattern_ID);
		new_end_of_file = feof(new_patternf);

		if (new_end_of_file == 0)
		{
		  for (k = 0; k < n_nodes; k++)
		  {
			fscanf(new_patternf, "%*c%d", &new_found_pattern[k]);
		  }
		  fscanf(new_patternf, "%*c%lf", &new_Energy);
                  fscanf(new_patternf, "%*c");

		  sort_insert_in_Energy_list(&new_energy_list, new_Energy, &num_new_energies);

		  num_new_patterns = num_new_patterns + 1.0;

		  rewind (init_patternf);
		  fscanf(init_patternf, "%lf", &pattern_ID);
		  init_end_of_file = feof(init_patternf);
		  rewind (init_patternf);	

		  equal = false;

		  while ( (init_end_of_file == 0) && (!equal) )
		  {
			fscanf(init_patternf, "%*c%lf", &pattern_ID);
			init_end_of_file = feof(init_patternf);
			
			if (init_end_of_file == 0)
			{
			   for (k = 0; k < n_nodes; k++)
			   {
				fscanf(init_patternf, "%*c%d", &init_found_pattern[k]);
			   }
			   fscanf(init_patternf, "%*c%lf", &init_Energy);
                           fscanf(init_patternf, "%*c");

			   equal = true;
			   j = 0;

			   while (equal && j < n_nodes)			
			   {
			     if ( new_found_pattern[j] == init_found_pattern[j] )
				j = j + 1;
			     else equal = false;
			   }  /*  end while for individual pattern in file  */

			   if (equal)
			   {
				num_equal_patterns = num_equal_patterns + 1;

		                fprintf (report_patternsf, "\t%lf", pattern_ID);
		                for (j = 0; j < n_nodes; j++)
		                { 
		    			fprintf (report_patternsf, " %d", init_found_pattern[j]);
		  		}
	          		fprintf (report_patternsf, " %lf", init_Energy);
                  		fprintf (report_patternsf, "\n");

			   }  /* end if equal  */
			
			}  /*  end if  */
			
		  }   /*  end while through file with initial patterns before learning  */

		}  /*  end if not end of new stored pattern file */

        }  /*  end while through file with new patterns after learning  */ 

	fprintf (report_patternsf, "\n\n\n Number of initially stored patterns: %lf\n", num_init_patterns);
	fprintf (report_patternsf, "\n Number of stored patterns after learning: %lf\n", num_new_patterns);
	fprintf (report_patternsf, "\n Number of patterns that are still stored after learning: %d\n",
                      num_equal_patterns);
	fprintf (report_patternsf, "\n Percentage of patterns that are still stored after learning: %lf\n",
                      (double) num_equal_patterns / num_init_patterns);

	fprintf (report_patternsf, "\n\n\n There are %d different energies before learning. \n\n", 
			num_init_energies);
	print_energies_of_stored_patterns(init_energy_list);

	fprintf (report_patternsf, "\n\n\n There are %d different energies after learning. \n\n", 
			num_new_energies);
	print_energies_of_stored_patterns(new_energy_list);

	fclose(init_patternf);
	fclose(new_patternf);
	fclose(report_patternsf);
	fclose(errorf);

}   /* end main() */


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




/*
********************************************************************************
Inserts energies in Energy list in increascing order of Energy value.

called by:
	main()
arguments:
functions called:
global variables:
local variables:
	en_pt <energy_list_elem *>: pointer to energy that is to be inserted.
I/O conditions:
********************************************************************************
*/
void
sort_insert_in_Energy_list(energy_list, energy, num_energies)
energy_list_elem	**energy_list;
double			energy;
int			*num_energies;
{
	energy_list_elem	*pl, *opl, *en_pt;

	opl = *energy_list;

	for (pl = *energy_list; (pl != NO_ENERGY) 
		&&  (pl->Energy <= energy); pl = pl->next_energy) opl = pl;

        if (pl == *energy_list)  /*  insert in head of list  */
        {
		en_pt = (energy_list_elem *)malloc(sizeof(energy_list_elem));
 		check_allocation( (void *)en_pt, "sort_insert_in_Energy_list()", 
			  "en_pt");

		en_pt->Energy = energy;

                *energy_list = en_pt;

		en_pt->next_energy = pl;
		
		*num_energies = *num_energies + 1.0;
        }				
	else if (opl->Energy != energy)  /* insert middle and end if energy 
                                            not already in list  */
	{
		en_pt = (energy_list_elem *)malloc(sizeof(energy_list_elem));
 		check_allocation( (void *)en_pt, "sort_insert_in_Energy_list()", 
			  "en_pt");

		opl->next_energy = en_pt;

		en_pt->Energy = energy;
					
		en_pt->next_energy = pl; 

		*num_energies = *num_energies + 1.0;
        }

} /*   end sort_insert_in_Energy_list();  */


/*
********************************************************************************
Prints Energy list in increascing order of Energy value.

called by:
	main()
arguments:
functions called:
global variables:
local variables:
	en_pt <energy_list_elem *>: pointer to energy that is to be inserted.
I/O conditions:
********************************************************************************
*/
void
print_energies_of_stored_patterns(energy_list)
energy_list_elem	*energy_list;
{
	energy_list_elem	*pl;

	for ( pl = energy_list; (pl != NO_ENERGY); pl = pl->next_energy ) 
          fprintf (report_patternsf, "\t %lf\n", pl->Energy);

} /*   end  print_energies_of_stored_patterns();  */


