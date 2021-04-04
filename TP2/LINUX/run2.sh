ex_folder="JeuTest1000plus"

echo "algo,serie,taille,distance" > ./glouton1000DIST.csv

for ex in $(ls ${ex_folder}); do
    size=$(echo $ex | cut -d_ -f2)
    t=$(./tp.sh -e ./${ex_folder}/${ex} -a glouton)
    echo $algo,$ex,$size,$t
done >> glouton1000DIST.csv
