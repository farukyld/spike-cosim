# cosim_pkg

paket dosyalarının isimlendirmesinde _0, _1 gibi ön-ekler kullandım. verilator paketler  arası bağlılık ilişkisini otomatik şekilde çözemiyordu, dosyalar isimlerine göre sıralansın diye _0 _1 gibi ön-ekler ile isimlendirdim. 

yüksek numaralılar düşük numaralıları kullanıyor. _ ile başlamayanlar _ ile başlayanları kulalnıyor.
***
`cosim_constants_pkg`, kullandığımız repolar (spike, verilator) ve SystemVerilog DPI'a bağlı sabitler. Çok büyük ihtimalle hiç değişmeyecekler.
***
`basic_types_pkg` paketinde `cosim_constants_pkg`de tanımlanan sabit bit genişliklerine göre 2-state türlere tanımlanıyor. yine bunlar da değişmeyecek. (ancak cosim'in sv arayüzünü değiştirmeye karar verirsek -yine çok küçük bir ihtimalle- değiştirmemiz gerekebilir) 
***
`csr_ids_pkg` spike'tan kopyalayarak oluşturduğum bir enum. kendi başına çok büyük olduğu için ayrı bir dosyaya aldım. normalde `cosim_pkg`de bulunacaktı. 
***
`private_dpi_imports`da fonksiyonlar c tarafından verilerin alındığı fonksiyonların import'ları var. bunları testbench'te direkt kullanmayın. sebebi rapor'da açıklanacak. bunlardan alınan veri `cosim_pkg`de SystemVerilog struct'larına uygun şekilde dönüştürülüyor.

bu package içinde `CommitLogEntries` diye bir sabit var, tek instruction'da spike, mem-read/mem-write/reg-write işlemlerinin her birinden en fazla kaç tane yapabilir.

mesela: bir instruction tek seferde 6 tane register'a yazıyorsa `CommitLogEntries` >= 6 olmalı. 

mesela: `huge` diye uydurma bir instruction var diyelim.  tek seferde 10 tane register'a yazıyor, 6 tane word (64 bit) okuyor, 8 tane word yazıyorsa `CommitLogEntries` >= 10 olmalı. çünkü üç tür (reg_write, mem_read, mem_write) ayrı array'lerde tutuluyor.

ben `CommitLogEntries` parametresini 16 koydum.  muhtemelen gereğinden fazla. ama fazla olmasının sorun olacağını zannetmiyorum çünkü verilator'un gerekli free'lemeleri yaptığını düşünüyorum. proxy-kernel'la yaptığım cosimulation'larda bile heap-overflow veya stack-overflow sorunu yaşamadım. eğer free, delete, dealloc vesaire yapılmıyor olsaydı sorun yaşamış olurdum. 



***

`cosim_pkg`, normal şartlar altında testbench'lerimizde kullanacağımız her şeyi içeriyor olmalı. eğer içermiyorsa benim tasarımımda bir sıkıntı vardır muhtemelen. 

cosim'i oluşturmak için: 
```verilog
void init()
```

cosim'i bir "simülasyon adımı" (detayları raporda) ilerletmek için:
```verilog
void step()
```

cosim'in bitirme sinyali göndderip göndermediğine bakmak için:
```verilog
bit simulation_completed()
```

cosim'de son adımda register'lara yapılan yazmalara bakmak için:
```verilog
get_log_reg_write(
  output commit_log_reg_item_t log_reg_write_o[CommitLogEntries]
  output int inserted_elements_o
  )`
```
bu fonksiyon, spike tarafından son çalıştırılan instruction'un yazdığı register'ları `log_reg_write_o` array'inin içine ekliyor. kaç tane eleman eklenmişse `inserted_elements_o`yu ona ayarlıyor.

`log_reg_write_o` şu türden bir array:

```verilog

  typedef struct packed {
    reg_key_t key;
    freg_t value; // 128 bit, `basic_types_pkg` icinde tanimli.
  } commit_log_reg_item_t;
```

bu türü tanımlarken kullanılan türler:

```verilog
  // reg_key_t de su sekilde
  typedef union packed {
    reg_t key; // 64 bit tek parca, `basic_types_pkg` icinde tanimli.
    key_parts_t key_parts; // 64 bit key {60 bit id, 4 bit type}
  } reg_key_t;


  // key parts tanimi:
  typedef struct packed {
    reg_id_t reg_id; // high 64 bit
    reg_key_type_e reg_type; // low 4 bit.
  } key_parts_t;

  // register key type enum
  // from riscv/decode_macros.h
  typedef enum bit [REG_KEY_TYPE_W-1:0] {
    XREG      = REG_KEY_TYPE_W'('b0000),
    FREG      = REG_KEY_TYPE_W'('b0001),
    VREG      = REG_KEY_TYPE_W'('b0010),
    VREG_HINT = REG_KEY_TYPE_W'('b0011),
    CSR       = REG_KEY_TYPE_W'('b0100)
  } reg_key_type_e;

  // csr_id'ler duz 0'dan 31'e dseklinde degil. onlar icin bu enum'u kullaniyoruz.
  typedef csr_ids_pkg::csr_id_e csr_id_e;

  // diger id'ler duz 0'dan 31'e
  typedef union packed {
    bit [REG_KEY_ID_W-1:0] int_float_vec;
    csr_id_e csr;
  } reg_id_t;
```

