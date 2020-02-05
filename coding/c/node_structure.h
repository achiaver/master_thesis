typedef struct NODE_INFO {
  int node_id;
  struct NODE_INFO *prev_node;
  struct NODE_INFO *next_node;
} node_info;

typedef struct KNOWLEDGE_ATOM {
  int k;
  int activation;
  struct NODE_INFO *list_nodes;
} knowledge_atom;

typedef struct FEATURE_NODE {
  int r;
  struct NODE_INFO *list_nodes;
} feature_node;



