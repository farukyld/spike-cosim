#include "cosimif.h"
#include "common.h"
#include "args_reader.h"
#include "debug_header.h"

#include <sim.h>

#define COSIM_ARGS "COSIM_ARGS"

std::function<void()> step_callback;

// !!! bu iki variable her ne kadar init'den baska bir yerde kullanilmiyor gibi gozukse de
// aslinda step_callback'e bind edildigi icin oralarda kullaniliyor.
// dolayisiyla bu iki variable, init local scope'dan disinda da kullanilabilir olmali.
// bu yuzden global scope'ta tanimliyorum.
std::queue<reg_t> fromhost_queue;
// !!! fromhost_calback reg_t alan, void donduren bir std::function
std::function<void(reg_t)> fromhost_callback;

extern sim_t* s_ptr;
extern int launch(int argc,char**argv,char**env,bool in_cosim);
void init()
{
  char* arguments_string = getenv(COSIM_ARGS);
  if (arguments_string == NULL)
  {
    fprintf(stderr,"ortam degiskeni " COSIM_ARGS " tanimlanmamis.\n");
    exit(1);
  }
  else
  {
    fprintf(stderr,"cosim argumanlari: %s\n", arguments_string);
  }

  argv_argc_t *argc_argv = split_args(arguments_string);

  argv_argc_t *spike_added_cosim_args = (argv_argc_t *) malloc(sizeof(argv_argc_t));
  spike_added_cosim_args->argc=argc_argv->argc + 1;
  spike_added_cosim_args->argv= (char**) malloc((argc_argv->argc + 1) * sizeof(char*));

  spike_added_cosim_args->argv[0] = "spike";

  for (size_t i = 0; i < argc_argv->argc; i++)
  {
    spike_added_cosim_args->argv[i+1]=argc_argv->argv[i];
  }


  launch(spike_added_cosim_args->argc,spike_added_cosim_args->argv,NULL,true);

  // sim_t::set_debug'a gerek yok.
  ((htif_t*)s_ptr)->set_expected_xlen(s_ptr->get_harts().at(0)->get_isa().get_max_xlen());

  ((htif_t*)s_ptr)->start();


  auto enq_func = [](std::queue<reg_t> *q, uint64_t x)
  { q->push(x); };

  // !!! first argument of enq_func is bind to fromhost_queue.
  // second argument is a placeholder for the reg_t argument of fromhost_callback.
  // caller passes an actual argument to the first (_1) argument of the fromhost_callback.
  fromhost_callback = std::bind(enq_func, &fromhost_queue, std::placeholders::_1);
  // !!! yani fromhost_callback, fromhost_queue'ye
  // reg_t turunden bir seyler push'lamaya yariyor

  if (((htif_t*)s_ptr)->communication_available())
  {
    // htif_t pointer'ine type-cast yapmaya gerek yoktu muhtemelen ama acik acik gostermek istedim
    step_callback = std::bind(&htif_t::single_step_with_communication, (htif_t*)s_ptr, &fromhost_queue, fromhost_callback);
  }
  else
  {
    printf("communication_available() is false\n");
    step_callback = std::bind(&htif_t::single_step_without_communication, (htif_t*)s_ptr);
  }
}

void step()
{
  step_callback();
}

svBit simulation_completed()
{
  return ((htif_t*)s_ptr)->exitcode_not_zero();
}

void get_pc(svBitVecVal* pc_o, int processor_i)
{
  *((reg_t*)pc_o) = s_ptr->get_core(processor_i)->get_state()->pc;
}


void get_log_reg_write(svBitVecVal* log_reg_write_o, int* inserted_elements_o, const int processor_i)
{
  auto map_from_c_side = s_ptr->get_core(processor_i)->get_state()->log_reg_write;
  int& num_entries = *inserted_elements_o;
  num_entries = 0;
  auto item_ptr = (commit_log_reg_item_t*) log_reg_write_o;
  
  for (auto x: map_from_c_side){
    item_ptr[num_entries].key = x.first;
    item_ptr[num_entries].value = x.second;
    num_entries++;
  }
}


void get_log_mem_read(svBitVecVal* log_mem_read_o, int* inserted_elements_o, const int processor_i){
  
  auto mem_read_vector = s_ptr->get_core(processor_i)->get_state()->log_mem_read;

  int& num_entries = *inserted_elements_o;
  num_entries = 0;


  auto item_ptr = (commit_log_mem_item_t*) log_mem_read_o; 

  for (auto x: mem_read_vector){
    item_ptr[num_entries].addr = std::get<0>(x);
    item_ptr[num_entries].paddr = std::get<1>(x);
    item_ptr[num_entries].wdata = std::get<2>(x);
    item_ptr[num_entries].len = std::get<3>(x);
    // item_ptr[num_entries].reserved = {0, 0, 0, 0, 0, 0, 0};
    num_entries++;
  }
}


void get_log_mem_write(svBitVecVal* log_mem_write_o, int* inserted_elements_o, const int processor_i){
  auto mem_write_vector = s_ptr->get_core(processor_i)->get_state()->log_mem_write;

  int& num_entries = *inserted_elements_o;
  num_entries = 0;
  
  auto item_ptr = (commit_log_mem_item_t*) log_mem_write_o;
  for (auto x: mem_write_vector){
    item_ptr[num_entries].addr = std::get<0>(x);
    item_ptr[num_entries].paddr = std::get<1>(x);
    item_ptr[num_entries].wdata = std::get<2>(x);
    item_ptr[num_entries].len = std::get<3>(x);
    num_entries++;
  }
}

void wait_key()
{
  std::cout << "press any key to continue..." << std::endl;
  std::cin.get();
}
