import subprocess
import re

SPIKE = "/home/faruk/araclar__arastirma/spike-cosim/riscv-isa-sim/build-cosim-io/spike"
PYLOD = "/home/faruk/araclar__arastirma/linux-spike-araclar/spike-linux-urunler/fw_payload.elf"

# --- Configurable Parameters ---

# # 1:
# LEFT = 266001194
# RIGHT = 266537888
# def cmd_with_insn_cnt(insn_cnt):
#     return [f"(make INSTRUCTIONS={insn_cnt} time_with_default_cosim_args)"]
# TIMEOUT_SECONDS = 120

# 2:
LEFT = 250000000
RIGHT = 300000000
def cmd_with_insn_cnt(insn_cnt):
    return ["bash", "-c", f"(time ({SPIKE} --instructions={insn_cnt} {PYLOD}))"]
TIMEOUT_SECONDS = 10

# # 3:
# LEFT = 281546067
# RIGHT = 281546069
# def cmd_with_insn_cnt(insn_cnt):
#     return ["bash", "-c", f"(time ({SPIKE} --instructions={insn_cnt} --log-commits {PYLOD} 2> >(cat >log.txt)))"]
# TIMEOUT_SECONDS = 540


def run_command(instruction_count, timeout):
    cmd =cmd_with_insn_cnt(instruction_count)    
    print(f"\nRunning: {' '.join(cmd)} (timeout: {timeout}s)")
    try:
        result = subprocess.run(cmd, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, timeout=timeout, text=True)
        return result.stdout.strip(), False
    except subprocess.TimeoutExpired:
        return None, True


def output_matches(output):
    # Match output ending with timing stats like:
    # ~ #
    # real	1m16,007s
    # user	1m15,929s
    # sys	0m0,077s
    pattern = r"~ #\s\nreal\s+\d+m\d+,\d+s\nuser\s+\d+m\d+,\d+s\nsys\s+\d+m\d+,\d+s"
    return re.search(pattern, output) is not None


def binary_search_cosim(left, right):
    while left < right:
        mid = (left + right) // 2
        print(f"\nCurrent range: {left} - {right}, trying mid: {mid}")

        output, timed_out = run_command(mid, TIMEOUT_SECONDS)

        if timed_out:
            print("→ Timed out. Go left.")
            right = mid
        elif output_matches(output):
            print("→ Output matched expected format. Go left.")
            right = mid
        else:
            print("→ Output did NOT match. Go right.")
            left = mid + 1

        if not timed_out:
            print("last 5 lines of the output: ")
            last_six_lines = '\n'.join(output.splitlines()[-5:])
            print(last_six_lines)

    print(f"\n✅ Final value found: {left}")


if __name__ == "__main__":
    binary_search_cosim(LEFT, RIGHT)
