rm -rf ./time_measures
mkdir time_measures
for f in executables/*
do
	echo ${f/*\/}
	for i in {1..500}
		do
			sudo perf stat $f 2>&1 >/dev/null | grep task-clock | awk {'print $1'} >> "./time_measures/et_${f/*\/}.txt"
		done
	
done
