# UCU_OS_Course_Project

## Description

iLander is a container engine, analog of Docker. Our container is called **isle** (pronunciation -- [il]), which is based on Linux namespaces and cgroup v1.


## Examples of usage

### Type of commands

```shell
sudo ./namespaces sh

# in this case default limits will be used
sudo ./namespaces sh --memory-in-bytes 1G --cpu-quota 100000 --device-write-bps 10485760
```


### Test examples

Different rootfs tar.gz are located in isle/files dir, can be useful for testing.

**Check --memory-in-bytes and --cpu-quota flags**
```shell
isle/build$ sudo ./namespaces /bin/bash --memory-in-bytes 1G --cpu-quota 10000 --device-write-bps 10485760

PID: 30106


=========== /bin/bash ============
/ # stress --cpu 6
stress: info: [2] dispatching hogs: 6 cpu, 0 io, 0 vm, 0 hdd
^C
/ # stress --vm 8
stress: info: [9] dispatching hogs: 0 cpu, 0 io, 8 vm, 0 hdd
^C
/ # stress --vm 8
stress: info: [18] dispatching hogs: 0 cpu, 0 io, 8 vm, 0 hdd
^C
/ # stress --vm 2 --vm-bytes 1G
stress: info: [32] dispatching hogs: 0 cpu, 0 io, 2 vm, 0 hdd
^C
/ # exit
```


```shell
isle/build$ sudo ./namespaces /bin/bash --memory-in-bytes 4G --cpu-quota 100000 --device-write-bps 10485760

PID: 30106


=========== /bin/bash ============
/ # stress --cpu 6
stress: info: [2] dispatching hogs: 6 cpu, 0 io, 0 vm, 0 hdd
^C
/ # stress --vm 8
stress: info: [9] dispatching hogs: 0 cpu, 0 io, 8 vm, 0 hdd
^C
/ # stress --vm 8
stress: info: [18] dispatching hogs: 0 cpu, 0 io, 8 vm, 0 hdd
^C
/ # stress --vm 4 --vm-bytes 1G
stress: info: [27] dispatching hogs: 0 cpu, 0 io, 4 vm, 0 hdd
^C
/ # stress --vm 2 --vm-bytes 1G
stress: info: [32] dispatching hogs: 0 cpu, 0 io, 2 vm, 0 hdd
^C
/ # exit
```


**Check --device-read-bps flags**

* Mount **/dev/** directory to our namespace with:
```shell
mkdir ./ubuntu-rootfs/host_dev

sudo nsenter -t <PID> mount --bind /dev/ ./ubuntu-rootfs/host_dev/

# before exit from our isle run this command on host to make safe exit
sudo nsenter -t <PID> umount -R ./ubuntu-rootfs/host_dev/
```

* Run dd command in our isle

```shell
dd iflag=direct if=/tmp/readtest of=/host_dev/null bs=64K count=1600
```



**Check --device-write-bps flags**

* Mount **/dev/** directory to our namespace with:
```shell
mkdir ./ubuntu-rootfs/host_dev

sudo nsenter -t <PID> mount --bind /dev/ ./ubuntu-rootfs/host_dev/

sudo nsenter -t <PID> umount -R ./ubuntu-rootfs/host_dev/
```

* Run dd command in our isle

```shell
dd if=/host_dev/zero of=/tmp/writetest bs=64k count=1600 conv=fdatasync && rm /tmp/writetest
```


### Useful commands:

* `docker run -ti --rm containerstack/alpine-stress sh`
* `docker export ec72296fbdde | gzip > alpine-stress.tar.gz` or
 `docker export 0d95c058d6ea > alpine-stress.tar.gz`
* to check limits ou can use `glances` Linux tool
* load computer memory
```shell
stress --vm 8 --vm-bytes 1G
```

* Run dd command in host
```shell
dd if=/dev/zero of=./ubuntu-rootfs/tmp/writetest bs=64k count=3200 && rm ./ubuntu-rootfs/tmp/writetest
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




