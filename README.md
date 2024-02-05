# spike-cosim

- spike'ta değiştirmemiz gereken kaynak dosyaları var. bunları değiştirmeden önce yedeklemek isteyebilirsiniz. (eğer kendi dosyalarınızda yaptığınız değişiklikler de korunsun istiyorsanız [tam olarak neler eklenmeli]() bölümüne bakabilirsiniz.)

```bash
# riscv-isa-sim klasoru tanimla:
export SPIKE=
# yedekleme klasoru tanimla:
export SPIKE_BACKUP=$(SPIKE)/backup
mkdir -p $(SPIKE_BACKUP)
mkdir -p $(SPIKE_BACKUP)/riscv
mkdir -p $(SPIKE_BACKUP)/fesvr

cp $(SPIKE)/riscv/processor.h $(SPIKE_BACKUP)/riscv
cp $(SPIKE)/riscv/execute.cc $(SPIKE_BACKUP)/riscv
cp $(SPIKE)/riscv/sim.h $(SPIKE_BACKUP)/riscv
cp $(SPIKE)/riscv/sim.cc $(SPIKE_BACKUP)/riscv
cp $(SPIKE)/fesvr/htif.h $(SPIKE_BACKUP)/fesvr
```

değişiklik yapılmış kaynak kodlarını spike'ta ilgili yerlere kopyalıyoruz. 


NOT: bu yapacağımız işlem, ilgili yerlerde hâlihazırda aynı isimle mevcut dosyaların üzerine yazar.

```bash
cp degistirilen_kodlar/riscv/processor.h $(SPIKE)/riscv
cp degistirilen_kodlar/riscv/execute.cc $(SPIKE)/riscv
cp degistirilen_kodlar/riscv/sim.h $(SPIKE)/riscv
cp degistirilen_kodlar/riscv/sim.cc $(SPIKE)/riscv
cp degistirilen_kodlar/fesvr/htif.h $(SPIKE)/fesvr
```

spike'ın var olan derlenmiş hâlini güncelleyelim (install yapmıyoruz)

bu yapacağımız işlem spike'ın kendi çalışmasını etkilemeyecek, sadece bizim cosim'de kullanacağımız bazı fonksiyonları library'lere ekliyor.

```bash
cd $(SPIKE)/build
export N_THREADS=8
sudo make -j$(N_THREADS)
```



- cosim'i örnek testbench için derleyelim.

```bash
(
  cd cosim
  make compile_tb_spike_link
)
```

- ornek_test_girdileri/pk_olmadan örneğini kullanarak cosim'i koşmak için bir elf dosyası oluşturalım. ve cosim'de bulunan testbench'in koştuğunda `ornek_test_girdileri/pk_olmadan/outputs/hello.elf`i kullanması için args.txt dosyasını değiştirelim.
```bash
(
  cd ornek_test_girdileri/pk_olmadan
  make all
)

echo "spike ${PWD}/ornek_test_girdileri/pk_olmadan/outputs/hello.elf" > cosim/log/args.txt
```

- cosim'i örnek testbench ile koşalım:

```bash
cd cosim
make run_with_compile_tb_spike_link
```
