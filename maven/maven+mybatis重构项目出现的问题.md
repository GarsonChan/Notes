### 

2、出现**No default constructor found**：

出现此错误是与构造函数有关，一般是与类默认的空构造函数有冲突或者mybatis中返回类型为po的对象的信息时没有对应信息的构造函数



3、使用存储过程是要注意存储过程内的select ... into ... 有没有重复，例如查询条件是用户名，而有两个用户名是相同的，此时会报错：

​      **results consisted of more than one row**

4、

不管是返回一个结果还是多个结果，MyBatis都是安装多个结果进行查询，`selectOne`是查询一个，`selectList`是查询多个，我们看看`selectOne`代码：

```java
public <T> T selectOne(String statement, Object parameter) {
    List<T> list = this.<T>selectList(statement, parameter);
    if (list.size() == 1) {
        return list.get(0);
    } else if (list.size() > 1) {
        throw new TooManyResultsException("Expected one result (or null) to be returned by selectOne(), but found: " + list.size());
    } else {
        return null;
    }
}
```

注意看：

```java
List<T> list = this.<T>selectList(statement, parameter);
```

实际上，不管查询一个还是多个结果，MyBatis都是先按多个结果进行查询。拿到`list`结果后再判断。如果是查询一个结果的情况，那么`list`最多只能有一个返回值。而List<T>中的泛型T，就是resultType内的类型，因此，**要注意如果返回类型是集合情形，那应该是集合可以包含的类型，而不能是集合本身。**



5、打开tomcat时如果报错：unsupported-major-minor-version-52-0，则原因是jar编译时是在jdk1.8环境中，而运行时是在jdk1.7中

- J2SE 9 = 53
- J2SE 8 = 52
- J2SE 7 = 51
- J2SE 6.0 = 50
- J2SE 5.0 = 49
- JDK 1.4 = 48
- JDK 1.3 = 47
- JDK 1.2 = 46
- JDK 1.1 = 45




6、一般resultType都为javabean，使用selectList方法，用List<javabean>接收，此时接收回来的是javabean对象，可以用

`list.get(index).getXXX()`获取对象的属性。当select的结果是javabean内的属性时，javabean类内要有该属性的构造方法（javabean类内默认有一个空的构造方法）。例如：要

```sql
select name , time from image
```

name与time是image的属性，那么在image类中要有name与time的构造方法

```java
public Image(String imageName ,String time){
  this.imageName = imageName;
  this.time = time;
}
```



7、**dependencyManagement只会影响现有依赖的配置，但不会引入依赖**，所以每个模块所使用到的依赖都要归到parent中，方便管理（例如更改依赖的版本时）



8、创建webapp时不要忘记部署web.xml



9、当一个模块已经依赖到别的模块时，如果对这个模块有所改变，需要对这个模块重新install一下



10、模块间相互依赖（摘自http://hck.iteye.com/blog/1728329）：

**（1）、什么是循环依赖** 
　　 
　　如果工程A依赖工程B，工程B又依赖工程A，就会形成循环依赖。或者A依赖B，B依赖C，C依赖A，也是循环依赖 
　　 
　　总的来说，在画出工程依赖图之后，如果发现工程间的依赖连线形成了一个有向循环图，则说明有循环依赖的现象 
　　 
　　如果循环依赖发生在工程之间，则会影响构建，因为maven不知道应该先编译哪个工程。如果循环依赖发生在同一个工程的模块之间，虽然不影响编译，但是也是一种不好的实践，说明模块的设计有问题，应该避免 
　　 
　　如果在模块内部，有几个类互相调用的话，我觉得可能是正常的。比如观察者模式里面，Observer和Observable就是互相依赖的 
　　 
　　**（2）、怎么解决循环依赖** 
　　 
　　目前知道有2个办法可以解决 
　　 
　　第一个办法是用build-helper-maven-plugin插件来规避。比如A依赖B，B依赖C，C依赖A的情况。这个插件提供了一种规避措施，即临时地将工程A、B、C合并成一个中间工程，编译出临时的模块D。然后A、B、C再分别依赖临时模块D进行编译 ，D模块的POM模型如下：



```xml
<project xmlns="http://maven.apache.org/POM/4.0.0"  
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"  
    xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">  
    <parent>  
        <groupId>org.kuuyee.sample</groupId>  
        <artifactId>sample-parent</artifactId>  
        <version>1.0-SNAPSHOT</version>  
        <relativePath>../../pom.xml</relativePath>  
    </parent>  
    <modelVersion>4.0.0</modelVersion>  
    <groupId>org.kuuyee.sample</groupId>  
    <artifactId>module-D</artifactId>  
    <version>1.0-SNAPSHOT</version>  
    <packaging>jar</packaging>  
    <name>module-D</name>  
    <url>http://maven.apache.org</url>  
    <properties>  
        <project.build.sourceEncoding>  
            UTF-8  
        </project.build.sourceEncoding>  
        <module.a.src>../../module/module-A/src/main/java</module.a.src>  
        <module.b.src>../../module/module-B/src/main/java</module.b.src>  
        <module.c.src>../../module/module-C/src/main/java</module.c.src>  
    </properties>  
    <build>  
        <plugins><!-- 解决模块相互依赖,综合所有相互依赖代码统一编译 -->  
            <plugin>  
                <groupId>org.codehaus.mojo</groupId>  
                <artifactId>build-helper-maven-plugin</artifactId>  
                <executions>  
                    <execution>  
                        <id>add-source</id>  
                        <phase>generate-sources</phase>  
                        <goals>  
                            <goal>add-source</goal>  
                        </goals>  
                        <configuration>  
                            <sources>  
                                <source>${module.a.src}</source>  
                                <source>${module.b.src}</source>  
                                <source>${module.c.src}</source>  
                            </sources>  
                        </configuration>  
                    </execution>  
                </executions>  
            </plugin>  
        </plugins>  
    </build>  
    <dependencies>  
        <dependency>  
            <groupId>junit</groupId>  
            <artifactId>junit</artifactId>  
            <version>3.8.1</version>  
            <scope>test</scope>  
        </dependency>  
    </dependencies>  
</project>  
```

**注：由于下载后eclipse报错说下载失败，因此没有用这种方法（以后用idea时再试）**

　　这种方法可以解决无法构建的问题，但是只是一个规避措施，工程的依赖关系依然是混乱的 
　　 
　　第二个办法是通过重构，从根本上消除循环依赖 
　　 

  **（3）、如何重构**
　　 
　　目前也知道2个重构的思路 
　　 
　　第一个办法是平移，比如A和B互相依赖，那么可以将B依赖A的那部分代码，移动到工程B中，这样一来，B就不需要继续依赖A，只要A依赖B就可以了，从而消除循环依赖 
　　 
　　第二个办法是下移，比如A和B互相依赖，同时它们都依赖C，那么可以将B和A相互依赖的那部分代码，移动到工程C里，这样一来，A和B相互之间都不依赖，只继续依赖C，也可以消除循环依赖 
　　 
　　这两种重构方式都是可行的，具体采用哪种方式要根据实际情况来判断。不管采取哪种方式，都需要对代码进行修改，有时候并不是那么容易的 



11、千万记得要在dao层配置conf.xml文件



12、出现**Cannot change version of project facet Dynamic web module to 3.0**，解决方法：

windows -> showView -> Navigator -> 打开web项目 -> .settings -> org.eclipse.wst.common.project.facet.core.xml ->将jst.web的version改为3.0



13、光标变为黑色小块解决方法：

将数字区的键锁住后按insert就好