procs=`sudo screen -ls pidp | egrep '[0-9]+\.pidp' | wc -l`
echo $procs
if [ $procs -ne 0 ]; then
	sudo screen -r
fi
