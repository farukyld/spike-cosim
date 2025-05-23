#include "testbench_if.h"
#include "cosimif.h"
#include "common.h"
#include "args_reader.h"


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
