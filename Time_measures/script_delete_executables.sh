for dir in sequential/*
do
    cd $dir
    make cleanall
    cd ../..
done
