#!/bin/sh
if test "X$QA_FWD" = "X" ; then 
    QA_FWD=$ALICE_PHYSICS/PWGLF/FORWARD/analysis2/qa
fi 

# 1: file
# 2: data type 
# 3: year
# 4: period
# 5: pass 
# 6: run
root -l -b -q ${QA_FWD}/RunQA.C\(\"$1\",\"$2\",$3,\"$4\",\"$5\",$6\)
cp ${QA_FWD}/style.css .
cp ${QA_FWD}/script.js .
cp ${QA_FWD}/fmd_favicon.png . 
cp ${QA_FWD}/fmd_logo.png .

#
# EOF
# 

