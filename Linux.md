# Linux

配置jdk

1、下载完jdk的tar.gz包后，进入要解压的路径，然后解压`tar zxvf jdk包.tar.gz`

2、设置环境变量`vi /etc/profile`，在最后加上三行，如下：

`export JAVA_HOME=/usr/src/jdk1.7.0_80`；`export PATH=$PATH:$JAVA_HOME/bin`;

`export   CLASSPATH=.:$JAVA_HOME/jre/lib/rt.jar:$JAVA_HOME/lib/dt.jar:$JAVA_HOME/lib/tools.jar`

```xml
for i in /etc/profile.d/*.sh ; do
    if [ -r "$i" ]; then：
        if [ "${-#*i}" != "$-" ]; then
            . "$i"
        else
            . "$i" >/dev/null
        fi
    fi
done

export JAVA_HOME=/usr/src/jdk1.7.0_80

export   CLASSPATH=.:$JAVA_HOME/jre/lib/rt.jar:$JAVA_HOME/lib/dt.jar:$JAVA_HOME/lib/tools.jar
export PATH=$PATH:$JAVA_HOME/bin

unset i
unset -f pathmunge
```

3、重新加载文件 `source /etc/profile`

4、查看jdk版本信息 `java -version`



配置tomcat

1、解压tomcat包 `tar zxvf tomcat包名`

2、配置tomcat环境变量：`vim /etc/profile`然后在最后加上`export CATALINA_HOME=/usr/src/apache-tomcat-7.0.82`，然后`source /etc/profile`（使得tomcat启动的时候是使用tomcat7版本的）

3、启动tomcat `cd /usr/src/tomcat文件名/bin`；`./startup.sh`(关闭tomcat `./shutdown.sh`)

4、打开防火墙，使外部能访问

`/sbin/iptables -I INPUT -p tcp --dport 8080 -j ACCEPT`（开放8080端口）

`service iptables save`

`service iptables restart`

(要先安装iptables-services `yum install iptables-services` ,然后设置开机启动`systemctl enable iptables`，再执行上面的操作)

5、在windows上访问tomcat主页：`ip名：8080`

如果无法访问网页，则查看日志`tail /usr/src/tomcat/logs/catalina.out`查看是什么问题

如果是Tomcat启动时卡在“INFO: Deploying web application directory ......“这一句，那么:

①、查看端口号8080、8009 `netstat -anp|grep 8080`，查看哪个进程使用了该端口，然后将该进程杀掉`kill -9 进程号`

②、修改 $JAVA_HOME/jre/lib/security/java.security 文件，替换`securerandom.source=file:/dev/random` 为 `securerandom.source=file:/dev/./urandom`

③、在开启tomcat `./startup.sh`



CentOS下配置mysql：

1、获取mysql的repo源：`wget http://repo.mysql.com/mysql-community-release-el7-5.noarch.rpm`

2、安装包：`sudo rpm -ivh mysql-community-release-el7-5.noarch.rpm`，获得两个yum repo源

3、安装mysql：`sudo yum install mysql-server`

4、重置密码：

mysql−uroot登录时有可能报这样的错：ERROR2002(HY000):Can‘tconnecttolocalMySQLserverthroughsocket‘/var/lib/mysql/mysql.sock‘(2)，原因是/var/lib/mysql的访问权限问题。下面的命令把/var/lib/mysql的拥有者改为当前 `sudo chown -R root:root /var/lib/mysql`重启服务：`service mysqld restart`

然后重置密码：

```mysql
$ mysql -u root
mysql > use mysql;
mysql > update user set password=password(‘123456‘) where user=‘root‘;
mysql > exit;
```

或者`mysqladmin -u root -p password "test123"`

5、防火墙开放3306端口，使得其他客户机也能连上mysql服务

（1）

`/sbin/iptables -I INPUT -p tcp --dport 3306-j ACCEPT`（开放3306端口）

`service iptables save`保存

`service iptables restart` 重启

（2）

`firewall-cmd --zone=public --add-port=3306/tcp --permanent`

`firewall-cmd --reload`重启防火墙

用第二种

6、重启mysql `service mysqld restart`



## 基本

Linux是一个简易的内核，经由他人加上了各种应用而衍生出了各种版本的操作系统（centos等），该操作系统的启动过程如下：

* 内核引导：CPU开始执行linux的核心代码，开始启动过程，最终linux内核调用加载了init程序，内核引导工作结束，进行init操作
* 运行init：读取/etc/inittab文件，该文件定义了整个初始化过程，包括接下来的系统初始化、建立终端以及用户登录过程
* 系统初始化
* 建立终端
* 用户登录

快捷键

