#!/bin/bash
read msg
git add .
git commit -m "$msg"
git push -f
