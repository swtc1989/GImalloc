memory_path="../../../../../memory/bin/memory"
MILU="../../Milu/bin/milu"
SUBJECT_PATH="../../../../../Subjects"

echo "Experiment: mutation sensitivity analysis"
echo "Subject: ED"
echo "November 24, 2013"

$MILU -m mop.txt -f func_list.txt malloc.c 

for index in `ls milu_output/mutants/`
do
	cd milu_output/mutants/$index
	echo "Compiling mutant $index"
	gcc -shared -fPIC src/malloc.c -o libmalloc.so
    cp $SUBJECT_PATH/bin/ed ed
    cp -r $SUBJECT_PATH/ed-1.9/testsuite/* .
#	cp $SUBJECT_PATH/fat_memory subject
#	cp $SUBJECT_PATH/mallocBase.txt mallocBase.txt
#	cp $SUBJECT_PATH/testcases.txt testcases.txt
#	cp $memory_path memory
#	./memory > result.txt
	cd ../../..
done
