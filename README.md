# UCU_OS_Course_Project

### Useful commands:

* docker run -ti --rm containerstack/alpine-stress sh
* docker export ec72296fbdde | gzip > alpine-stress.tar.gz or docker export 0d95c058d6ea > alpine-stress.tar.gz
* to check limits ou can use `glances` Linux tool
* load computer memory
```shell
stress --vm 8 --vm-bytes 1G
```

* block io measure
```shell
(base) root@denys-herasymuk-Strix-15-GL503GE:/# time bash -c "dd if=/home/writetest of=/home/writetest2 bs=64k count=3200 && rm /home/writetest2"
3200+0 records in
3200+0 records out
real	0m 0.18s
user	0m 0.00s
sys	0m 0.17s
```

### Examples of usage

```shell
UCU_OS_Course_Project/namespaces/binary$ sudo ./namespaces sh

sudo ./namespaces sh --memory-in-bytes 1G --cpu-quota 100000 --device-write-bps 10485760
```
