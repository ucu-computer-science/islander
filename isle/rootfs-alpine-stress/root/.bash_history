ls
ls
dd if=/host_dev/zero of=/tmp/writetest bs=64k count=3200 && rm /tmp/writetest
dd -h
dd if=/host_dev/zero of=/tmp/writetest bs=64k count=3200 conv=fsync && rm /tmp/writetest
cd bin
ls
exit
