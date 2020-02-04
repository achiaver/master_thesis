typedef struct NEIGHBOR_INF {
  int neigh_ID;
  double wij;
  struct NEIGHBOR_INF *prev_neigh;
  struct NEIGHBOR_INF *next_neigh;
} neighbor_inf;


typedef struct NODE_INF {
  int num_neighbors;
  int feature;
  struct NEIGHBOR_INF *list_neighs;
} node_inf;

typedef struct ATOM_INF {
  int num_neighbors;
  int knowledge;
  int activation;
  struct NEIGHBOR_INF *list_neighs;
} atom_inf;