* `Ctrl + C`：这个是用来终止当前命令的快捷键，当然你也可以输入一大串字符，不想让它运行直接`Ctrl + C`，光标就会跳入下一行。
* Tab： 这个键是最有用的键了，也是笔者敲击概率最高的一个键。因为当你打一个命令打一半时，它会帮你补全的。不光是命令，当你打一个目录时，同样可以补全，不信你试试。
* `Ctrl + D`： 退出当前终端，同样你也可以输入exit
* `Ctrl + Z`： 暂停当前进程，比如你正运行一个命令，突然觉得有点问题想暂停一下，就可以使用这个快捷键。暂停后，可以使用fg 恢复它。
* `Ctrl + L`： 清屏，使光标移动到第一行。

命令：`man+命令` 表示查看某个命令的相关信息

几个重要的目录：

* /bin： bin是Binary的缩写。这个目录存放着最经常使用的命令。
* /etc： 这个目录用来存放所有的系统管理所需要的配置文件和子目录。
* /var： 这个目录中存放着在不断扩充着的东西，我们习惯将那些经常被修改的目录放在这个目录下。包括各种日志文件。
* /tmp： 这个目录是用来存放一些临时文件的。
* /usr： 这是一个非常重要的目录，用户的很多应用程序和文件都放在这个目录下，类似与windows下的program files目录。
* /usr/bin：系统用户使用的应用程序。
  * /usr/sbin：超级用户使用的比较高级的管理程序和系统守护程序。
* /usr/src：内核源代码默认的放置目录。

注：/bin, /usr/bin 是给系统用户使用的指令（除root外的通用户），而/sbin, /usr/sbin 则是给root使用的指令。 /var： 这是一个非常重要的目录，系统上跑了很多程序，那么每个程序都会有相应的日志产生，而这些日志就被记录到这个目录下，具体在/var/log 目录下，另外mail的预设放置也是在这里。

正确的关机流程：sysnc ---> shutdown  ([-r/-h]) ([now/+10]) (-r表示重启，reboot；-h表示关闭，halt；+10表示十分钟后，now表示现在)



## 文件管理

### 1、文件的基本命令

各类指令其实都是在/bin目录下的，而环境变量Path为/bin的路径，因此使用命令是可以直接打出，不用使用绝对路径，以`echo $PATH`查看path路径，`PATH=”$PATH”:/root`将/root加入path中,如果将某个命令移入root中则可以直接打出命令，否则的话需要使用绝对路径，例如/root/ls

命令：

* pwd：打印当前所在目录

* ./ ：当前目录

* ../：当前目录的上一级目录

* `mkdir [-p]+目录名称`：创建目录，-p表示递归创建目录，即连同上级目录一同创建，

  ​				    `mkdir -p /tmp/test/123 `

* `rmdir [-p] + 目录`：删除空目录，-p作用与创建目录相同

* `rm [-f/-r/-i] +目录或文件`：删除目录或文件，-f表示强制，若不加此，删除一个不存在的文件时会报错；-r为当删除目录时要加该选项；-i是

  ​						提示用户是否真的要删除文件或目录

* `alias[别名]=[指令名称]`：设置指令别名，如`alias rm='rm -i' `，此时修改别名的指令是path下的rm指令

* `echo $PATH`：查看环境变量PATH，因为命令基本都在path目录下，因此输出命令时不需要使用绝对路径

* `ls` ：查看某个目录或文件列表，-a表示全部文件包括隐藏文件都列出，-l详细列出文件的属性信息，包括大小、创建日期、所属主所属组等等

  ​	   注：`alias ls='ls --color=tty'`使文件名带颜色，蓝色为目录，绿色为可执行文件

* `cp [-driu][来源文件][目的文件]` ：复制文件，-r拷贝目录，-i询问是否覆盖已存在的文件，-d确认是软连接还是硬连接，如果不加这个-d 则拷

  ​						        软连接时会把软连接的目标文件拷贝过去，而加上后，其实只是拷贝了一个连接文件（即快捷方式），-u 			

  ​							选项仅当目标文件存在时才会生效，如果源文件比目标文件新才会拷贝，否则不做任何动作。

* `touch + 文件名`：“摸”一下这个文件，如果有这个文件，则会改变文件的访问时间，如果没有这个文件就会创建这个文件

* `echo 内容 > 文件名`：将内容写入某文件中

* `mv [-ui] [来源文件] [目的文件]`：移动文件，-i询问是否覆盖，-u与cp相同；如果目的文件存在，则会把源文件移动到目标文件目录中。不存

  ​							在的话移动完后，目标文件是一个文件（相当于改了文件名）。

* `cat [-nA] 文件名`：显示文件内容，-n显示行号，-A显示所有东西包括特殊字符

