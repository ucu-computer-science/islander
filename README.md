# UCU_OS_Course_Project

## Description

**iLander** is a container engine, analog of Docker. Our container is called **isle** (pronunciation -- [il]), which is based on Linux namespaces and cgroup v1.


### Description of limit options

For more details we recommend to look in section 3 of RedHat documentation about cgroups "Subsystems and Tunable Parameters" [here](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/resource_management_guide/ch-subsystems_and_tunable_parameters).

| Option [default]     | Description |
| --------------- | ----------- |
|   --memory-in-bytes [500M]   | Memory limit (format: `<number>`[`<unit>`]). Number is a positive integer. Unit can be one of b, k, m, or g. Minimum is 4M. |
|   --cpu-shares [100]   | CPU shares (relative weight). For example, tasks in two cgroups that have cpu.shares set to 100 will receive equal CPU time, but tasks in a cgroup that has cpu.shares set to 200 receive twice the CPU time of tasks in a cgroup where cpu.shares is set to 100. The value specified in the cpu.shares file must be 2 or higher. |
|   --cpu-period [100_000]   | Limit the CPU CFS (Completely Fair Scheduler) period.  If tasks in a cgroup should be able to access a single CPU for 0.2 seconds out of every 1 second, set cpu.cfs_quota_us to 200000 and cpu.cfs_period_us to 1000000. |
|   --cpu-quota [1000_000]   | Limit the CPU CFS (Completely Fair Scheduler) quota. |
|   --device-read-bps [500M]   | Limit read rate from the host filesystem (format: `<number>`[`<unit>`]). Number is a positive integer. Unit can be one of kb, mb, or gb. |
|   --device-write-bps [100M]   | Limit write rate the host filesystem (format: `<number>`[`<unit>]`). Number is a positive integer. Unit can be one of kb, mb, or gb. |


## Examples of usage

### Type of commands

```shell
sudo ./namespaces /bin/bash

# in this case default limits will be used
sudo ./namespaces /bin/bash --memory-in-bytes 1G --cpu-quota 100000 --device-write-bps 10485760
```


### Test examples

Different rootfs tar.gz are located in isle/files dir, can be useful for testing.

**Check --memory-in-bytes and --cpu-quota flags**
```shell
isle/build$ sudo ./namespaces /bin/bash --memory-in-bytes 1G --cpu-quota 100000 --device-write-bps 10485760

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
isle/build$ sudo ./namespaces /bin/bash --memory-in-bytes 4G --cpu-quota 1000000 --device-write-bps 10485760

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


* Run this command to test --device-read-bps
```shell
sudo ./namespaces /bin/bash --device-read-bps 20975760
```

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

* Run this command to test --device-write-bps
```shell
sudo ./namespaces /bin/bash --device-write-bps 20975760
```

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




