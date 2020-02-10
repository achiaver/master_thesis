typedef struct NODE_INFO {
  int node_id; //node_id : node identification
  int type; //type : define which type of node it is (0 - feature, 1 - knowledge)
  double wij;  //wij : weight value between to connected nodes
  struct NODE_INFO *prev_node; //???
  struct NODE_INFO *next_node; //???
} node_info;

typedef struct KNOWLEDGE_ATOM {
  int k;
  int activation;
  double strength;
  struct NODE_INFO *list_node; //List of feature nodes to which this knowledge atom is connected to
} knowledge_atom;

typedef struct FEATURE_NODE {
  int r;
  struct NODE_INFO *list_nodes; //List of knowledge atoms to which this feature node is connected to
} feature_node;