* `more 文件名`：翻页查看文件，空格翻页，q退出

  * `less 文件名`：作用跟more一样，但比more好在可以上翻，下翻。空格键同样可以翻页，而按”j”键可以向下移动（按一下就向下移动一行），按”k”键向上移动。在使用more和less查看某个文件时，你可以按一下”/” 键，然后输入一个word回车，这样就可以查找这个word了。如果是多个该word可以按”n”键显示下一个。另外你也可以不按”/”而是按”?”后边同样跟word来搜索这个word，唯一不同的是，”/”是在当前行向下搜索，而”?”是在当前行向上搜索。

* `head/tail [-nf] 文件名`：显示文件前十行（head）或后十行（tail），-n显示行号，-f动态显示文件的头或最后十行，如果文件是不断增加的，则用-f 选项




### 2、文件的所属主以及所属组

​	一个linux目录或者文件，都会有一个所属主和所属组。所属主，即文件的拥有者，而所属组，即该文件所属主所在的一个组。Linux这样设置文件属性的目的是为了文件的安全。例如，test文件的所属主是user0 而test1文件的所属主是user1，那么user1是不能查看test文件的，相应的user0也不能查看test1文件。然后有这样一个应用，我想创建一个文件同时让user0和user1来查看怎么办呢？这时“所属组”就派上用场了。即，创建一个群组users，让user0和user1同属于users组，然后建立一个文件test2，且其所属组为users，那么user0和user1都可以访问test2文件。用命令`ls -l [目录]` 查看当下目录或其他目录的文件，可看到文件包括九列信息：

* 第1列，包含的东西有该文件类型和所属主、所属组以及其他用户对该文件的权限。其中，第一位用来描述该文件的类型，其中有d表示该文件

  为目录，-表示普通文件，l表示连接文件，b表示块设备文件（磁盘分区），c表示端口设备（鼠标等），s表示套接字文件（通信）。

  ​

  后边的9位，每三个为一组。均为rwx 三个参数的组合。其中r 代表可读，w代表可写，x代表可执行。前三位为所属主（user）的权限，中间三位为所属组（group）的权限，最后三位为其他非本群组（others）的权限。下面拿一个具体的例子来述说一下：

  一个文件的属性为-rwxr-xr-- ，它代表的意思是，该文件为普通文件，文件拥有者可读可写可执行，文件所属组对其可读不可写可执行，其他用户对其只可读。

  ​

  对于一个目录来讲，打开这个目录即为执行这个目录，所以任何一个目录必须要有x权限才能打开并查看该目录。例如一个目录的属性为 drwxr--r-- 其所属主为root，那么除了root外的其他用户是不能打开这个目录的。

  ​

* 第2列，表示为连接占用的节点（inode），若为目录时，通常与该目录地下还有多少目录有关系，关于连接（link）在以后章节详细介绍。

* 第3列，表示该文件的所属主。

* 第4列，表示该文件的所属组。

* 第5列，表示该文件的大小。

* 第6列、第7列和第8列为该文件的创建日期或者最近的修改日期，分别为月份日期以及时间。

* 第9列，文件名。如果前面有一个. 则表示该文件为隐藏文件。

  ​


### 3、更改文件的权限

更改文件的权限，也就是更改所属主、所属组以及他们对应的读写执行权限。

#### 1）更改所属组 chgrp

语法：`chgrp [组名][文件名]` 

#### 2）更改文件的所属主 chown

语法：`chown [ -R ] 账户名 文件名`

`chown [ -R ] 账户名：组名 文件名`（连同所属组也更改）

这里的-R选项只作用于目录，作用是级联更改，即不仅更改当前目录，连目录里的目录或者文件全部更改。

#### 3）改变用户对文件的读写执行权限 chmod

在linux中为了方便更改这些权限，linux使用数字去代替rwx ，具体规则为r: 4 w:2 x:1 -:0 举个例子，-rwxrwx---用数字表示就是 770，具体是这样来的：

rwx = 4+2+1=7; rwx= 4+2+1=7; --- = 0+0+0=0

`chmod 语法： chmod [-R] xyz 文件名` （这里的xyz，表示数字）-R 选项作用同chown，级联更改。

值得提一下的是，在linux系统中，默认一个目录的权限为 755，而一个文件的默认权限为644.

如果你创建了一个目录，而该目录不想让其他人看到内容，则只需设置成 rwxr----- (740) 即可。

chmod 还支持使用rwx的方式来设置权限！从之前的介绍中我们可以发现，基本上就九个属性分别是(1)user (2)group (3)others 三群啦！那么我们就可以藉由 u, g, o 来代表三群的属性！此外， a 则代表 all 亦即全部的三群！那么读写的属性就可以写成了 r, w, x！现在我想把一个文件设置成这样的权限 rwxr-xr-x (755)，使用这种方式改变权限的命令为：

