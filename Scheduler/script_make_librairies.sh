for dir in librairies/*
do
    cd $dir
    make
    sudo cp lib${dir/*\/}.so /usr/lib
    sudo chmod 0755 /usr/lib/lib${dir/*\/}.so
    make cleanall
    cd ../..
done
