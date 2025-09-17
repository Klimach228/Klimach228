#!/bin/bash

while read prefix repo branch; do
    echo "Обновляем лабораторную $prefix... $prefix из $repo ($branch)"
    git subtree pull --prefix="$prefix" "$repo" "$branch" --squash
done < .subtrees

