#include "cosimif.h"

int main()
{
  init(); // spike simulasyon init

  while (true)
  {
    if (!simulation_completed())
    {
      // for (size_t i = 0; i < 100; i++)
      step(); // spike simulasyon step
      reg_t pc = s_ptr->get_core(0)->get_state()->pc;
      fwrite(&pc, sizeof(reg_t), 1, stderr);
    }
    else
    {
      break;
    }
  }
}
