/*
**********************************************************************************

	File with data structures for Boltzmann machine.

		Version 1.0, August 19, 2009
			Roseli Wedemann

**********************************************************************************
*/



/*
**********************************************************************************
Structure for saving information about each node's neighbors in neuronal graph.
        neigh_ID <long>: stores neighbor's ID in graph.
	wij <double>: weight of synapse that connects neighbor j to neuron i.
	prev_ neigh <struct NEIGHBOR_INF *>: pointer to previous neighbor in list.
	next_neigh <struct NEIGHBOR_INF *>: pointer to next neighbor in list.
**********************************************************************************
*/
typedef	struct NEIGHBOR_INF	{
					int			neigh_ID;
				  	double			wij;
					struct NEIGHBOR_INF	*prev_neigh;
					struct NEIGHBOR_INF	*next_neigh;
				} neighbor_inf;

#define	NO_NEIGHBOR (neighbor_inf *)BOLTZNULL


/*
**********************************************************************************
Structure for saving information about each neuron (node) in neuronal graph.
        num_neighbors <int>: stores number of neighbors of neuron.
	list_neighs <neighbor_inf *>: pointer to beginning of list of 
                                       neuron's neighbors.
**********************************************************************************
*/
typedef	struct NEURON_INF	{
					int			num_neighbors;
					double			pos_coord1;
					double			pos_coord2;
					neighbor_inf		*list_neighs;
				} neuron_inf;


/*
**********************************************************************************
Structure for saving information about each pattern stored by the neural network.
        pattern_ID <double>: stores a unique number associated with pattern of bits.
                             found by function convert_bitstring_into_integer().
        pattern <int *>: pointer to string of bits representing a stored pattern
                           (network state).
	energy_pattern <double>: stores energy associated with a patten.
**********************************************************************************
*/
typedef	struct PATTERN_INF	{
				  	double			pattern_ID;
                                        int                     *pattern_bits;
					double			energy_pattern;
				} pattern_inf;


/*
**********************************************************************************
Structure for saving information about list of patterns in neuronal graph.
        neigh_ID <long>: stores neighbor's ID in graph.
	wij <double>: weight of synapse that connects neighbor j to neuron i.
	prev_ neigh <struct NEIGHBOR_INF *>: pointer to previous neighbor in list.
	next_neigh <struct NEIGHBOR_INF *>: pointer to next neighbor in list.
**********************************************************************************
*/
typedef	struct PATTERN_LIST_INF	{
					pattern_inf		*pattern;
					struct PATTERN_LIST_INF	*next_pattern;
				} pattern_list_inf;

#define	NO_PATTERN (pattern_list_inf *)BOLTZNULL

