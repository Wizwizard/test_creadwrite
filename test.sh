t=2
for i in  1 5 10 20 30 40 60 80 100
do
echo client_num:$i >> output
./test2 $i $t
ls | grep txt | wc -l >> output
rm *.txt
done
echo '----------' >> output
