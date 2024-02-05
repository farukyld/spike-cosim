#include "cosimif.h"
#include "common.h"
#include "spike__cosim_common_conf.h"
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
  auto filename = "/home/usr1/riscv-isa-sim/a_tets_faruk/spike_link/log/args.txt";
  printf(__FILE__ ":%d: reading args from file: %s\n", __LINE__, filename);

  auto argc_argv = read_args_from_file(filename);

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

void private_get_log_reg_write(const svOpenArrayHandle log_reg_write_o, int* inserted_elements_o, const int processor_i)
{
  auto map_from_c_side = simulation_object->get_core(processor_i)->get_state()->log_reg_write;

#ifdef ENABLE_SIZE_ASSERTION
#warning size assertion enabled. checking at each simulation step.
  if (unlikely(CMT_LOG_REG_ITEM_DPI_WORDS != svSize(log_reg_write_o,2))){
    std::cout << "sizeof(commit_log_reg_item_t)/sizeof(svBitVecVal) != svSize(log_reg_write_o,2)" << std::endl;
    std::cout << "sizeof(commit_log_reg_item_t)/sizeof(svBitVecVal): " << sizeof(commit_log_reg_item_t)/sizeof(svBitVecVal) << std::endl;
    std::cout << "svSize(log_reg_write_o,2): " << svSize(log_reg_write_o,2) << std::endl;
    exit(1);
  }
#endif

  int& num_entries = *inserted_elements_o;
  num_entries = 0;
  // log_reg_write_o'nun her bir elementi 192 bit. ilk 64 bit key, sonraki 128 bit value.
  for (auto x: map_from_c_side){
    commit_log_reg_item_t kvp = {x.first, x.second};
    commit_log_reg_item_t* kvp_ptr = &kvp;
    svBitVecVal* part_ptr = (svBitVecVal*) kvp_ptr;
    for (int i = 0; i < CMT_LOG_REG_ITEM_DPI_WORDS; i++, part_ptr++){
      svPutBitArrElemVecVal(log_reg_write_o, part_ptr, num_entries, i);
    }
    num_entries++;
  }
}


void private_get_log_mem_read(const svOpenArrayHandle log_mem_read_o, int* inserted_elements_o, const int processor_i){
  auto mem_read_vector = simulation_object->get_core(processor_i)->get_state()->log_mem_read;
  #ifdef ENABLE_SIZE_ASSERTION
  #warning size assertion enabled. checking at each simulation step.
  if (unlikely(CMT_LOG_MEM_ITEM_DPI_WORDS != svSize(log_mem_read_o,2))){
    // print UINT64T_W/DPI_W
    std::cout << "UINT64T_W/DPI_W: " << UINT64T_W/DPI_W << std::endl;
    // print FREGT_W/DPI_W
    std::cout << "FREGT_W/DPI_W: " << FREGT_W/DPI_W << std::endl;
    // print UINT8T_W/DPI_W
    std::cout << "UINT8T_W/DPI_W: " << UINT8T_W/DPI_W << std::endl;
    // print CMT_LOG_MEM_ITEM_DPI_WORDS
    std::cout << "CMT_LOG_MEM_ITEM_DPI_WORDS: " << CMT_LOG_MEM_ITEM_DPI_WORDS << std::endl;

    std::cout << "svSize(log_mem_read_o,2): " << svSize(log_mem_read_o,2) << std::endl;
    std::cout << "sizeof(unsigned char): " << sizeof(unsigned char) << std::endl;
    std::cout << "sizeof(svBitVecVal): " << sizeof(svBitVecVal) << std::endl;

    exit(1);
  }
  #endif

  int& num_entries = *inserted_elements_o;
  num_entries = 0;

  for (auto x: mem_read_vector){
    commit_log_mem_item_t addr_wdata_len_tuple = {std::get<0>(x), std::get<1>(x), std::get<2>(x)};

    commit_log_mem_item_t* awl_ptr = &addr_wdata_len_tuple;
    svBitVecVal* part_ptr = (svBitVecVal*) awl_ptr;
    for (int i = 0; i < CMT_LOG_MEM_ITEM_DPI_WORDS; i++, part_ptr++){
      svPutBitArrElemVecVal(log_mem_read_o, part_ptr, num_entries, i);
    }
    num_entries++;
  }
}


void private_get_log_mem_write(const svOpenArrayHandle log_mem_write_o, int* inserted_elements_o, const int processor_i){
  auto mem_write_vector = simulation_object->get_core(processor_i)->get_state()->log_mem_write;
  #ifdef ENABLE_SIZE_ASSERTION
  #warning size assertion enabled. checking at each simulation step.
  if (unlikely(CMT_LOG_MEM_ITEM_DPI_WORDS != svSize(log_mem_write_o,2))){
    // print UINT64T_W/DPI_W
    std::cout << "UINT64T_W/DPI_W: " << UINT64T_W/DPI_W << std::endl;
    // print FREGT_W/DPI_W
    std::cout << "FREGT_W/DPI_W: " << FREGT_W/DPI_W << std::endl;
    // print UINT8T_W/DPI_W
    std::cout << "UINT8T_W/DPI_W: " << UINT8T_W/DPI_W << std::endl;
    // print CMT_LOG_MEM_ITEM_DPI_WORDS
    std::cout << "CMT_LOG_MEM_ITEM_DPI_WORDS: " << CMT_LOG_MEM_ITEM_DPI_WORDS << std::endl;

    std::cout << "svSize(log_mem_write_o,2): " << svSize(log_mem_write_o,2) << std::endl;
    std::cout << "sizeof(unsigned char): " << sizeof(unsigned char) << std::endl;
    std::cout << "sizeof(svBitVecVal): " << sizeof(svBitVecVal) << std::endl;

    exit(1);
  }
  #endif

  int& num_entries = *inserted_elements_o;
  num_entries = 0;

  for (auto x: mem_write_vector){
    commit_log_mem_item_t addr_wdata_len_tuple = {std::get<0>(x), std::get<1>(x), std::get<2>(x)};

    commit_log_mem_item_t* awl_ptr = &addr_wdata_len_tuple;
    svBitVecVal* part_ptr = (svBitVecVal*) awl_ptr;
    for (int i = 0; i < CMT_LOG_MEM_ITEM_DPI_WORDS; i++, part_ptr++){
      svPutBitArrElemVecVal(log_mem_write_o, part_ptr, num_entries, i);
    }
    num_entries++;
  }
}

void wait_key()
{
  std::cout << "press any key to continue..." << std::endl;
  std::cin.get();
}
