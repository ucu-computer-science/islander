# UCU_OS_Course_Project

### Useful commands:

* docker run -ti --rm containerstack/alpine-stress sh
* docker export ec72296fbdde | gzip > alpine-stress.tar.gz or docker export 0d95c058d6ea > alpine-stress.tar.gz
* load computer memory
```shell
stress --vm 8 --vm-bytes 1G
```

### Examples of usage

```shell
UCU_OS_Course_Project/namespaces/binary$ sudo ./namespaces sh
```