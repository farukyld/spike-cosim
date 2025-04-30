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
    }
    else
    {
      break;
    }
  }
}