`chomd u=rwx,og=rx test/test2`

另外还可以针对u, g, o, a增加或者减少某个权限（读，写，执行），例如

`chomd u-x test/test2`

#### 4）umask

上边也提到了默认情况下，目录权限值为766，普通文件权限值为644。那么这个值是由谁规定呢？追究其原因就涉及到了umask。

umask语法：` umask xxx `（这里的xxx代表三个数字）

查看umask值只要输入umask然后回车。 umask预设是0022，其代表什么含义？先看一下下面的规则：

* 若用户建立为普通文件，则预设“没有可执行权限”，只有rw两个权限。最大为666（-rw-rw-rw-）
* 若用户建立为目录，则预设所有权限均开放，即777（drwxrwxrwx）

umask数值代表的含义为，上边两条规则中的默认值（文件为666，目录为777）需要减掉的权限。所以目录的权限为(rwxrwxrwx) – (----w--w-) = (rwxr-xr-x)，普通文件的权限为(rw-rw-rw-) – (----w--w-) = (rw-r--r--)。umask的值是可以自定义的，比如设定umask 为 002，你再创建目录或者文件时，默认权限分别为(rwxrwxrwx) – (-------w-) = (rwxrwxr-x)和(rw-rw-rw-) – (-------w-) = (rw-rw-r--)。umask 可以在/etc/bashrc里面更改，预设情况下，root的umask为022，而一般使用者则为002，因为可写的权限非常重要，因此预设会去掉写权限。

#### 5）修改文件/目录的特殊属性

`chattr [+-=][ASaci [文件或者目录名]`

#### 6）列出文件/目录的特殊属性

` lsattr [-aR] [文件/目录名]`：-a类似与ls 的-a 选项，即连同隐藏文件一同列出；-R连同子目录的数据一同列出

一些特殊属性包括：

A：增加该属性后，文件或目录的atime将不可被修改；


S：增加该属性后，会将数据同步写入磁盘中；

a：增加该属性后，只能追加不能删除，非root用户不能设定该属性；

c：自动压缩该文件，读取时会自动解压；

i：增加后，使文件不能被删除、重命名、设定连接、写入、新增数据（不能在该目录中创建文件）；



### 4、在linux下搜索一个文件

`find [路径][参数] `：`-name filename` 直接查找该文件名的文件；`-type type` 通过文件类型查找（包括 f, b, c, d, l, s 等），例如：

​				    `find ./file -name filename`，`find ./file -type d`

### 5、软连接与硬链接

inode table（记录inode位置）--> inode（记录某档案的属性及放置在哪一个block内）-->block（记录“档案内容数据”的地区）

 Linux 在读取数据的时候，是先查询 inode table 以得到数据是放在那个 Block 里面，然后再去该 Block 里面读取真正的数据内容！

`ln [-s] 源文件名 链接名`：其中-s表示软链接，不加表示硬链接

#### 1）原理

硬链接(hard link)：

文件A是文件B的硬链接，则A的目录项中的inode节点号与B的目录项中的inode节点号相同，即一个inode节点对应两个不同的文件名，两个文件名指向同一个文件，A和B对文件系统来说是完全平等的。如果删除了其中一个，对另外一个没有影响。每增加一个文件名，inode节点上的链接数增加一，每删除一个对应的文件名，inode节点上的链接数减一，直到为0，inode节点和对应的数据块被回收。注：文件和文件名是不同的东西，rm A删除的只是A这个文件名，而A对应的数据块（文件）只有在inode节点链接数减少为0的时候才会被系统回收。

软链接(soft link)：

A是B的软链接（A和B都是文件名），A的目录项中的inode节点号与B的目录项中的inode节点号不相同，A和B指向的是两个不同的inode，继而指向两块不同的数据块。但是A的数据块中存放的只是B的路径名（可以根据这个找到B的目录项）。A和B之间是“主从”关系，如果B被删除了，A仍然存在（因为两个是不同的文件），但指向的是一个无效的链接。

#### 2）使用上限制

硬链接：

a：不能对目录创建硬链接，原因有几种，最重要的是：文件系统不能存在链接环（目录创建时的”..”除外，这个系统可以识别出来）,存在环的后果会导致例如文件遍历等操作的混乱(du，pwd等命令的运作原理就是基于文件硬链接，顺便一提，ls -l结果的第二列也是文件的硬链接数，即inode节点的链接数)

b：不能对不同的文件系统创建硬链接,即两个文件名要在相同的文件系统下（Linux的文件系统为EXT3）。

c：不能对不存在的文件创建硬链接，由原理即可知原因。

软链接：

