[CC BY-SA](https://en.wikipedia.org/wiki/Creative_Commons_license#Types_of_licenses) lisansı altında [TÜBİTAK BİLGEM - TÜTEL](https://github.com/TUTEL-TUBITAK) bünyesinde geliştirilmiştir. 

Developed by [TÜBİTAK BİLGEM - TÜTEL](https://github.com/TUTEL-TUBITAK) under [CC BY-SA](https://en.wikipedia.org/wiki/Creative_Commons_license#Types_of_licenses). 

# spike-cosim

`cosim/src/cpp` directory'si içindeki kodlar, spike'a yapılan eklemelerin büyük bir kısmını içeriyor. (bazı eklemeler doğrudan spike'ın kaynak kodlarının içinde olması gerekti) `cosimif.cc`de cosimulation için kullanılacak fonksiyonlar var.
```cpp
// cosimif.cc'de tanimlaniyorlar
void init();
void step();
svBit simulation_completed();
void private_get_pc(svBitVecVal* pc_o, int processor_i);
void private_get_log_reg_write(const svOpenArrayHandle log_reg_write_o, int* inserted_elements_o, const int processor_i);
void private_get_log_mem_read(const svOpenArrayHandle log_mem_read_o, int* inserted_elements_o, const int processor_i);
void private_get_log_mem_write(const svOpenArrayHandle log_mem_write_o, int* inserted_elements_o, const int processor_i);
```

`cosim/src/pkg` içerisinde bu genel amaçlı fonksiyonlar SystemVerilog Direct Programming Interface (DPI) ile import'lanıyor. Bu fonksiyonların sv'de kullanımını kolaylaştırmak için çevreleyici fonksiyonlar, `enum`lar, `union`lar ve `struct`ları içeren sv `package`ları da burada.

Cosim'i kullanmak için:
- spike'ın cosim için değiştirilmiş hâlini [kurmalıyız](https://github.com/farukyld/spike-cosim?tab=readme-ov-file#spike-kurulumu).
- DPI bağlantısını kuracak olan araca (verilator/questa/xcelium, ben verilator'u kullandım);
  - cpp kaynağı olarak:
    spike kurulumunun çıktısı olan `libriscv.so` ve `libspike_dasm.a` ile `cosim/src/cpp`de bulunan kaynak kodlarını, 
  - sv kaynağı olarak:
    `cosim/src/pkg`deki dosyaları ve bu sv `package`larını kullanan bir top modul'ü (`cosim/src/tb` içerisinde örnek bir testbench var) veriyoruz.
  - `include` search path olarak (gcc `-I` flag'i) bunları tanıtmamız gerekiyor:
```makefile
  # cosim için değiştirilmiş riscv-isa-sim'in absolute path'i.
  SPIKE := 

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

(NOT: halihazırda derlemiş olduğunuz bir spike kurulumu varsa ve sıfırdan kurulmasını beklemek istemiyorsanız [kurulu_spike_uzerine.md](https://github.com/farukyld/spike-cosim/blob/main/dokumantasyon/kurulu_spike_uzerine.md)'deki adımları takip edebilirsiniz. Fakat mevcut kurulumunuzu etkilememek için submodule'u kullanmanızı tavsiye ederiz.)

([esas repo](https://github.com/riscv-software-src/riscv-isa-sim#:~:text=major%20version%20number.-,Build%20Steps,-We%20assume%20that)'nun readme'sinden de yararlanabilirsiniz)

- spike'ı kurmadan önce, [eğer riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)'i henüz kurmadıysanız, kuracağınız directory'yi `RISCV` ortam değişkeni olarak tanımlayın:
```bash
# ~/.bashrc'ye (shell acildiginda source'lanan script) ekleyebilirsiniz
export RISCV=/opt/riscv
```
```bash
# PWD = spike-cosim oldugunu varsayarak
git submodule update --init riscv-isa-sim # riscv submodule'unu guncelle
( # parantezler, shell'in icinde subshell olusturuyor. boylece "parent" shell, cd'lerden etkilenmemis oluyor.
  cd riscv-isa-sim
  [sudo] apt-get install device-tree-compiler
  mkdir build
  cd build
  ../configure --prefix=$RISCV # riscv tool'larinin kurulu oldugu directory
  [sudo] make -j8 # -j8'i degistirmek isteyebilirsiniz
)
```

***
## Verilator Kurulumu
verilator verilog kodlarını cpp/SystemC'ye dönüştürürüp derleme ve linkleme işlemininde gcc kullanımını otomatize eden bir araç. Çıktı olarak systemC/cpp (verilator kodlarının dönüştürülmüş hâli), veya çalıştırılabilir dosya oluşturabiliyor. Çıktı olarak üretilen çalıştırılabilir dosyayı koştuğumuzda simülasyonu koşmuş oluyoruz. 

**NOT:** Ben simülasyon akışını verilog içerisinde (testbench'lerle) oluşturdum. Verilator, ek seçenek olarak cpp kodu içerisindeki `main` fonksiyonunu değiştirerek simülasyon oluşturma imkanı da sağlıyor. [Şurada](https://www.youtube.com/watch?v=_VYv4MZmKR0) bununla ilgili bir örnek var.


- verilator'u clone'layıp build'liyoruz. install yapmadım, doğrudan repo'nun yerel kopyasının içinde oluşturulan binary'leri (`VERILATOR_ROOT/bin`deki exexcutable'ları) kullandım. (run-in-place)

-  `systemC` kulanmadım.

ayrıntılı bilgi için [verilator docs](https://verilator.org/guide/latest/install.html)'a bakabilirsiniz.
```bash
# Prerequisites:
#sudo apt-get install git help2man perl python3 make autoconf g++ flex bison ccache
#sudo apt-get install libgoogle-perftools-dev numactl perl-doc
#sudo apt-get install libfl2  # Ubuntu only (ignore if gives error)
#sudo apt-get install libfl-dev  # Ubuntu only (ignore if gives error)
#sudo apt-get install zlibc zlib1g zlib1g-dev  # Ubuntu only (ignore if gives error)

# PWD = spike-cosim oldugunu varsayarak
(
  cd .. # verilator'u spike-cosim repo'sunun altina clone'lamayalim, bir ust directory'ye gecelim.
  git clone https://github.com/verilator/verilator
  cd verilator
  autoconf         # Create ./configure script
  ./configure      # Configure and create Makefile
  make -j8         # Build Verilator itself
)
```

- şunları `~/.bashrc`ye (ubuntu için adı `~/.bashrc` farklı dağıtımlarda ismi farklı olabilir, bir shell açıldığında `source`lanan script.) ekliyoruz:

```bash
# git clone yaptigimizda nereye indirdiysek,
#  mesela: /home/usr1/verilator
export VERILATOR_ROOT=
export PATH=$VERILATOR_ROOT/bin:$PATH
```


***
## Cosim Örnek Kullanımı
`cosim/makefile` içindeki kurallar, cosim'i kullanan örnek testbench'i derlerken `SPIKE` environment variable'ının tanımlı olduğu varsayımı üzerine yazılmıştır. Bu variable'ı spike yerel kopyanızın bulunduğu directory'yi gösterecek şekilde mutlak yol olarak ayarlıyoruz:
```bash
# spike-cosim/riscv-isa-sim'in absolute path'i.
export SPIKE=
```
### Baremetal örnek

- cosim'i kullanan örnek testbench'i verilator ile derleyelim.

```bash
# PWD = spike-cosim oldugunu varsayarak
(
  cd cosim
  make clean_tb_spike_link
  make compile_tb_spike_link
)
```

- `ornek_test_girdileri/pk_olmadan` örneğini kullanarak bir `elf` dosyası oluşturalım. Bu elf dosyasını oluştururken [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)'in elf derleyicisini kullanacağız.

**NOT:** Eğer riscv-gnu-toolchain'i ilk defa kuracaksanız ilerde `riscv proxy-kernel`i kurarken sıkıntı yaşamamak için [proxy-kernel.md](https://github.com/farukyld/spike-cosim/blob/main/dokumantasyon/proxy-kernel.md#:~:text=Burada%2C%20ben%20kurulum%20yapmaya%20%C3%A7al%C4%B1%C5%9F%C4%B1rken)'ye göz atmanızı tavsiye ederim. Riscv proxy-kernel kullanmayacaksanız (yalnızca bare-metal kod çalıştıracaksanız) bu NOT'u görmezden gelebilirsiniz. 


```bash
# PWD = spike-cosim oldugunu varsayarak
(
  cd ornek_test_girdileri/pk_olmadan
  make all
)
```


- `tb_spike_link.exe`nin az önce oluşturduğumuz `elf` dosyasını kullanması için `args.txt` dosyasını değiştirelim. 
```bash
# PWD = spike-cosim oldugunu varsayarak
echo "spike ${PWD}/ornek_test_girdileri/pk_olmadan/outputs/hello.elf" > cosim/log/args.txt
```
`args.txt`, `cosimif.cc`deki `void init()` fonksiyonu tarafından okunuyor.

- `tb_spike_link`i koşalım:

```bash
# PWD = spike-cosim oldugunu varsayarak
cd cosim
make run_with_compile_tb_spike_link
cd ..
```
***
### proxy-kernel ile örnek

proxy-kernel'in kısa bir açıklaması için [proxy-kernel.md](dokumantasyon/proxy-kernel.md)'ye bakabilirsiniz.

#### RISCV-GNU-Toolchain kurulumu
proxy-kernel'i kurmak için önce [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)'i kurmamız gerek (çünkü pk, riscv mimarili bilgisayar üzerinde koşmak için derlenecek):

  - prerequisite'ler:
```bash
sudo apt-get install autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev
```

  NOT: burada hata alinirsa `sudo apt-get update` veya `apt-get` yerine `apt` ile deneyebilirsiniz. Yine hata alınırsa [ubuntu_paket_hatalari.md](dokumantasyon/ubuntu_paket_hatalari.md)'ye bakabilirsiniz.

  - newlib kurulumu (alternatif olarak linux kurulumu da var ama hiç gerek duymadım)

**NOT:** riscv-gnu-toolchain repo'sunu clone'lamak (git'in submodule.recurse configuration'ını true olarak ayarladıysanız.ayarlamadıysanız kurarken indirecek) uzun sürecektir. repo, submodule'lerle birlikte yaklaşık 6GB.

**NOT:** `/opt/riscv`ın içine kurulum yapılıyor.
```bash
# PWD = spike-cosim oldugunu varsayarak
(
  cd .. # spike-cosim'in icine clone'lamayalim, bir ust directory'ye gecelim
  git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
  cd riscv-gnu-toolchain
  ./configure --prefix=/opt/riscv --with-arch=rv64imafdc_zifencei
  [sudo] make -j8 # -j8'i degistirmek isteyebilirsiniz.
)
```
  - daha sonra şu satırları `~/.bashrc`ye ekleyeyin:
```bash
export RISCV=/opt/riscv
export PATH=$RISCV/bin:$PATH
```
  - bunları `~/.bashrc`ye ekledikten sonra terminali açıp kapatalım ki ortam değişkenlerinin yeni hâllerini tanısın.
***
#### proxy-kernel kurulumu

riscv proxy-kernel için:

```bash
# PWD = spike-cosim oldugunu varsayarak
(
  cd .. # spike-cosim'in icine clone'lamayalim, bir ust directory'ye gecelim
  git clone https://github.com/riscv-software-src/riscv-pk.git
  cd riscv-pk
  mkdir build
  cd build
  ../configure --prefix=/opt/riscv --host=riscv64-unknown-elf --with-arch=rv64imafdc_zifencei
  make -j8 # -j8'i degistirmek isteyebilirsiniz.
  [sudo] make -j8 install
)
```
***

- bunları yaptıktan sonra cosim'i proxy-kernel ile denemek için test girdimizi derleyelim
```bash
# PWD = spike-cosim oldugunu varsayarak
(
  cd ornek_test_girdileri/fromhost_tohost_test
  make all
)
```

- args.txt'yi az önce derlediğimiz .elf dosyasını pk ile kullanacak şekilde değiştirelim:

```bash
# PWD = spike-cosim oldugunu varsayarak
echo "spike pk ${PWD}/ornek_test_girdileri/ornek_test_girdileri/fromhost_tohost_test/a.out" > cosim/log/args.txt
```

- örnek testbench'i derleyip koşalım:

```bash
# PWD = spike-cosim oldugunu varsayarak
cd cosim
make run_with_compile_tb_spike_link
cd ..
```

