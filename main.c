#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const int POSTFIX_LENGTH = 7;
const int HOST_LENGTH = 7 + POSTFIX_LENGTH;
const char *ALPHABET = "abcdefghijklmnopqrstuvwxyz0123456789";
const int ALPHABET_LENGTH = 26 + 10;

int int_math_mod(int n, int mod) {
  if (n < mod) {
    return n;
  } else {
    return n % mod;
  }
}

uint_fast64_t uint_math_mod(uint_fast64_t n, uint_fast64_t mod) {
  if (n < mod) {
    return n;
  } else {
    return n % mod;
  }
}

int is_alive_sim(char *str, uint_fast64_t seq) {
  uint_fast64_t n = 500 * 1000;
  n *= 1000;
  if (uint_math_mod(seq, n) == 0) {
    printf("SEQ: %lu - Is alive! %.*s\n", seq, HOST_LENGTH, str);
    return 1;
  }

  return 0;
}

void get_sequence(char *buffer, uint_fast64_t seq_idx) {
  int postfix_start = 7;

  for (int i = 0; i < POSTFIX_LENGTH; i++) {
    uint_fast64_t div = ALPHABET_LENGTH * i;
    if (div == 0) {
      div = 1;
    }
    uint_fast64_t mod = ALPHABET_LENGTH;
    uint_fast64_t res = uint_math_mod(seq_idx / div, mod);
    char new_char = ALPHABET[res];
    int buffer_pos = postfix_start + i;

    // printf("SEQ: %lu - mod: %lu - res: %lu - new_char: %c - buff_pos: %d\n",
    //        seq_idx, mod, res, new_char, buffer_pos);
    buffer[buffer_pos] = new_char;
  }
}

void compute_loads(int world_size, uint_fast64_t *total_load,
                   uint_fast64_t *world_load) {
  *total_load = 36;
  for (int i = 1; i < 7; i++) {
    *total_load *= 36;
  }
  *world_load = *total_load / (uint_fast64_t)world_size;
}

void slave_world_routine(int rank) {
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  uint_fast64_t total_load;
  uint_fast64_t world_load;
  compute_loads(world_size, &total_load, &world_load);

  uint_fast64_t start = total_load * (uint_fast64_t)rank;
  uint_fast64_t end = (start + world_load);

  // printf("%lu - %lu, Load: %lu\n", start, end, world_load);

  char current_host[] = {'h', 'o', 's', 't', '-', 'A', '-', 0,
                         0,   0,   0,   0,   0,   0,   0};

  char *host_ref = (char *)&current_host;
  for (uint_fast64_t i = start; i <= end; i++) {
    get_sequence(host_ref, i);
    // printf("Sequence: %s\n", host_ref);
    is_alive_sim(host_ref, i);
  }
}

void master_world_routine() {
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  uint_fast64_t total_load;
  uint_fast64_t world_load;
  compute_loads(world_size, &total_load, &world_load);

  printf("TOTAL LOAD: %lu\nWORLD LOAD: %lu\nWORLD_COUNT: %d\n", total_load,
         world_load, world_size);
  slave_world_routine(0);
}

int main(int argc, char **argv) {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  if (world_rank == 0) {
    master_world_routine();
  } else {
    slave_world_routine(world_rank);
  }

  MPI_Finalize();
}