a.可以对目录创建软链接，遍历操作会忽略目录的软链接。

b:可以跨文件系统

c:可以对不存在的文件创建软链接，因为放的只是一个字符串，至于这个字符串是不是对于一个实际的文件，就是另外一回事了

#### 3）作用

硬链接：

硬连接的作用是允许一个文件拥有多个有效路径名，这样用户就可以建立硬连接到重要文件，以防止“误删”的功能。只删除一个连接并不影响节点本身和其它的连接，只有当最后一个连接被删除后，文件的数据块及目录的连接才会被释放。也就是说，文件真正删除的条件是与之相关的所有硬连接文件均被删除。



软链接：

软链接又称之为符号连接（Symbolic Link）。软链接文件类似于Windows的快捷方式。它实际上是一个特殊的文件。在符号连接中，文件实际上是一个文本文件，其中包含的有另一文件的位置信息。



## 系统用户及用户组管理

### 1、/etc/passwd文件

执行`cat /etc/passwd`命令查看该文件可得：`root:x:0:0:root:/root:/bin/bash` `bin:x:1:1:bin:/bin:/sbin/nologin`

其中割为7个字段：

1）用户名

2）该账号的口令，存放在/etc/shadow中，用一个'x'代替

3）用户标识号uid ，默认root的uid为0，1~499为管理账号，500以上为普通用户，创建普通用户的uid大于等于500

4）组标识号gid，字段对应着/etc/group的一条记录

5）注释说明，一般记录姓名、电话等属性

6）用户的家目录，登录时就处在此目录下

7）用户登录后启动一个进程，给用户下达的指令传给内核的shell，创建用户后默认为/bin/bash，可将字段改成/sbin/nologin，不让他登录



### 2、/etc/shadow文件

执行`cat /etc/shadow`命令查看该文件可得：`root:$6$eKAQquDE/EObd8EI$B.vnu3hhDWaQMvW8DyZ7ZPvEgoR2BHjEb6wjIJ0Q3VbeP1Kd4oyIbfLXHBk3xzIuslpv9pZ85WrhNGy.1LKrX/::0:99999:7:::`
​      `bin:*:17110:0:99999:7:::`

其中割为9个字段：

1）用户名

2）用户密码，已加密过，为了安全，该文件属性设置为600，只允许root读写

3）上次修改密码的日期，距离1970年1月1日到上次更改密码的日期

4）要过几天才可以修改密码

5）密码多少天后到期。即在多少天内必须更改密码，例如这里设置成30，则30天内必须更改一次密码，否则将不能登录系统，默认是99999，可以理解为永远不需要改。

6）密码到期前的警告期限，若这个值设置成7，则表示当7天后密码过期时，系统就发出警告告诉用户，提醒用户他的密码将在7天后到期。

7）账号失效期限。你可以这样理解，如果设置这个值为3，则表示：密码已经到期，然而用户并没有在到期前修改密码，那么再过3天，则这个账号就失效了，即锁定了。

8）账号的生命周期，跟第三段一样，是按距离1970年1月1日多少天算的。它表示的含义是，账号在这个日期前可以使用，到期后账号作废。

9）作为保留用的，没有什么意义。



### 3、新增/删除用户和用户组

#### 1）新增用户组

`groupadd [-g GID] groupname`，例`groupadd [-g 505] test`，gid从500开始

#### 2）删除用户组

`groupdel groupname`

#### 3）增加用户

`useradd [-u UID] [-g GID] [-d HOME] [-M] [-s]`，其中

​			-u 自定义UID

​			-g 使其属于已经存在的某个GID

​			-d 自定义用户的家目录

​			-M 不建立家目录

​			-s 自定义shell

#### 4）删除用户

`userdel [-r] username`，-r删除cat 用户时连同家目录一起删除

### 4、创建修改密码

1）创建/修改密码：`passwd [username]`，passwd 后面不跟用户名则是更改当前用户的密码，当前用户为root。创建完账户后，默认是没有设置密			

​				 码的，虽然没有密码，但该账户同样登录不了系统。只有设置好密码后方可登录系统。

### 5、切换用户身份

1）切换用户：`su  [-] [username]`，不加username表示从普通用户切换到root用户，-表示连同环境变量一起切换过来，所存文件有所不同

2）查看当前登录的用户：`echo $LOGNAME`

3）其他普通用用户使用sudo执行一个root才能执行的命令：`sudo + 命令`，但执行的时候需要输入当前登录用户的密码。默认只有root用户能使用

sudo命令，普通用户想要使用sudo，是需要root预先设定的，即，使用`visudo`命令 编辑相关的配置文件/etc/sudoers：如果没有visudo这个命

