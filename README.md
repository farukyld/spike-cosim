# spike-cosim

`cosim/src/cpp` directory'si içindeki kodlar, spike'a eklediğim fonksiyonları kullanarak genel amaçlı fonksiyonları tanımlıyorlar.
```cpp
// cosimif.cc'de tanimlaniyorlar
void init();
void step();
svBit simulation_completed();
void private_get_log_reg_write(const svOpenArrayHandle log_reg_write_o, int* inserted_elements_o, const int processor_i);
void private_get_log_mem_read(const svOpenArrayHandle log_mem_read_o, int* inserted_elements_o, const int processor_i);
void private_get_log_mem_write(const svOpenArrayHandle log_mem_write_o, int* inserted_elements_o, const int processor_i);
```

`cosim/src/pkg` içerisinde bu genel amaçlı fonksiyonlar SystemVerilog Direct Programming Interface (DPI) ile import'lanıyor. Bu fonksiyonların sv'de kullanımını kolaylaştırmak için çevreleyici fonksiyonlar, `enum`lar, `union`lar ve `struct`ları içeren sv `package`ları da burada.

Cosim'i kullanmak için:
- spike'ın cosim için değiştirilmiş hâlini kurmalıyız.
- DPI bağlantısını kuracak olan araca (verilator/questa/xcelium, ben verilator'u kullandım);
  - cpp kaynağı olarak:
    spike kurulumunun çıktısı olan `libriscv.so` ve `libspike_dasm.a` ile `cosim/src/cpp`de bulunan kaynak kodlarını, 
  - sv kaynağı olarak:
    `cosim/src/pkg`deki dosyaları ve bu sv `package`larını kullanan bir top modul'ü (`cosim/src/tb` içerisinde örnek bir testbench var) veriyoruz.
  - `include` search path olarak (gcc `-I` flag'i) bunları tanıtmamız gerekiyor:
```makefile
  SPIKE := riscv-isa-sim # spike'in yerel kopyasi
  INC_DIRS := -I$(SPIKE)/build
  INC_DIRS += -I$(SPIKE)/riscv
  INC_DIRS += -I$(SPIKE)/fesvr
  INC_DIRS += -I$(SPIKE)/
  INC_DIRS += -I$(SPIKE)/softfloat
  INC_DIRS += -I$(SPIKE)/fdt
```
***
## Spike Kurulumu
spike submodule'ünü build'leyelim ama install etmeyelim. (install etmenin bir zararı yok, spike'ın normal çalışmasını etkilemez fakat gereksiz yere install etmesini beklemiş oluruz.)

(NOT: halihazırda derlemiş olduğunuz bir spike kurulumu varsa ve sıfırdan kurmakla zaman kaybetmek istemiyorsanız [kurulu_spike_uzerine.md](https://github.com/farukyld/spike-cosim/blob/main/dokumantasyon/kurulu_spike_uzerine.md)'deki adımları takip edebilirsiniz. Fakat cosim'de kullanılan güncel spike versiyonuyla devam etmek için aşağıdaki adımları tavsiye ederiz.)

([esas repo](https://github.com/riscv-software-src/riscv-isa-sim#:~:text=major%20version%20number.-,Build%20Steps,-We%20assume%20that)'nun readme'sinden de yararlanabilirsiniz)

- spike'ı kurmadan önce, [eğer riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)'i henüz kurmadıysanız, kuracağınız directory'yi `RISCV` ortam değişkeni olarak tanımlayın:
```bash
# ~/.bashrc'ye (shell acildiginda source'lanan script) ekleyebilirsiniz
export RISCV=/opt/riscv
```
```bash
git submodule update --init riscv-isa-sim # riscv submodule'unu guncelle
( # parantezler, shell'in icinde subshell olusturuyor. boylece "parent" shell, cd'lerden etkilenmemis oluyor.
  cd riscv-isa-sim
  apt-get install device-tree-compiler
  mkdir build
  cd build
  ../configure --prefix=$RISCV # riscv tool'larinin kurulu oldugu directory
  [sudo] make -j8 # -j8'i degistirmek isteyebilirsiniz
)
```

***
## Verilator Kurulumu

verilator'u clone'layıp build'liyoruz. install yapmadım, direkt repo'nun yerel kopyasının içinde oluşturulan binary'leri (`VERILATOR_ROOT/bin`deki exexcutable'ları) kullandım. (run-in-place)

`systemC` kulanmadım.

ayrıntılı bilgi için [verilator docs](https://verilator.org/guide/latest/install.html)'a bakabilirsiniz.
```bash
# Prerequisites:
#sudo apt-get install git help2man perl python3 make autoconf g++ flex bison ccache
#sudo apt-get install libgoogle-perftools-dev numactl perl-doc
#sudo apt-get install libfl2  # Ubuntu only (ignore if gives error)
#sudo apt-get install libfl-dev  # Ubuntu only (ignore if gives error)
#sudo apt-get install zlibc zlib1g zlib1g-dev  # Ubuntu only (ignore if gives error)
(
  cd .. # verilator'u spike-cosim repo'sunun altina clone'lamayalim, bir ust directory'ye gecelim.
  git clone https://github.com/verilator/verilator
  cd verilator
  autoconf         # Create ./configure script
  ./configure      # Configure and create Makefile
  make -j8         # Build Verilator itself
)
```

şunları `~/.bashrc`ye (ubuntu için adı `~/.bashrc`, bir shell açıldığında `source`lanan script.) ekliyoruz:

```bash
# git clone yaptigimizda nereye indirdiysek, mesela 
# /home/usr1/verilator
export VERILATOR_ROOT=
export PATH=$VERILATOR_ROOT/bin:$PATH
```


***
## Cosim Örnek Kullanımı
`cosim/makefile` içindeki kurallar, cosim'i kullanan örnek testbench'i derlerken `SPIKE` environment variable'ının tanımlı olduğu varsayımı üzerine yazılmıştır. Bu variable'ı spike yerel kopyanızın bulunduğu directory'yi gösterecek şekilde mutlak yol olarak ayarlıyoruz:
```bash

export SPIKE=/home/usr1/spike-cosim/riscv-isa-sim 
```
### Baremetal örnek

- cosim'i kullanan örnek testbench'i verilator ile derleyelim.

```bash
(
  cd cosim
  make clean_tb_spike_link
  make compile_tb_spike_link
)
```

- `ornek_test_girdileri/pk_olmadan` örneğini kullanarak bir `elf` dosyası oluşturalım. Bu elf dosyasını oluştururken [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)'in elf derleyicisini kullanacağız.

NOT: Eğer riscv-gnu-toolchain'i ilk defa kuracaksanız ilerde `riscv proxy-kernel`i kurarken sıkıntı yaşamamak için [proxy-kernel.md](https://github.com/farukyld/spike-cosim/blob/main/dokumantasyon/proxy-kernel.md#:~:text=Burada%2C%20ben%20kurulum%20yapmaya%20%C3%A7al%C4%B1%C5%9F%C4%B1rken)'ye göz atmanızı tavsiye ederim. Riscv proxy-kernel kullanmayacaksanız (yalnızca bare-metal kod çalıştıracaksanız) bu NOT'u görmezden gelebilirsiniz. 


```bash
(
  cd ornek_test_girdileri/pk_olmadan
  make all
)
```
- `tb_spike_link.exe`nin az önce oluşturduğumuz `elf` dosyasını kullanması için `args.txt` dosyasını değiştirelim. 
```bash
echo "spike ${PWD}/ornek_test_girdileri/pk_olmadan/outputs/hello.elf" > cosim/log/args.txt
```
`args.txt`, `cosimif.cc`deki `void init()` fonksiyonu tarafından okunuyor.

- `tb_spike_link`i koşalım:

```bash
(
  cd cosim
  make run_with_compile_tb_spike_link
)
```
***
### proxy-kernel ile örnek

proxy-kernel'in kısa bir açıklaması için [proxy-kernel.md](https://github.com/farukyld/spike-cosim/blob/main/dokumantasyon/proxy-kernel.md)'ye bakabilirsiniz.

#### RISCV-GNU-Toolchain kurulumu
- proxy-kernel'i kurmak için önce [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)'i kurmamız gerek:

  - prerequisite'ler:
```bash
sudo apt-get install autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev
```

  NOT: burada hata alinirsa `sudo apt-get update`, yine hata alınırsa aşağıdaki sayfalara bakabilirsiniz: (burayı özetlemeliyim ~faruk)

  - https://askubuntu.com/questions/378558/unable-to-locate-package-while-trying-to-install-packages-with-apt
  
  - https://help.ubuntu.com/community/AptGet/Offline/Repository
  
  - https://packages.ubuntu.com/focal/zlibc
  
  - https://packages.ubuntu.com/focal/
  
  - https://help.ubuntu.com/community/Repositories/CommandLine

    - newlib kurulumu (alternatif olarak linux kurulumu da var ama hiç gerek duymadım)

**NOT:** riscv-gnu-toolchain repo'sunu clone'lamak uzun sürecektir. repo yaklaşık 6GB.
**NOT:** `/opt/riscv`ın içine kurulum yapılıyor.
```bash
(
  cd .. # spike-cosim'in icine clone'lamayalim, bir ust directory'ye gecelim
  git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
  cd riscv-gnu-toolchain
  ./configure --prefix=/opt/riscv --with-arch=rv64imafdc_zifencei
  [sudo] make -j8 # -j8'i degistirmek isteyebilirsiniz.
)
```
  - daha sonra şu satırları `~/.bashrc`ye ekleyebilirsiniz:
```bash
export RISCV=/opt/riscv
export PATH=$RISCV/bin:$PATH
```
***
#### proxy-kernel kurulumu

riscv proxy-kernel için:

```bash
(
  cd .. # spike-cosim'in icine clone'lamayalim, bir ust directory'ye gecelim
  git clone https://github.com/riscv-software-src/riscv-pk.git
  cd riscv-pk
  mkdir build
  cd build
  ../configure --prefix=/opt/riscv --host=riscv64-unknown-elf --with-arch=rv64imafdc_zifencei
  make -j8 # -j8'i degistirmek isteyebilirsiniz.
  [sudo [-E]] make -j8 install # -E: sudo, user environment variable'larini korumama ihtimali oldugu icin
)
```
***

- bunları yaptıktan sonra cosim'i proxy-kernel ile denemek için test girdimizi derleyelim
```bash
(
  cd ornek_test_girdileri/fromhost_tohost_test
  make all
)
```

- args.txt'yi az önce derlediğimiz .elf dosyasını pk ile kullanacak şekilde değiştirelim:

```bash
echo "spike pk ${PWD}/ornek_test_girdileri/ornek_test_girdileri/fromhost_tohost_test/a.out" > cosim/log/args.txt
```

- örnek testbench'i derleyip koşalım:

```bash
(
  cd cosim
  make run_with_compile_tb_spike_link
)
```

