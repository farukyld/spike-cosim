
### Var Olan Spike Derlemesi Üzerine Kurulum
Spike'ın kurulu bir versiyonuna zaten sahipseniz ve spike'ı sıfırdan derlemeden cosim için yapılan değişiklikleri var olan kurulumunuza eklemek için:

- spike'ta değiştirmemiz gereken kaynak dosyaları var. bunları değiştirmeden önce kendinizinkileri yedeklemek isteyebilirsiniz. 

spike (riscv-isa-sim repo'sunu indirdiğimiz directory) ve backup directory'lerini ayarlıyoruz.

```bash
# riscv-isa-sim klasoru tanimla:
export SPIKE=
# yedekleme klasoru tanimla:
export SPIKE_BACKUP=${SPIKE}/backup
mkdir -p ${SPIKE_BACKUP}
mkdir -p ${SPIKE_BACKUP}/riscv
mkdir -p ${SPIKE_BACKUP}/fesvr

timestamp=${date +%Y%m%d%H%M%S} # eski backup dosyalarinin uzerine yazmamak icin

cp ${SPIKE}/riscv/processor.h ${SPIKE_BACKUP}/riscv/processor_${timestamp}.h
cp ${SPIKE}/riscv/execute.cc ${SPIKE_BACKUP}/riscv/execute_${timestamp}.cc
cp ${SPIKE}/riscv/sim.h ${SPIKE_BACKUP}/riscv/sim_${timestamp}.h
cp ${SPIKE}/riscv/sim.cc ${SPIKE_BACKUP}/riscv/sim_${timestamp}.cc
cp ${SPIKE}/fesvr/htif.h ${SPIKE_BACKUP}/fesvr/htif_${timestamp}.h
```

- değişiklik yapılmış kaynak kodlarını spike'ta ilgili yerlere kopyalıyoruz. 


NOT: bu yapacağımız işlem, ilgili yerlerde hâlihazırda aynı isimle mevcut dosyaların üzerine yazar.

```bash
git submodule update --init riscv-isa-sim # riscv-isa-sim sumbodule'unu guncelle

cp riscv-isa-sim/riscv/processor.h ${SPIKE}/riscv
cp riscv-isa-sim/riscv/execute.cc ${SPIKE}/riscv
cp riscv-isa-sim/riscv/sim.h ${SPIKE}/riscv
cp riscv-isa-sim/riscv/sim.cc ${SPIKE}/riscv
cp riscv-isa-sim/fesvr/htif.h ${SPIKE}/fesvr
```
NOT: bu işlemi yaptıktan sonra `${SPIKE}/riscv/sim.cc`de ilk satırdaki `#include` directive'ini `cosim/src/cpp/spike__cosim_common_conf.h`i gösterecek şekilde düzeltmemiz gerekiyor. 
- spike'ın var olan derlenmiş hâlini güncelliyoruz. (install yapmıyoruz)

bu yapacağımız işlem spike'ın kendi çalışmasını etkilemeyecek, sadece bizim cosim'de kullanacağımız bazı fonksiyonları library'lere ekliyor.


```bash
cd ${SPIKE}/build
export N_THREADS=8
sudo make -j${N_THREADS}
```
