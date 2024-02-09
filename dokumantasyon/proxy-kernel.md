# proxy kernel

proxy kernel, spike'ta koşan kodumuzda, spike'ta modellenen donanımların (`processor`, `clint`, `plic`, `ns16550`, `disk`, `bus`, `remote_bitbang`,...) kabiliyeti dışında bir ihtiyaç olduğunda (mesela host os'te kalıcı bir dosyaya bir şeyler yazıp kaydetmek, terminale `ns16550`yi kullanmadan printf, scanf fonksiyonunu kullanarak yazıp okumak.) bu `fopen`, `fclose`, `fread`, `fscanf`, `fprintf`, `printf`, `scanf`, gibi fonksiyon çağrılarını host os'in anlayabileceği bir fonksiyon çağrısına dönüştürüp çağrıyla ilgili ayrıntıları hafızada `tohost` diye bir bölgeye yazar. spike'ın `htif_t` (host target interface) türünden nesnesi; `tohost`a yazılan çağrıları host os'e yönlendirir, host os'ten gelen cevapları da `fromhost` denilen bölgeye yazar. `fromhost`a yazılan cevaplar, `proxy-kernel` tarafından bizim spike'ta koşan kodumuza uygun formata dönüştürülür. Bu konuyla ilgili ayrıntılar raporda bulunacak.

## proxy kernel kurarken dikkat etmeniz gereken noktalar

proxy kernel, riscv mimarisi için derlenir, çünkü spike core'u üzerinde çalışarak ve bizim kodlarımızın sistem çağrılarını host os'in sistem çağrılarına tercüme etmesi gerekmektedir. 

O yüzden proxy-kernel riscv-gnu-toolchain kullanılarak derlenmektedir. Burada, ben kurulum yapmaya çalışırken [riscv-pk#260](https://github.com/riscv-software-src/riscv-pk/issues/260)'da bahsedilen hatayı almıştım. 
Anladığım kadarıyla problem, riscv-gnu-toolchain'in yeni versiyonlarında varsayılan olarak `zifencei` extension'u yok. Bu extension olmayınca `proxy-kernel`de assembly olarak yazılan kodların birinde geçen `fence.i` instruction'ını derleyemiyor. 

Çözüm olarak:

[riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain)'i kurarken aşağıdaki configuration ile (eklemeler olabilir):
```bash
# /opt/riscv'a kuracaginizi varsayarak
./configure --prefix=/opt/riscv --with-arch=rv64imafdc_zifencei
```


proxy-kernel'i kurarken de şu şekilde configure script'ini çalıştırıyoruz:
```bash
../configure --prefix=/opt/riscv --host=riscv64-unknown-elf --with-arch=rv64imafdc_zifencei
```
