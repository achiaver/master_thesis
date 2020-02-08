#include <stdlib.h>
#include <stdio.h>
#include <math.h>	  
#include <string.h>	  
//#include <malloc.h>	  

/*
**********************************************************************************
Structure for saving information about each node's neighbors in neuronal graph.
        neigh_ID <long>: stores neighbor's ID in graph.
	wij <double>: weight of synapse that connects neighbor j to neuron i.
	prev_ neigh <struct NEIGHBOR_INF *>: pointer to previous neighbor in list.
	next_neigh <struct NEIGHBOR_INF *>: pointer to next neighbor in list.
**********************************************************************************
*/
typedef struct NEURONIO_CONTENT{  

	double V;
	double V2;
	double *peso;
	double delta;
	double h_in;
	            
} neuronio_content;


/*
**********************************************************************************
Structure for saving information about each node's neighbors in neuronal graph.
        neigh_ID <long>: stores neighbor's ID in graph.
	wij <double>: weight of synapse that connects neighbor j to neuron i.
	prev_ neigh <struct NEIGHBOR_INF *>: pointer to previous neighbor in list.
	next_neigh <struct NEIGHBOR_INF *>: pointer to next neighbor in list.
**********************************************************************************
*/
typedef struct CAMADA_CONTENT{    

	int num_neuronios;
	struct NEURONIO_CONTENT *neuronio;
	                      
} camada_content;



