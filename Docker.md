# Docker

## 一、概述

### 1、docker技术简述

​	Docker是一套轻量级操作系统虚拟化工具，它由go语言编写，作用类似于虚拟机。它基于Linux容器技术（LXC），Namespace，Cgroup，UnionFS（联合文件系统）等技术。以下是对这几个技术的简单描述：

**namespace(命名空间)**：每个容器自己单独有一个命名空间，实现了进程和进程所使用的资源隔离，是进程彼此不可见

**cgroup(控制组)(controll group)**：控制分配到容器的资源，避开多个容器同时运行时对系统资源的竞争

**UnionFs(联合文件系统)**：Union文件系统（UnionFS）是一种分层、轻量级并且高性能的文件系统，它支持对 文件系统的修改作为一次提交来一层层的叠加，同时可以将不同目录挂载到同一个虚拟文件系统下(unite several directories into a single virtual filesystem)。UnionFS用到了一个资源管理技术：写时复制(`copy-on-write`)，即在原有的文件层上如果有文件修改的话，会再创建一个新的文件层叠加在上面；如果没有文件修改，则不需要创建新的资源。

Union 文件系统是 Docker 镜像的基础。镜像可以通过分层来进行继承，基于基础镜像（没有父镜像），可以制作各种具体的应用镜像。例如，基于centos环境下的docker容器中搭建jdk环境的镜像。另外，不同 Docker 容器就可以共享一些基础的文件系统层，同时再加上自己独有的改动层，大大提高了存储的效率。

Docker使用的简单流程：

