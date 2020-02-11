#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <string.h>
#include <time.h>
#include "common.h"


int main(){
  FILE *pattern, *configuration;

  pattern = fopen("init_stored_patterns", "w+");

  void initialize_restricted_boltzmann_machine();
  return 0;
}

void input_data_from_file(){
  void print_initial_parameters();

  FILE *fp;
  fp = fopen("inboltz.dat", "r");

  fscanf(fp, "%*s%d%*s \
              %*s%d%*s \
              %*s%d%*s \
              %*s%d%*s \
              %*s%d%*s,
              &output_to_monitor,
              &store_found_patterns_memory,
              &recovering_from_checkpoint,
              &num_found_patterns_between_checkpoints,
              &n_nodes);

  fclose(fp);

  print_initial_parameters();

}
