[CC BY-SA](https://en.wikipedia.org/wiki/Creative_Commons_license#Types_of_licenses) lisansı altında [TÜBİTAK BİLGEM - TÜTEL](https://github.com/TUTEL-TUBITAK) bünyesinde geliştirilmiştir. 

Developed by [TÜBİTAK BİLGEM - TÜTEL](https://github.com/TUTEL-TUBITAK) under [CC BY-SA](https://en.wikipedia.org/wiki/Creative_Commons_license#Types_of_licenses). 

# spike-cosim

[Spike](https://github.com/riscv-software-src/riscv-isa-sim)'ın adım adım (instruction-by-instruction) çalıştırılarak co-simülasyon tarzı kullanıma elverişli hale getirmeyi amaçlar.

İleride spike'ın hangi interruptları alacağına veya [lr/sc davranışına](https://github.com/farukyld/spike-cosim/issues/26) müdahale ederek spike'ı, doğrulamak istediğimiz işlemciyle daha uyumlu çalışmaya zorlayabilmeyi hedefliyoruz. 

# Verilator ile Kullanım
`cosim_ornek_kullanim.sv` dosyasında, `cosim_pkg.sv`'deki dpi fonksiyonlarını kullanarak spike simülasyonunu başlatma, ilerletme, bitirme sinyalini kontrol etme, spike simülasyonu tarafından yapılan register/bellek yazma/okuma işlemlerini almayla  ilgili basit bir örnek mevcut. 

`cosim-verilator`'ün `Makefile`'ında verilator ile derleme komutu mevcut. Bu komut, spike build sistemi tarafından oluşturulmuş bazı `.o` (bkz. [launch.o](#launcho)), `.a`, `.so` dosyalarını, spike'ın `.h` dosyalarını, cosim'in dpi fonksiyonlarını tanımlayan `.cc` dosyalarını ve bunların spike ile etkileşimini sağlayan `.cc` dosyalarını kullanır.

- spike submodülünü ve verilator'ü kurun. 

- [riscv-gnu-toolchain](#riscv-gnu-toolchain-kurulumu) ile derlenmiş bir elf dosyasına sahip olmalısınız. Bu repo'da birkaç küçük örnek mevcut:!!!


- daha sonra, COSIM_ARGS ortam değişkenini export'layın. Bu repo'daki örnek elf dosyalarını kullanarak şu şekilde devam edebilirsiniz:
```shell
export COSIM_ARGS="--instructions=500 hazir_ornekler/baremetal.elf"

```
 veya 
```shell
export COSIM_ARGS="--instructions=500 hazir_ornekler/pk hazir_ornekler/fhth_test.elf"
```

 veya 
```shell
export COSIM_ARGS="hazir_ornekler/fw_payload.elf"
```
> bkz: baremetal.elf
> bkz: pk
> bkz: fhth_test.elf
> bkz: fw_payload.elf

- ve  verilatör komutunuzu şu şekilde değiştirip çalıştırabilirsiniz.

*** bu işlerin hepsini yapan interaktif bir shell scripti olacak:
- spike submodule'unu kurar.
- spike'ı build'ler:
  - $RISCV değişkeni tanımlı mı diye bak,
  - değilse, sırasıyla path'te spike, riscv64-unknown-elf-gcc, riscv64-unknown-linux-gnu-gcc, riscv64-linux-gnu-gcc bak, bunların kurulu olduğu bin dizininin parent'ı olarak tanımlıyorum diye sor. hayır derse nereye tanımlayacağını sor. 
- verilator kurulu mu diye bakar
- q1: verilator'ü şuaraya kuruyorum, başka bir yere kurmak istiyor musunuz diye sorar
- verilator'ü dependency'leri ile birlikte kurar.
- q2: bu repo'daki basit testbench ile çalıştırıyorum diye sorar.
- q2-hayır denildiyse, verilator komutunu ister, değiştirilmiş verilator komutunu ve ek makefile satırlarını verir. (pkg dizinindeki .sv dosyalarını --top'tan önce koyar, -CFLAGS "-DVERILATOR $(INCDIR)diger cpp dosyaları, libraryler, vs.")
- spike'a arguman olarak verilecek cosim argumanlarını şu şekilde export'lamalısınız diye örnek verir.
- q2-evet denildiyse, fw_payload.elf (1, def), barematal.elf (2), pk fhth_test.elf (3) çalıştırıyorum diye sorar
-  
***

# Geliştirici Rehberi
`cosim-common/cosimif.cc`de hem verilator'e hem io tarzı cosim'e uyumlu şu fonksiyonlar tanımlı:
```cpp
// cosimif.cc'de tanimlaniyorlar
void init();
void step();
svBit simulation_completed();
```

`cosim-verilator/testbench_if.cc`'de, sadece verilator'ü ilgilendiren, şu fonksiyonlar tanımlı:
```cpp
void get_pc(svBitVecVal* pc_o, int processor_i);
void get_log_reg_write(const svOpenArrayHandle log_reg_write_o, int* inserted_elements_o, const int processor_i);
void get_log_mem_read(const svOpenArrayHandle log_mem_read_o, int* inserted_elements_o, const int processor_i);
void get_log_mem_write(const svOpenArrayHandle log_mem_write_o, int* inserted_elements_o, const int processor_i);
```

`cosim-verilator/src/pkg` içerisinde bu genel amaçlı fonksiyonlar SystemVerilog Direct Programming Interface (DPI) ile import'lanıyor. Bu fonksiyonların sv'de kullanımını kolaylaştırmak için `enum`lar, `union`lar ve `struct`ları içeren sv `package`ları da burada.


<!-- buradan aşağısı eski. -->
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

# Dipnotlar

#### launch.o
spike.cc'de, orijinal repo'da `main` fonksiyonu tanımlıdır. `main`i `launch` olarak yeniden isimlendirdim. `launch`'un aynı zamanda fazladan `in_cosim` diye bir argümanı var. spike.cc'ye yeni bir main fonksiyonu tanımladım, bu fonksiyon launch'u in_cosim=false olarak çağırıyor. Böylece spike'ın normal build sistemiyle oluşturulmuş spike executable'ı, müstakil bir şekilde çalışabiliyor. Aynı zamanda spike.o'dan main fonksiyonunu kaldırınca (objcopy --strip-symbol=main spike.o launch.o) geriye kalan `.o` dosyası, cosim executable'ının içine derlenebiliyor. ve launch fonksiyonu, cosim tarafından in_cosim=true olarak çağrıldığında cosim'e uygun şekilde simülasyonu oluşturuyor.