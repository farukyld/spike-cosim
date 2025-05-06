#include "cosimif.h"
#include "common.h"
#include "args_reader.h"

extern int launch(int argc, char **argv, char **env, bool in_cosim);

#define COSIM_ARGS "COSIM_ARGS"

void (*step_callback)();

// !!! bu iki variable her ne kadar init'den baska bir yerde kullanilmiyor gibi gozukse de
// aslinda step_callback'e bind edildigi icin oralarda kullaniliyor.
// dolayisiyla bu iki variable, init local scope'dan disinda da kullanilabilir olmali.
// bu yuzden global scope'ta tanimliyorum.
std::queue<reg_t> fromhost_queue;
// !!! fromhost_calback reg_t alan, void donduren bir std::function
std::function<void(reg_t)> fromhost_callback;

void step_callback_with_comm()
{
  ((htif_t *)s_ptr)->single_step_with_communication(&fromhost_queue, fromhost_callback);
}

void step_callback_without_comm()
{
  ((htif_t *)s_ptr)->single_step_without_communication();
}

void init()
{
  char *arguments_string = getenv(COSIM_ARGS);
  if (arguments_string == NULL)
  {
    fprintf(stderr, "ortam degiskeni " COSIM_ARGS " tanimlanmamis.\n");
    exit(1);
  }
  else
  {
    fprintf(stderr, "cosim argumanlari: %s\n", arguments_string);
  }

  argv_argc_t *argc_argv = split_args(arguments_string);

  argv_argc_t *spike_added_cosim_args = (argv_argc_t *)malloc(sizeof(argv_argc_t));
  spike_added_cosim_args->argc = argc_argv->argc + 1;
  spike_added_cosim_args->argv = (char **)malloc((argc_argv->argc + 1) * sizeof(char *));

  spike_added_cosim_args->argv[0] = (char *)malloc(sizeof("spike"));
  strcpy(spike_added_cosim_args->argv[0], "spike");

  for (size_t i = 0; i < argc_argv->argc; i++)
  {
    spike_added_cosim_args->argv[i + 1] = argc_argv->argv[i];
  }

  launch(spike_added_cosim_args->argc, spike_added_cosim_args->argv, NULL, true);

  // sim_t::set_debug'a gerek yok.
  ((htif_t *)s_ptr)->set_expected_xlen(s_ptr->get_harts().at(0)->get_isa().get_max_xlen());

  ((htif_t *)s_ptr)->start();

  auto enq_func = [](std::queue<reg_t> *q, uint64_t x)
  { q->push(x); };

  // !!! first argument of enq_func is bind to fromhost_queue.
  // second argument is a placeholder for the reg_t argument of fromhost_callback.
  // caller passes an actual argument to the first (_1) argument of the fromhost_callback.
  fromhost_callback = std::bind(enq_func, &fromhost_queue, std::placeholders::_1);
  // !!! yani fromhost_callback, fromhost_queue'ye
  // reg_t turunden bir seyler push'lamaya yariyor

  if (((htif_t *)s_ptr)->communication_available())
  {
    // htif_t pointer'ine type-cast yapmaya gerek yoktu muhtemelen ama acik acik gostermek istedim
    step_callback = step_callback_with_comm;
  }
  else
  {
    printf("communication_available() is false\n");
    step_callback = step_callback_without_comm;
  }
}

void step()
{
  step_callback();
}

uint8_t simulation_completed()
{
  return ((htif_t *)s_ptr)->exitcode_has_value();
}
