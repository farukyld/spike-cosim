# proxy kernel

proxy kernel, spike'ta koşan kodumuzda, spike'ta modellenen donanımların (`processor`, `clint`, `plic`, `ns16550`, `disk`, `bus`, `remote_bitbang`,...) kabiliyeti dışında bir ihtiyaç olduğunda (mesela host os'te kalıcı bir dosyaya bir şeyler yazıp kaydetmek, terminale `ns16550`yi kullanmadan printf, scanf fonksiyonunu kullanarak yazıp okumak.) bu `fopen`, `fclose`, `fread`, `fscanf`, `fprintf`, `printf`, `scanf`, gibi fonksiyon çağrılarını host os'in anlayabileceği bir fonksiyon çağrısına dönüştürüp çağrıyla ilgili ayrıntıları hafızada `tohost` diye bir bölgeye yazar. spike'ın `htif_t` (host target interface) türünden nesnesi; `tohost`a yazılan çağrıları host os'e yönlendirir, host os'ten gelen cevapları da `fromhost` denilen bölgeye yazar. `fromhost`a yazılan cevaplar, `proxy-kernel` tarafından bizim spike'ta koşan kodumuza uygun formata dönüştürülür. Bu konuyla ilgili ayrıntılar raporda bulunacak.

## proxy kernel kurarken dikkat etmeniz gereken noktalar

proxy kernel, riscv mimarisi için derlenir, çünkü spike core'u üzerinde çalışarak ve bizim kodlarımızın sistem çağrılarını host os'in sistem çağrılarına tercüme etmesi gerekmektedir. 

O yüzden proxy-kernel riscv-gnu-toolchain kullanılarak derlenmektedir. Burada, ben kurulum yapmaya çalışırken 