令，请使用`yum install -y sudo`安装。默认root能够sudo是因为这个文件中有一行·`root ALL=(ALL) ALL` 在该行下面加入` test ALL=(ALL) ALL`就

可以让test用户拥有了sudo的权利。也可以将`Allows people in group wheel to run all commands`下的`# %wheel ALL=(ALL) ALL`的#去掉。使wheel这个组的用户有sudo权限，然后将用户加入wheel组



## vim编辑

vim模式下的命令

### 1、一般模式：

`Ctrl+f`屏幕向前移动一页

`Ctrl+b`屏幕向后移动一页

`0` 数字0移动到本行行首

`shift+f4`移动到本行行尾

`G`光标移动到文本最末行

`nG`移动到第n行

`gg`移动到首行

`/word ` 向光标之后寻找一个字符串名为word的字符串，当找到第一个word后，按”n”继续搜后一个

`?word`  想光标之前寻找一个字符串名为word的字符串，当找到第一个word后，按”n”继续搜前一个

`:n1,n2s/word1/word2/g` 在n1和n2行间查找word1这个字符串并替换为word2，你也可以把”/”换成”#”

`:1,$s/word1/word2/g` 从第一行到最末行，查找word1并替换成word2

`x X` x为向后删除一个字符，X为向前删除一个字符

`dd` 删除光标所在的那一行

`ndd（n为数字）`删除光标所在的向下n行

`nyy` 复制从光标所在行起向下n行

`p P` p复制的数据从光标下一行粘贴，P则从光标上一行粘贴

### 2、编辑模式：

`i` 在当前字符前插入字符

`I` 在当前行行首插入字符

`a`在当前字符后插入字符

`A`在当前行行末插入字符

`o`在当前行下插入新的一行

`O`在当前行上插入新的一行

### 3、命令模式：

`w`将编辑过的文本保存

`:w!`若文本属性为只读时，强制保存

`:q` 退出vim

`:q!` 不管编辑或未编辑都不保存退出

`:wq` 保存，退出

`:e!` 将文档还原成最原始状态

`:set nu` 在每行的行首显示行号

`:set nonu` 取消行号



## 安装RPM包或源码包

yum是安装RPM程序包的工具，用RPM安装某一个程序包有可能会因为该程序包依赖另一个程序包而无法安装，而使用yum工具就可以连同依赖的程序包一起安装

### 1、RPM工具

RPM是”Redhat Package Manager”的缩写，根据名字也能猜到这是Redhat公司开发出来的。RPM 是以一种数据库记录的方式来将你所需要的套件安装到你的Linux 主机的一套管理程序。也就是说，你的linux系统中存在着一个关于RPM的数据库，它记录了安装的包以及包与包之间依赖相关性。RPM包是预先在linux机器上编译好并打包好的文件，安装起来非常快捷。但是也有一些缺点，比如安装的环境必须与编译时的环境一致或者相当；包与包之间存在着相互依赖的情况；卸载包时需要先把依赖的包卸载掉，如果依赖的包是系统所必须的，那就不能卸载这个包，否则会造成系统崩溃。

每一个rpm包的名称都由”-“和”.”分成了若干部分。就拿` a2ps-4.13b-57.2.el5.i386.rpm` 这个包来解释一下，a2ps 为包名；4.13b则为版本信息；57.2.el5为发布版本号；i386为运行平台。其中运行平台常见的有i386, i586, i686, x86_64 ，需要你注意的是cpu目前是分32位和64位的，i386,i586和i686都为32位平台，x86_64则代表为64位的平台。另外有些rpm包并没有写具体的平台而是noarch，这代表这个rpm包没有硬件平台限制。例如 `alacarte-0.10.0-1.fc6.noarch.rpm`。

1）安装一个rpm包：`rpm [-ivh] rpm包名` -i安装的意思  -v可视化  -h显示安装进度；另外两个常用的附带参数：`--force`强制安装，即使覆盖属于其他包的文件也要安装；`--nodeps` 当要安装的rpm包依赖其他包时，即使其他包没有安装，也要安装这个包

2）升级一个rpm包：`rpm -uvh filename -U`

3）卸载一个rpm包：`rpm -e filename`

4）查询一个rpm包是否安装：`rpm -q rpm名称`（不带平台信息及后缀名）

5）查询所安装的所有rpm包：`rpm -qa`

6）得到一个rpm包的信息：`rpm -qi rpm包名`

7）列出一个rpm包安装的文件：`rpm -ql rpm包名`

8）列出某个文件属于哪个rpm包：`rpm -qf rpm包的绝对路径`

### 2、yum工具

#### a、工作原理

