# tiny-docker: the essence of docker in 100 lines of C++

To evaluate the filesystem segregation feature of this tiny-docker, you will initially need an additional ubuntu filesystem for tiny-docker execution and mounting activities.
You can arrange this using your existing ubuntu with the subsequent instructions:
```
sudo tar -cpf ubuntu-fs.tar --exclude=/home --one-file-system /
mv ubuntu-fs.tar /parent/directory/of/tiny-docker
cd /parent/directory/of/tiny-docker
mkdir ubuntu-fs
mv ubuntu-fs.tar ubuntu-fs
cd ubuntu-fs
sudo tar xf ubuntu-fs.tar
```
As I usually perform all the commands within my `$HOME` directory, `ubuntu-fs.tar` is also omitted by the `--exclude=/home` choice. If you don't execute these commands within your `$HOME`, you can manually specify `ubuntu-fs.tar`.

Following the prior setup, now you possess an `ubuntu-fs` directory located in the parent path of `tiny-docker`. You can now test it as follows:
```
make 
sudo make run
```
The output will appear as:
```
./mocker run /bin/bash
Parent running /bin/bash as 40000
Child running /bin/bash as 1
root@container:/# 
```
This indicates that hostname is separate. Furthermore, a `ps aux` will show:
```
root@container:/# ps aux
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root           1  0.0  0.0  10916  4016 ?        S    21:31   0:00 /bin/bash
root          10  0.0  0.0  12126  3504 ?        R+   21:32   0:00 ps aux
```
It is thereby clear, that pid namespace remains separate. A `ls` will display:
```
root@container:/# ls
bin  boot  cdrom  dev  etc  lib  lib32  lib64  libx32  lost+found  media  mnt  opt  proc  root  run  sbin  snap  srv  swapfile  sys  tmp  ubuntu-fs.tar  usr  var
```
The file system is also separate and you can even locate `ubuntu-fs.tar` within your `/`.

Show proc pseudo-filesystem mounts in host to establish that container mount points are not visible outside the container:
```
host@linux:~/tiny-docker$ cat /proc/mounts | grep ^proc
proc /proc proc rw,nosuid,nodev,noexec,relatime 0 0
```
