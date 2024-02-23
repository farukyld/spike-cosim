#include "cosimif.h"
#include "common.h"
#include "cosim_conf.h"
#include "cosim_create_sim.h"
#include "args_reader.h"
#include "debug_header.h"


sim_t *simulation_object;

std::function<void()> step_callback;

// !!! bu iki variable her ne kadar init'den baska bir yerde kullanilmiyor gibi gozukse de
// aslinda step_callback'e bind edildigi icin oralarda kullaniliyor.
// dolayisiyla bu iki variable, init local scope'dan disinda da kullanilabilir olmali.
// bu yuzden global scope'ta tanimliyorum.
std::queue<reg_t> fromhost_queue;
// !!! fromhost_calback reg_t alan, void donduren bir std::function
std::function<void(reg_t)> fromhost_callback;


void init()
{
  #ifndef ARGS_FILE_PATH
  #define ARGS_FILE_PATH "args.txt"
  #warning ARGS_FILE_PATH is not defined. Using default value: "args.txt"
  #endif

  const char *args_filepath = ARGS_FILE_PATH;

  // route to the args.txt file from the current directory

  printf(__FILE__ ":%d: reading args from file: %s\n", __LINE__, args_filepath);

  auto argc_argv = read_args_from_file(args_filepath);

  simulation_object = create_sim_with_args(argc_argv->argc, argc_argv->argv);

  #if DEBUG_LEVEL >= DEBUG_WARN
  std::cout <<__FILE__ <<":" <<__LINE__<< " sim.cfg.startpc.hasval: " << simulation_object->get_cfg().start_pc.has_value() << std::endl;
  std::cout << "simulation object created at: " << simulation_object << std::endl;
  #endif
  simulation_object->prerun();

  #if DEBUG_LEVEL >= DEBUG_WARN
  std::cout <<__FILE__ <<":" <<__LINE__<< " sim.cfg.startpc.hasval: " << simulation_object->get_cfg().start_pc.has_value() << std::endl;
  #endif


  ((htif_t*)simulation_object)->start();

  #if DEBUG_LEVEL >= DEBUG_WARN
  std::cout <<__FILE__ <<":" <<__LINE__<< " sim.cfg.startpc.hasval: " << simulation_object->get_cfg().start_pc.has_value() << std::endl;
  #endif


  auto enq_func = [](std::queue<reg_t> *q, uint64_t x)
  { q->push(x); };

  // !!! first argument of enq_func is bind to fromhost_queue.
  // second argument is a placeholder for the reg_t argument of fromhost_callback.
  // caller passes an actual argument to the first (_1) argument of the fromhost_callback.
  fromhost_callback = std::bind(enq_func, &fromhost_queue, std::placeholders::_1);
  // !!! yani fromhost_callback, fromhost_queue'ye
  // reg_t turunden bir seyler push'lamaya yariyor

  if (((htif_t*)simulation_object)->communication_available())
  {
    printf("communication_available() is true\n");
    // htif_t pointer'ine type-cast yapmaya gerek yoktu muhtemelen ama acik acik gostermek istedim
    step_callback = std::bind(&htif_t::single_step_with_communication, (htif_t*)simulation_object, &fromhost_queue, fromhost_callback);
  }
  else
  {
    printf("communication_available() is false\n");
    step_callback = std::bind(&htif_t::single_step_without_communication, (htif_t*)simulation_object);
  }
}

void step()
{
  #if DEBUG_LEVEL >= DEBUG_WARN
  std::cout << "cosimif.cc: step. callback address: " << &step_callback << std::endl;
  #endif
  step_callback();
}

svBit simulation_completed()
{
  return ((htif_t*)simulation_object)->exitcode_not_zero();
}

void get_pc(svBitVecVal* pc_o, int processor_i)
{
  *((reg_t*)pc_o) = simulation_object->get_core(processor_i)->get_state()->pc;
}


void get_log_reg_write(svBitVecVal* log_reg_write_o, int* inserted_elements_o, const int processor_i)
{
  auto map_from_c_side = simulation_object->get_core(processor_i)->get_state()->log_reg_write;
  typedef struct { freg_t val; uint64_t key; } swapped_commit_log_reg_item_t;

  int& num_entries = *inserted_elements_o;
  num_entries = 0;
  // log_reg_write_o'nun her bir elementi 192 bit. son? 64 bit key, ilk? 128 bit value.
  // auto item_ptr = (commit_log_reg_item_t*) log_reg_write_o;
  auto swapped_item_ptr = (swapped_commit_log_reg_item_t*) log_reg_write_o;
  for (auto x: map_from_c_side){
    // commit_log_reg_item_t kvp = {x.first, x.second};
    swapped_commit_log_reg_item_t kvp_swapped = {x.second, x.first};
    // commit_log_reg_item_t* kvp_ptr = &kvp;
    swapped_commit_log_reg_item_t* kvp_swapped_ptr = &kvp_swapped;
    memcpy(swapped_item_ptr, kvp_swapped_ptr, sizeof(commit_log_reg_item_t));
    swapped_item_ptr++;
    num_entries++;
  }
}


void get_log_mem_read(svBitVecVal* log_mem_read_o, int* inserted_elements_o, const int processor_i){
  auto mem_read_vector = simulation_object->get_core(processor_i)->get_state()->log_mem_read;

  int& num_entries = *inserted_elements_o;
  num_entries = 0;

  for (auto x: mem_read_vector){
    commit_log_mem_item_t addr_wdata_len_tuple = {std::get<0>(x), std::get<1>(x), std::get<2>(x)};

    commit_log_mem_item_t* awl_ptr = &addr_wdata_len_tuple;
    svBitVecVal* part_ptr = (svBitVecVal*) awl_ptr;
    for (int i = 0; i < CMT_LOG_MEM_ITEM_DPI_WORDS; i++, part_ptr++){
      // svPutBitArrElemVecVal(log_mem_read_o, part_ptr, num_entries, i);
    }
    num_entries++;
  }
}


void get_log_mem_write(svBitVecVal* log_mem_write_o, int* inserted_elements_o, const int processor_i){
  auto mem_write_vector = simulation_object->get_core(processor_i)->get_state()->log_mem_write;

  int& num_entries = *inserted_elements_o;
  num_entries = 0;

  for (auto x: mem_write_vector){
    commit_log_mem_item_t addr_wdata_len_tuple = {std::get<0>(x), std::get<1>(x), std::get<2>(x)};

    commit_log_mem_item_t* awl_ptr = &addr_wdata_len_tuple;
    svBitVecVal* part_ptr = (svBitVecVal*) awl_ptr;
    for (int i = 0; i < CMT_LOG_MEM_ITEM_DPI_WORDS; i++, part_ptr++){
      // svPutBitArrElemVecVal(log_mem_write_o, part_ptr, num_entries, i);
    }
    num_entries++;
  }
}

void wait_key()
{
  std::cout << "press any key to continue..." << std::endl;
  std::cin.get();
}