​	yum工作需要依赖C/S架构工作模式的文件服务器，服务器中存放了yum工作时所需的程序包。yum接收到需要安装的程序包的名称之后，通过文件共享协议(或者文件传输协议)，在配置文件中指向的yum仓库(可以是多个)中查询需要的程序包。找到之后，通过文件下载协议，将程序包下载至本地yum的缓存目录中，当安装完成后，缓存目录便会被删除

​	但如果yum每次到yum仓库都需要遍历，就会导致速度很慢。而yum仓库中有两类数据：程序和程序的元数据。yum仓库在创建时，会将所有程序的名字、大小、版本以及依赖关系这类的属性信息提取出来并保存至repodata目录中。所以在yum第一次访问yum仓库时，会获取仓库中元数据(repodata)，并下载至本地。因此，下次需要安装程序包时，只需从本地缓存中获得信息，直接到仓库下载即可

#### b、yum的配置文件

​	指向仓库的位置以及相关的各种配置信息。每个yum命令行可以同时指向多个仓库（update库，base库），仓库间可以进行优先级等相关配置，优先级是由开销决定的。配置文件有两部分组成：主配置文件以及各仓库的配置文件。因为如果所有的配置信息都放在一个文件中就显得太臃肿、不方便。其中主配置文件所在路径为/etc/yum.conf，它为各仓库指向提供公共配置文件；而各仓库的配置文件所在路径为：/etc/yum.repos.d/*.repo，里面都是以赋值的格式存在

```conf
#主配置文件
[main]#所有仓库公共的配置
cachedir=/var/cache/yum/$basearch/$releasever #缓存目录
keepcache=0 #程序包在安装完成后不再保存至缓存中
debuglevel=2 #程序安装时的输出信息，数字越大输出信息越多。生产环境中关闭最好，但是开启可以让我			  #们快速定位安装中出现问题的所在
logfile=/var/log/yum.log #日志文件
exactarch=1 #安装程序版本与当前平台一致
obsoletes=1 
gpgcheck=1 # 检查来源合法性和包完整性，还应该有个gpgkey，用于指明仓库的公钥文件从哪获取，但这		   # 是公共配置，而配置仓库都不相同，所以不放在这里
plugins=1 #支持插件
installonly_limit=5 #一次安装的程序包限制5个
bugtracker_url=http://bugs.centos.org/set_project.php?project_id=23&ref=http://bugs.centos.org/bug_report_page.php?category=yum #bug追踪路径
distroverpkg=centos-release #判定当前系统信息
```

```conf
#各配置文件
[c6-media]    # id，最好能够见名知意，并且必须唯一
name=    # 当前仓库的描述信息
#mirrorlist=    # 可以将所有的镜像站点url写入一个文件，然后指向这个文件，然后会挑一个速度最快的来使用。一般不和baseurl一起使用
baseurl=ftp://    # 指向仓库的路径，指向多个备用
       http://    # 备用的路径不能顶格写，至少需要有1个空格
       file:///media/cdrecorder/
gpgcheck=1    # 此处定义的优先级更高，需要和gpgkey配合
enabled=0   # 此仓库不被使用
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-6   
cost=    # 优先级，数字越小表示开销小，优先级越高。默认1000
[media]    # 第二个仓库
```

yum最大的优势在于可以联网去下载所需要的rpm包（baseurl以及mirrorlist网址为每个库的地址），然后自动安装，在这个工程中如果要安装的rpm包有依赖关系，yum会帮你解决掉这些依赖关系依次安装所有rpm包。

#### c、命令

1）列出所有可用的rpm包：`yum list`，例如`yum list |head -n 15`显示前15行

2）搜索一个rpm包：`yum search 相关关键词`，如`yum search vim`；除此之外还可以这样过滤显示`yum list |grep 关键词`

3）安装一个rpm包：`yum install [-y] [rpm包名]`，如果不加-y选项，则会以与用户交互的方式安装，首先是列出需要安装的rpm包信息，然后会问用户是否需要安装，输入y则安装，输入n则不安装。

4）卸载一个rpm包：`yum remove [-y][rpm包名]`，-y与上述安装相同，要注意卸载某包会连同其他包一起删除了，最好不加-y

5）升级一个rpm包：`yum update[-y][rpm包]`

6）利用yum下载一个rpm包（不自动安装）：

a、安装`yum-downloadonly`：`yum install -y yum-downloadonly.noarch`

b、下载一个rpm包而不安装：`yum install test.rpm -y --downloadonly --downloaddir=/usr/local/src `(下载到指定目录)

7）安装源码包

RPM 有分兩種：binary rpm（binary code机器跑的码） 跟 source rpm（source code人工写的代码） 。 
前者是編好的 binary ，安裝就可用。 
後者是還沒編好的 source ，需 rebuild 之後才能安裝

 源代码方式和二进制包是软件包的两种形式。二进制包里面包括了已经经过编译，可以马上运行的程序。你只需要下载和解包（安装）它们以后，就马上可以使用。源代码包里面包括了程序原始的程序代码，需要在你的计算机上进行编译以后才可以产生可以运行程序,所以从源代码安装的时间会比较长。Linux上的C语言编译器叫做gcc，利用它就可以把C语言变成可执行的二进制文件。所以如果机器上没有安装gcc就没有办法去编译源码。可以使用`yum install -y gcc` 来完成安装。

安装一个源码包，通常需要三个步骤：

* `./config` 在这一步可以定制功能，加上相应的选项即可，具有有什么选项可以通过”./config --help ”命令来查看。在这一步会自动检测你的linux系统与相关的套件是否有编译该源码包时需要的库，因为一旦缺少某个库就不能完成编译。只有检测通过后才会生成一个Makefile文件。
* `make` 使用这个命令会根据Makefile文件中预设的参数进行编译，这一步其实就是gcc在工作了。
* `make install` 安装步骤，生成相关的软件存放目录和配置文件的过程。

源码包的安装并非具有一定的标准安装步骤。这就需要在拿到源码包解压后，进入到目录找相关的帮助文档，通常会以INSTALL或者README为文件名。

①下载源码包

最好将源码包放到`/usr/local/src`目录下，用`wget`下载

`cd /usr/local/src`；`wget 下载网址`

②解压源码包

`tar zxvf xxx.tar.gz`

③ 配置相关的选项，并生成Makefile

例如将文件安装到`/usr/local/file`目录下，则用命令`./configure --prefix=/usr/local/file`，随后进行check，check结束后生成了Makefile文件

④进行编译 `make`

这一步操作，就是把源代码编译成二进制的可执行文件

⑤安装 `make install`

make install 会创建相应的目录以及文件



## 文档压缩与打包

.gz压缩文件：`gzip [-d#] filename` 其中#为1-9的数字（-d：解压缩时使用；-#：压缩等级，1压缩最差，9压缩最好，6为默认）(不可压缩目录)

.bz2压缩文件：`bzip2 [-dz] filename`（-d解压缩 -z压缩）

.tar压缩文件：`tar [-zjxcvfpP] filename`

​	z ：是否同时用gzip压缩

​	-j ：是否同时用bzip2压缩

​	-x ：解包或者解压缩

​	-t ：查看tar包里面的文件

​	-c ：建立一个tar包或者压缩文件包

​	-v ：可视化

​	-f ：后面跟文件名，压缩时跟-f文件名，意思是压缩后的文件名为filename，解压时跟-f文件名，意思是解压filename。请注意，如果是多个参数组合的情况下带有-f，请把f写到最后面。

​	--exclude：打包的时候过滤掉某些文件，如果想过滤多个文件只能继续跟`--exclude`

A、打包文件的同时使用gzip压缩：`tar -zcvf test.tar.gz test`

​      解压文件：`tar -zxvf test.tar.gz`

B、打包文件的同时使用bzip2压缩：`tar -zcvf test.tar.bz2 test`

​      解压文件：`tar -jxvf test.tar.bz2`



## 数据库基本操作

进入数据库： `mysql -u root -p`，然后输入密码

退出：`exit;`

更改密码：`mysqladmin -u root -p password "更改的密码"`，然后输入原先的密码确认就行

连接网络中某个主机上的mysql：`mysql -u user1 -p –P 3306 -h 10.0.2.69`，其中-P（大写）指定远程主机mysql的绑定端口，默认都是3306；-h指定远程主机的IP

【mysql基本命令】

`show databases;`：查询所有库

`use mysql;`：操作mysql这个库，`use + 数据库名`

`show tables;`：在使用上面的命令后查询某个库的表

`desc + 表名`：查看某表的字段

`create database + 库名`：创建数据库

`create table t1 ( `id` int(4), `name` char(40)); `：创建表1（含id ，name字段）

`mysqladmin -u root -p password "test123"`：修改mysql的root的密码

`grant all on *.* to user1 identified by '123456';`：创建一个普通用户并授权，all 表示所有的权限（读、写、查询、删除等等操作），*.*前面的*表示所有的数据库，后面的*表示所有的表，identified by 后面跟密码，用单引号括起来。这里的user1指的是localhost上的user1

`grant all on db1.* to 'user3'@'10.0.2.100' identified by '123456';`：给网络上的其他机器某个用户授权，使用'%'可表示所有主机

`mysqldump -uroot -p database table >fileName.sql`：导出某个表到fileName.sql

导入表：①`mysql -uroot -p database < fileName.sql`②`use 数据库名`命令后使用`source filename.sql`命令