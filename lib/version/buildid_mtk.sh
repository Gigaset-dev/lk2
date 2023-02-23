#!/bin/bash

eval `date -u +'DATE=%Y%m%d%H%M'`
commit_ID=$(git log -1 --pretty=format:%h)
printf '%s_%s\n' `echo $commit_ID $DATE`
