#!/bin/sh

echo ========== Build ==========
make clean
qmake aromatic.pro
make
echo =========== Done ==========

echo ""
echo ""

echo =========== Run ===========
./aromatic
echo =========== Done ==========

echo ""
echo ""

echo =========== Commit report ===========
RUN_DATE=$(cat .run_date)
( cd ../aromatic-gh-pages.git/ && git pull --rebase)
cp forecast.html ../aromatic-gh-pages.git/
( cd ../aromatic-gh-pages.git/ git add forecast.html && git commit -m "Update forecast html output (run: ${RUN_DATE})." && git push )
echo =============== Done ================
