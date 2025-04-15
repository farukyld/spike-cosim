#include "cosimif.h"

int main()
{
  init();
  while (true)
  {
    if (!simulation_completed())
    {
      // for (size_t i = 0; i < 100; i++)
        step();
    }
    else
    {
      break;
    }
  }
}