![](http://ot0aou666.bkt.clouddn.com/docker.jpg)



#### a、镜像

**docker镜像是什么：**

​	**docker的镜像实际上由一层一层的文件系统组成**，这种层级的文件系统就是上文说到的UnionFS。在Docker镜像的最底层是bootfs。这一层与Linux/Unix系统是一样的，包含boot加载器和内核。当boot加载完成之后整个内核就都在内存中了，此时内存的使用权已由bootfs转交给内核，此时系统也会卸载bootfs。Docker在bootfs之上的一层是rootfs（根文件系统），或者说是各种不同的操作系统发行版，比如Ubuntu，Centos等等。

Docker镜像结构层图如下：

![](http://ot0aou666.bkt.clouddn.com/docker%E9%95%9C%E5%83%8F%E7%BB%93%E6%9E%84%E5%B1%82%E5%9B%BE.png)

​	传统的Linux加载bootfs时会先将rootfs设为read-only，然后在系统自检之后将rootfs从read-only改为read-write。然后我们就可以在rootfs上进行写和读的操作了。但docker的镜像却不是这样，他在bootfs自检完毕之后并不会把每层文件的read-only改为read-write。而是利用union mount（UnionFS的一种挂载机制）将一个或多个read-only的文件层加载到之前的read-only 的文件层之上。并在加载了这么多层的文件层之后，仍然让它看起来只像一个文件系统，在docker的体系里把union mount的这些read-only层的文件层叫做docker的镜像（image）。请注意，此时的每一层文件层都是read-only的，也就是说我们此时还不能对其进行操作，那么我们怎样对其进行读写操作呢？

​	答案是将docker镜像进行实例化，就是上文说的从镜像（image）变成容器（container）的过程，当镜像被实例化为容器之后，系统会为在一层或是多层的read-only的rootfs之上分配一层空的read-write的rootfs。而这个分配的动作是由docker  run命令发起的。

​	当我们将一个镜像实例化为一个容器之后，docker会在read-only 的rootfs之上分配一层空的read-write的rootfs，我们对文件系统的改变实际上是在空的这层rootfs（read-write）上发生的。打个比方，如果你想修改一个文件，系统实际上是将这个在read-only层的rootfs的文件拷贝到read-write层的rootfs之中，然后对它进行修改，但read-only层的文件并不会被修改，依然存在于read-only层之中，只不过是在read-write层下被隐藏了。这种模式被称为copy on write。这是unionFS的特性，也是docker的强大之处。它允许镜像被继承，也就是说我们想生成一套虚拟环境不用从零开始了，而只要在一个相对完善的基础环境之上来创建我们的虚拟环境就可以了，比如我们想生成一个具有tomcat环境的镜像，只要在一个装有java环境的镜像之上来创建就可以了。这也是docker便捷性的体现。

#### b、容器

**什么是容器：**

​	docker实际上使用了命名空间（namespace）来对进程进行隔离，使不同namespace的进程彼此不可见，同时使用cgroup来对彼此隔离的进程的资源进行限制，docker的容器（container）其实就是一个进程的容器，而并不是一个全虚拟化的操作系统，所以他不会有什么init进程。docker将进程、进程所需要的操作系统、运行环境称为容器。它实际上是一个内核级别的虚拟化技术，容器还是在使用宿主机的内核。所以它比传统的基于hypervisor的虚拟机拥有更高的效率，并使用更低的资源。

#### c、docker与虚拟机的区别

首先看下图

![](http://ot0aou666.bkt.clouddn.com/docker%E4%B8%8E%E8%99%9A%E6%8B%9F%E6%9C%BA%E5%8C%BA%E5%88%AB.png)

​	以上是虚拟机的层次结构。其中，Guest OS为从操作系统，即虚拟机的系统，Host OS为主操作系统，即宿主机的操作系统，Hyervisor为虚拟机管理系统，可以使主操作系统上运行多个从操作系统。

![](http://ot0aou666.bkt.clouddn.com/docker%E6%9C%BA%E6%9E%84%E5%9B%BE.png)

​	以上为docker的层次结构。与虚拟机结构不同的是，每个应用都没有了Guest OS，并且Hyervisor被替换为Docker Engine，它是运行在操作系统之上的后台进程，负责管理Docker镜像、容器，直接与主操作系统进行通信，为Docker容器分配资源并将容器与主操作系统、容器与容器之间隔离。docker目前使用的是Linux内核本身支持的容器方式实现资源和环境隔离，也即利用namespace实现系统环境隔离，利用Cgroup实现资源限制，利用镜像实现根目录环境的隔离（上述有提到这三种技术）。

​	通过一些分析，可以大致得出一些结论：

​	（1）docker有着比虚拟机更少的抽象层。docker不需要Hypervisor实现硬件资源虚拟化，程序运行在docker容器上都是直接使用实际物理机的硬件资源，因此在CPU和内存利用率上docker都会有效率优势

​	（2）docker利用的是宿主机的内核，不需要Guest OS。引导、加载操作系统内核是一个费时费资源的过程，开启虚拟机时需要加载Guest OS，新建过程是分钟级别的；而docker直接利用了宿主机的操作系统，省略了这个过程，因此一个docker容器的启动时间只需要几秒钟。此外，在一台物理机上新增一个操作系统对内存、磁盘空间等资源的开销也非常大，因此docker在资源消耗上也占有较大的优势。

## 二、使用docker镜像

### 1、基本命令

`docker images`：查看主机上的镜像

- REPOSITORY：表示镜像的仓库源
- TAG：镜像的标签
- IMAGE ID：镜像ID
- CREATED：镜像创建时间
- SIZE：镜像大小

`docker pull 镜像`：当我们在本地主机上使用一个不存在的镜像时 Docker 就会自动下载这个镜像。

`docker search 镜像`：搜索镜像

`docker rmi imageId`：删除镜像,要删除全部image的话`docker rmi $(docker images -q)`

`docker tag imageId imageName:tag`：设置镜像标签

### 2、创建（更新）镜像的两种方法

#### （1）提交镜像

**从已经创建的容器中创建镜像，并且提交这个镜像。**首先创建一个容器：`docker run -t -i centos:7.2.1511 /bin/bash`（或者进入一个已创建好的容器`docker attach 容器id`），**对容器内部修改操作完之后**退出容器`exit`。然后通过命令 `docker commit -m="" -a="" e218edb10161 java:1.0`来提交该容器，形成一个镜像

ps：其中，-m表示提交的描述信息，-a表示镜像作者，e218edb10161 表示容器Id（上步创建的容器），java：1.0表示要创建目标的镜像名

#### （2）使用Dockerfile构建镜像

在构建新的镜像时如果采用docker build的方式是需要编写Dockerfile文件的，该文件定义了容器在创建时的行为（安装软件、执行命令、拷贝文件等）。

Dockerfile编写命令：

1）`FROM`：设置基础镜像的名称，可以采用“minhviet/centos-6.5”这种全称形式，minhviet为作者名称，centos-6.6为镜像名称，也可以不出现全称，例如`FROM centos`

2）`MAINTAINER`：设置创建该镜像的作者信息。`MAINTAINER garson "876833508@qq.com"`

3）`CMD`：用于指定一个**容器**启动时需要运行的命令，可通过exec方式书写。`CMD["yum" ,"list"]`

注：1.在Dockerfile中只可以指定一个CMD命令。如果指定多个，也只有最后一个生效。
​	2.如果在容器启动命令run中指定了启动时的命令，则run中的指令会覆盖掉CMD的操作。

4）`RUN`：构建镜像时所需要执行的命令，书写方式有两种：

1.shell命令行形式：`RUN yum list`；2.exec形式：`RUN["yum", "install", "httpd", "-y"]`

注：RUN的构建时在基础镜像之上按照**由上到下**的顺序，以层级的方式进行构建，这有些类似于git的版本控制，一旦某个RUN命令发生错误，docker将会停止构建，并且得到构建失败前最后一次正确构建的镜像ID，可以进入该镜像排查问题。

5）`ADD`：将构建环境下的文件和目录复制到镜像中，他和COPY的功能非常类似，额外的功能是：如果ADD的内容是一个压缩文件（tar、zip等文件），ADD会自动进行解压。`ADD tar.nginx.gz /usr/local/nginx`

注： 1.ADD的文件路径必须是build的环境内的； 
​	2.如果ADD的是一个目录，则目录下的数据全部会复制到指定的目录中（目录本身不复制）； 
​	3.如果是压缩包会被解压缩。

6）`COPY`：COPY的左右和ADD功能十分相近，它也有两种不同的方式： `COPY src dest （shell形式）`; 
`COPY[“src”, “dest”]`

注： 1.COPY的文件路径必须是build的环境内的； 
​	2.当src有多个文件时，dest必须是一个目录，而且必须以/结尾。

7）`WORKDIR`：在容器内部设置一个工作目录，ENTRYPOINT和/或CMD指定的程序会在这个目录下执行。

```ini
WORKDIR /opt/webapp/db
RUN bundle install
WORKDIR /ope/webapp
ENTRYPOINT ["rackup"]
```

这里设定了两个WORKDIR目录，在第一个目录执行了RUN命令，切换到另外一个WORKDIR，然后又执行了一个ENTRYPOINT命令。**WORKDIR指定的目录不存在就会被主动创建出来**。

8）`USER`：USER指定了该镜像启动的容器会以什么样的用户去运行。`USER root`

9）`LABEL`

LABEL用于为DOCKER镜像添加元数据，元数据以键值对额形式展示。

```
LABEL version=“1.0”
LABEL location=“New work” type=“Data Center”12
```

10）`STOPSIGNAL`

STOPSIGNAL指令用来设置停止容器是发送什么系统调用给容器。这个信号必须是内核系统调用表的合法数。

11）`ARG`

ARG指令用来定义可以在docker build命令运行时传递给构建运行时的变量，我们只需要在构建时使用–build-arg即可。

```
ARG build
ARG webapp_user=user12
```

12）`ENV`

ENV指令用来在镜像构建过程中设置环境变量。`ENV <key> <value>`

```
ENV RVM_PATH /home/rvm/1
```

13）`EXPOSE`

此命令通知Docker容器监听指定的网络端口 ， EXPOSE 指令不会让容器的端口访问host主机，如果想要这样做就需要在运行容器的时候指定 -p 发布一个端口范围或者 -P 发布所有打开的端口。



**实例**：首先需要在宿主机上创建镜像的根目录，然后在根目录下编辑Dockerfile文件

```
#create the zabbix server images
FROM oraclelinux:6.7

#create zabbix user
RUN useradd zabbix

#install packages
RUN yum install gcc* mysql-devel libxml2-devel net-snmp* java* curl-devel -y

#set work directory
WORKDIR /zabbix

#copy zabbix source code 
ADD zabbix-3.2.1.tar.gz /zabbix/


#set configure directory
WORKDIR /zabbix/zabbix-3.2.1

#configure
CMD ./configure --enable-serer --enable-agent --with-mysql --enable-ipv6 --with-netsnmp --with-libcurl --with-libxml2 --enable-java

EXPOSE 10051
EXPOSE 10050
```

构建完Dockerfile之后，在Dockerfile所在目录下执行命令：`docker build -t imageName:tag`

查看`docker images`，便可看到是否有新的镜像生成

#### （3）将镜像推到docker hub上

首先登录hub `docker login`，然后将镜像推上hub  `docker push imageName:tag`



## 三、使用docker容器

### 1、基本命令

`docker ps [-a]`：查看正在运行的容器信息，加上-a表示查看全部容器，包括没有运行的

`docker logs 容器id`：查看容器应用程序日志

`docker inspect`：检查容器底层信息

`docker start/stop/rm 容器名或id`：开启、停止、删除容器,`docker stop $(docker ps -a -q)`停止所有容器，`docker rm $(docker ps -a -q)`删除所有容器（先停止后删除）

`docker run [-d] [-it] [-p/-P] --name garson imageId `：创建运行容器，-d 指定容器运行后与当前tty分离，后台运行并返回容器ID，-P是容器内部端口随机映射到主机的高端口，-p是容器内部端口绑定到指定的主机端口（8080：8080 主机8080端口映射到容器的8080端口）。-i以交互模式运行容器，通常与 -t 同时使用的。-t为容器重新分配一个伪输入终端，通常与 -i 同时使用；--name容器名称。

`docker attach 容器id或名`  或 `docker exec -it 容器id或名 bash`：进入正在后台运行的容器

### 2、实现主机与容器间传输文件

首先用`docker ps`获取容器短id，然后获取id全称`docker inspect -f '{{.Id}}' 容器短id`，然后执行命令

`docker cp 本地文件路径 id全称:容器路径`，如果要从容器传输文件到主机，则反过来

`docker cp id全称:容器路径 本地文件路径`



## 四、搭建jdk+tomcat容器

首先创建Dockerfile根目录，例如test目录。将jdk、tomcat解压包或者解压文件放置到该目录下，然后创建Dockerfile文件并进行编写，如下(下面以/usr/src/jdk为环境变量)：

```
#重点是环境变量要设对，以及startup.sh与catalina.out的路径要写对（关注jdk与tomcat目录ADD到了
#哪个目录下）
#pull down centos image
FROM centos
MAINTAINER test@test.com
#copy jdk and tomcat into image
ADD ./jdk /usr/src/jdk
ADD ./tomcat /usr/src/tomcat
#set environment variable
ENV JAVA_HOME /usr/src/jdk
ENV PATH $JAVA_HOME/bin:$PATH
ENV CATALINA_HOME /usr/src/tomcat
#define entry point which will be run first when the container starts up
ENTRYPOINT /usr/src/tomcat/bin/startup.sh && tail -F /usr/src/tomcat/logs/catalina.out
```

然后构建镜像`docker build -t garson/centos:tomcat-centos --rm=true .` ，-t选择指定生成镜像的用户名，仓库名和tag，--rm=true指定在生成镜像过程中删除中间产生的临时容器。

最后运行镜像`docker run -d -p 8080:8080 imageId`，登录http://宿主机IP:8080便可看到tomcat主页

## 五、搭建mysql容器并连接jdk+tomcat容器

### 1、基本操作

​	（1）首先从dockerhub下载mysql镜像 `docker pull mysql:版本号`

然后创建容器`docker run -it --name mysql-server -e MYSQL_ROOT_PASSWORD=123456 -d mysql`

最后进入容器 `docker attach 容器id` 运行mysql测试。

​	（2）创建jdk+tomcat容器与mysql容器相连接：

`docker run -d -p 8080:8080 -v ~/test:/usr/local/ --name=jdk_tomcat --link=mysql-server:db -i -t java-web:1.0`

其中，可以通过-v将war包传进容器中，--link的形式：`--link name:alias`，name为要连接的容器名，alias为link的别名。

### 2、“容器连接”概述

​	连接的两个容器是父子关系，容器间访问需要打开一个安全连接隧道（不暴露任何端口在容器外部），启动容器时没有使用-p或-P标识，**表示连接容器时我们不需要通过网络给数据库开放端口（上面命令中的-p 8080:8080 是宿主机与jdk_tomcat容器的端口映射）**，而Docker容器间连接信息有两种方法：

​	（1）docker为数据库容器创建了一系列的环境变量，可进入连接的容器中使用命令：`env`查看。如下连接mysql容器的jdk_tomcat容器：

```
[root@89df90662b98 /]# env

HOSTNAME=89df90662b98

DB_NAME=/jdk_tomcat/db

TERM=xterm

CATALINA_HOME=/usr/src/tomcat

DB_PORT=tcp://172.17.0.2:3306

DB_PORT3306TCP_PORT=3306

DB_ENV_GOSU_VERSION=1.7
```

​	（2）docker将信息添加到了连接容器的hosts文件中

还是用以上的jdk_tomcat容器，查看/etc/hosts文件 `cat /etc/hosts`

```
127.0.0.1	localhost
::1	localhost ip6-localhost ip6-loopback
fe00::0	ip6-localnet
ff00::0	ip6-mcastprefix
ff02::1	ip6-allnodes
ff02::2	ip6-allrouters
172.17.0.2	db 0a17e4d12751 mysql-server  
172.17.0.3	89df90662b98
```

可以看到两条hosts的配置，一个是数据库的ip（172.17.0.2）对应的别名，容器ID，容器名称。另外一个是客户端（172.17.0.3）对应的容器ID。所以这里可以使用别名连接到数据库服务端。

**容器连接完后，我们就可以将数据库连接改为**

```java
jdbc.jdbcUrl=jdbc:mysql://db:3306/db.shiro?useSSL=false&useUnicode=true&characterEncoding=UTF-8
```



## 六、应用场景

### 1、简化配置

对于各应用的配置环境，使用docker只需要在容器中配置好创建镜像后将其push到自己的dockerhub上就能在其他地方重复使用，使用时pull下来就行

### 2、多应用隔离部署

当一台服务器上部署多个应用或者同一个应用的多个版本时，多个版本，文件路径、端口等资源往往会发生冲突，因此使用docker来部署多个应用，使它们各自有自己的文件系统，只要在启动容器时注意端口映射问题就行

### 3、应用内部测试

内部开发测试环境一般负载较低，如果使用多台虚拟机充当开发测试环境的话无疑会将大量的系统资源浪费在虚拟机上。因为docker容器没有cpu和内存上的额外开销，所以可以使用docker容器提供测试环境。

