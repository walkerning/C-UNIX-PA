csv information parse multi-procss
------------------------

数据结构在info.h里

会创建一个临时的目录, 子进程会将结果写在临时目录里的临时文件。
父进程回收所有子进程后遍历该临时目录将所有文件读取并写入最后的结果文件。

```
make
./csv_conv <csv file name> <output bin file name>
```
