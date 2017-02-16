./server 3000 10000
for i in {0..99}
do
./client 3000 10000
echo $?, >> results.csv
done
