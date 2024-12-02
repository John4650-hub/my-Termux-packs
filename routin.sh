#!/bin/bash
shopt -s expand_aliases
source ~/.bashrc
gitsh
git add .
git commit -m "get completion"
git push
