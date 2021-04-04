ex_folder="JeuTestDP"

echo "algo,serie,taille,temps" > ./results.csv

for algo in {"glouton","progdyn"}; do
    for ex in $(ls ${ex_folder}); do
        size=$(echo $ex | cut -d_ -f2)
        t=$(./tp.sh -e ./${ex_folder}/${ex} -a ${algo} -t)
        echo $algo,$ex,$size,$t
    done
done >> results.csv
