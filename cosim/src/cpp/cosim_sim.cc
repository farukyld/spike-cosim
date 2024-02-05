#include "config.h"
#include "sim.h"
#include "mmu.h"
#include "dts.h"
#include "remote_bitbang.h"
#include "byteorder.h"
#include "platform.h"
#include "libfdt.h"
#include "socketif.h"
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include <climits>
#include <cstdlib>
#include <cassert>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

// // bunlari yapabilmek icin sim_t'nin constructor'unu da degistirmek gerekecek.
// static void cosim_handle_signal(int sig)
// {
//   if (ctrlc_pressed)
//     exit(-1);
//   ctrlc_pressed = true;
//   signal(sig, &cosim_handle_signal);
// }

void sim_t::prerun()
{
  if (!debug && log)
    set_procs_debug(true);

  htif_t::set_expected_xlen(isa.get_max_xlen());
}
