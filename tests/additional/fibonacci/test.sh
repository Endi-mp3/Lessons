#!/usr/bin/env bash
set -euo pipefail

# Usage: ./check_fib.sh <input_file> [<app_path>]
#   <input_file>: text file, one expected Fib number per line
#   <app_path>  : path to your Fib-app (defaults to 'app')

INPUT_FILE=${1:-}
APP=${2:-app}

if [[ -z "$INPUT_FILE" || ! -r "$INPUT_FILE" ]]; then
  echo "ERROR: please provide a readable input file"
  echo "Usage: $0 <input_file> [<app_path>]"
  exit 1
fi

#
# 1. Read expected values into an indexed array
#
declare -a expected
line_no=1
while IFS= read -r line || [[ -n "$line" ]]; do
  # skip blank lines but still increment the line number
  if [[ -n "$line" ]]; then
    expected[$line_no]=$line
  fi
  (( line_no++ ))
done < "$INPUT_FILE"

#
# 2. Build the list of indexes to query
#
indexes=()
for idx in "${!expected[@]}"; do
  indexes+=("$idx")
done

#
# 3. Invoke the app and compare its output
#
pass_count=0
fail_count=0

while IFS= read -r out; do
  if [[ $out =~ ^Fib\[([0-9]+)\]=([0-9]+)$ ]]; then
    idx=${BASH_REMATCH[1]}
    got=${BASH_REMATCH[2]}
    exp=${expected[$idx]}

    if [[ "$got" == "$exp" ]]; then
      echo "INDEX $idx: PASS ($out)"
      (( pass_count++ ))
    else
      echo "INDEX $idx: FAIL — expected Fib[$idx]=$exp, got $got"
      (( fail_count++ ))
    fi
  else
    echo "UNPARSED OUTPUT: $out"
    (( fail_count++ ))
  fi
done < <("$APP" "${indexes[@]}")

#
# 4. Summary
#
echo
echo "SUMMARY: $pass_count passed, $fail_count failed"

