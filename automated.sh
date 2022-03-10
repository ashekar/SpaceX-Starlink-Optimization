#!/bin/bash
for filename in test_cases/*.txt; do
    ./a.out "$filename" | python3 evaluate.py "$filename"
done

