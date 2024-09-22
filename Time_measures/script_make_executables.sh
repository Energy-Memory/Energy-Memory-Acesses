for dir in sequential/*
do
    cd $dir
    make
    cd ../..
done
