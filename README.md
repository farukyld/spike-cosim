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
***
## Spike Kurulumu
- spike submodule'ünü build'leyelim ama install etmeyelim. (install etmenin bir zararı yok, spike'ın normal çalışmasını etkilemez fakat gereksiz yere install etmesini beklemiş oluruz.)

(NOT: halihazırda derlemiş olduğunuz bir spike kurulumu varsa ve sıfırdan kurmakla zaman kaybetmek istemiyorsanız [kurulu_spike_uzerine.md](https://github.com/farukyld/spike-cosim/blob/main/dokumantasyon/kurulu_spike_uzerine.md)'deki adımları takip edebilirsiniz. Fakat cosim'de kullanılan güncel spike versiyonuyla devam etmek için aşağıdaki adımları tavsiye ederiz.)

([esas repo](https://github.com/riscv-software-src/riscv-isa-sim#:~:text=major%20version%20number.-,Build%20Steps,-We%20assume%20that)'nun readme'sinden de yararlanabilirsiniz)
```bash
cd riscv-isa-sim
apt-get install device-tree-compiler
mkdir build
cd build
../configure --prefix=$RISCV
make -j8

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

git clone https://github.com/verilator/verilator   # Only first time

cd verilator
git pull         # Make sure git repository is up-to-date
git tag          # See what versions exist
#git checkout master      # Use development branch (e.g. recent bug fixes)
#git checkout stable      # Use most recent stable release
#git checkout v{version}  # Switch to specified release version

autoconf         # Create ./configure script
./configure      # Configure and create Makefile
make -j8         # Build Verilator itself (if error, try just 'make')
```

şunları `~/.bashrc`ye (ubuntu için adı ~/.bashrc, bir shell açıldığında `source`lanan script.) ekliyoruz:

```bash
# git clone yaptigimizda nereye indirdiysek, mesela 
# /home/usr1/verilator
export VERILATOR_ROOT=
export PATH=$VERILATOR_ROOT/bin:$PATH
```


***
## Cosim Örnek Kullanımı

- cosim'i kullanan örnek testbench'i verilator ile derleyelim.

```bash
(
  cd cosim
  make compile_tb_spike_link
)
```

- `ornek_test_girdileri/pk_olmadan` örneğini kullanarak bir `elf` dosyası oluşturalım. Bu elf dosyasını oluştururken [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)'in elf derleyicisini kullanacağız.

NOT: Eğer riscv-gnu-toolchain'i ilk defa kuracaksanız ilerde `riscv proxy-kernel`i kurarken sıkıntı yaşamamak için [proxy-kernel.md](https://github.com/farukyld/spike-cosim/blob/main/proxy-kernel.md#:~:text=Burada%2C%20ben%20kurulum%20yapmaya%20%C3%A7al%C4%B1%C5%9F%C4%B1rken)'ye göz atmanızı tavsiye ederim. Riscv proxy-kernel kullanmayacaksanız (yalnızca bare-metal kod çalıştıracaksanız) bu NOT'u görmezden gelebilirsiniz. 


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
cd cosim
make run_with_compile_tb_spike_link
```
- proxy-kernel ile örnekler gelecek.