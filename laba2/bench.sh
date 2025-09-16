#!/usr/bin/env bash
set -euo pipefail

export LC_ALL=C
export LANG=C

# Собираем генератор
g++ -std=gnu++20 -O2 -pipe -s gen_dict.cpp -o gen_dict

runs=(10000 100000 1000000)

printf "%-18s %-12s
" "Количество команд" "Время (мс)"
for n in "${runs[@]}"; do
  in="input_${n}.txt"
  out="/dev/null"   # чтобы не упираться в дисковый вывод
  ./gen_dict "$n" "$in"

  # Берём wall-clock секунды с точкой вне зависимости от локали
  sec=$( { TIMEFORMAT=%R; time ./dict < "$in" > "$out"; } 2>&1 )

  # Переводим в миллисекунды с округлением
  ms=$(python3 - "$sec" << 'PY'
import sys
s = float(sys.argv[1])
print(int(round(s * 1000)))
PY
  )

  printf "%-18s %-12s
" "$n" "$ms"
 done
