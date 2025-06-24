#!/bin/bash

echo bu dosya henüz son haline getirilmedi. kullanmayın.
exit 1

set -e

function yesno_prompt() {
    local prompt="$1"
    local default="Y"
    echo -n "$prompt [Y/n]: "
    read -r answer
    if [[ -z "$answer" || "$answer" =~ ^[Yy]$ ]]; then
        return 0  # yes
    else
        return 1  # no
    fi
}

echo "🔍 Spike submodule init & update..."
git submodule update --init --recursive || { echo "❌ Submodule güncellenemedi"; exit 1; }

# --- RISCV detection ---
echo "🔧 Spike derlemesi için RISCV ortam değişkeni kontrol ediliyor..."
if [ -z "$RISCV" ]; then
    echo "⚠️  RISCV tanımlı değil. Otomatik aramaya çalışılıyor..."
    candidates=$(which -a spike riscv64-unknown-elf-gcc riscv64-unknown-linux-gnu-gcc riscv64-linux-gnu-gcc 2>/dev/null | xargs -n1 dirname | uniq)
    for path in $candidates; do
        candidate=$(dirname "$path")
        if yesno_prompt "RISCV='$candidate' olarak ayarlansın mı?"; then
            export RISCV="$candidate"
            echo "✔️ RISCV='$RISCV' olarak ayarlandı."
            break
        fi
    done
    if [ -z "$RISCV" ]; then
        echo -n "Lütfen RISCV dizinini manuel girin: "
        read -r riscv_manual
        export RISCV="$riscv_manual"
    fi
else
    echo "✔️ RISCV='$RISCV' zaten tanımlı."
fi

# --- Spike Build ---
echo "🔨 Spike derleniyor..."
cd spike
mkdir -p build && cd build
../configure --prefix="$RISCV"
make -j$(nproc)
make install
cd ../../

# --- Verilator kontrol ve kurulum ---
if ! command -v verilator &> /dev/null; then
    echo "⚠️ Verilator bulunamadı."
    default_dir="/opt/verilator"
    if yesno_prompt "Verilator '$default_dir' dizinine kurulsun mu?"; then
        verilator_prefix="$default_dir"
    else
        echo -n "Lütfen başka bir dizin belirtin: "
        read -r verilator_prefix
    fi

    echo "📦 Verilator ve bağımlılıkları kuruluyor..."
    sudo apt-get update
    sudo apt-get install -y git make autoconf g++ flex bison libfl2 libfl-dev zlib1g-dev libgoogle-perftools-dev

    git clone https://github.com/verilator/verilator.git
    cd verilator
    git checkout stable
    autoconf && ./configure --prefix="$verilator_prefix"
    make -j$(nproc)
    sudo make install
    export PATH="$verilator_prefix/bin:$PATH"
    cd ..
else
    echo "✔️ Verilator zaten kurulu."
fi

# --- Örnek çalıştırma tercihi ---
if yesno_prompt "Bu repodaki örnek testbench'i kullanarak simülasyonu başlatmak ister misiniz?"; then
    echo "Hangi örneği çalıştırmak istersiniz?"
    echo "1 - fw_payload.elf (default)"
    echo "2 - baremetal.elf"
    echo "3 - pk + fhth_test.elf"
    echo -n "Seçiminiz [1/2/3]: "
    read -r example_id
    case "$example_id" in
        2) export COSIM_ARGS="--instructions=500 hazir_ornekler/baremetal.elf" ;;
        3) export COSIM_ARGS="--instructions=500 hazir_ornekler/pk hazir_ornekler/fhth_test.elf" ;;
        *) export COSIM_ARGS="hazir_ornekler/fw_payload.elf" ;;  # default
    esac

    echo "🛠️ Verilator ile testbench derleniyor..."
    make -C cosim-verilator

    echo "🚀 Simülasyon başlatılıyor..."
    ./cosim-verilator/Vcosim_ornek_kullanim
else
    echo "🔧 Kendi verilator komutunuzu elle tanımlayabilirsiniz."
    echo "Örnek verilator komutu için aşağıdakileri Makefile'a eklemelisiniz:"
    echo "--------------------------------------------------"
    echo "SRCS += cosim_pkg/*.sv"
    echo "TOP = cosim_ornek_kullanim"
    echo "CFLAGS += -DVERILATOR -I\$(INCDIR)"
    echo "LIBS += ... (gerekli .so/.a dosyaları)"
    echo "--------------------------------------------------"
    echo -n "Lütfen kendi verilator komutunuzu yazın: "
    read -r verilator_cmd
    echo "📌 Girdiğiniz komut: $verilator_cmd"
fi
