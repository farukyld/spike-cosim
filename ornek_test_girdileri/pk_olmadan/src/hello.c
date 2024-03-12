

// volatile long int tohost;
// volatile long int fromhost;

#define SIZE 1000

long int modular_random() __attribute__((section(".text2")));


volatile static long long int magic_mem[8] = {1,0,0,0,0,0,0,1}; // ismi onemli degil.


volatile static long long int* tohost __attribute__((used));// bunlarin ismi onemli
volatile static long long int* fromhost __attribute__((used));// 

void baremetal_exit(long long int exit_code) __attribute__((section(".text2")));

// volatile long long int number = 0x123456789abcdef0;
void main() __attribute__((section(".text2")));
void main() 
{

//   int a[261878];
//   (*(void (*)())(&(a[261878])))(); // stack end
//   (*(void (*)())(&(a[259857])))(); // bu da calisiyor, text'te bir yere denk geliyor
  int a[SIZE];
  // number = number << 64; // etkisiz. (shamt % 64)

  for (int ii = 0; ii < SIZE; ii++)
  {
    a[ii] = modular_random();
  }
  // number = 43 / 0; // ayni kod satiri burada yazinca patliyor.
  __asm(
    "nop\n"
    "nop\n");
    baremetal_exit(0);
}



long int modular_random()
{
  static long int state = -1; // Static state variable

  // Constants for the linear congruential generator
  long int a = 1664525;    // Multiplier
  long int c = 1013904223; // Increment
  long int m = 4294967296; // Modulus (2^32)

  // Update the state using the LCG formula
  state = (a * state + c) % m;

  return state;
}


void baremetal_exit(long long int exit_code){
  magic_mem[1] = exit_code;
  magic_mem[0] = 93; // 93: exit 
  // see riscv-isa-sim/fesvr/syscall.cc }}
  // syscall_t::syscall_t
  for (int i = 2; i < 8; i++)
    magic_mem[i] = 0;

  tohost = magic_mem;

  while (!fromhost);
  fromhost = 0;
  while (1);
}
