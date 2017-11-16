# Spring基础

## 第一讲、spring入门

### 1、创建核心文件

```xml
<?xml version="1.0" encoding="UTF-8"?>    
<beans xmlns="http://www.springframework.org/schema/beans"    
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"   
    xmlns:p="http://www.springframework.org/schema/p"  
    xmlns:aop="http://www.springframework.org/schema/aop"   
    xmlns:context="http://www.springframework.org/schema/context"  
    xmlns:jee="http://www.springframework.org/schema/jee"  
    xmlns:tx="http://www.springframework.org/schema/tx"  
    xsi:schemaLocation="    
        http://www.springframework.org/schema/aop http://www.springframework.org/schema/aop/spring-aop-4.0.xsd  
        http://www.springframework.org/schema/beans http://www.springframework.org/schema/beans/spring-beans-4.0.xsd  
        http://www.springframework.org/schema/context http://www.springframework.org/schema/context/spring-context-4.0.xsd  
        http://www.springframework.org/schema/jee http://www.springframework.org/schema/jee/spring-jee-4.0.xsd  
        http://www.springframework.org/schema/tx http://www.springframework.org/schema/tx/spring-tx-4.0.xsd">
</beans>
```

**（springframework4.2.0）**

一般讲文件放在src中



### 2、入门demo

（applicationContext.xml中）

```xml
 <!-- bean元素作用：当spring框架加载时，spring就会自动创建一个bean，并存入内存中 -->
 <!-- UserService userService = new UserService();//bean的id对应userService这个对象名称
 	  userService.setName("陈嘉盛");
 -->
	<bean id="userService" class="com.service.UserService">
    <property name="name">
    	<value>陈嘉盛</value>
    </property> 
	</bean>
```

（java代码demo）

```java
ApplicationContext ac = new ClassPathXmlApplicationContext("applicationContext.xml");
UserService uc = (UserService) ac.getBean("userService");//getbean内的值应该与bean元素的id属性相同
uc.sayHello();
```

(其中ref属性维护UserService和ByeService之间的关系 ；name属性指bean中的属性名称；value是对该属性设值；ref属性对应bean的id值)

`<property name="bs" ref="byeService" />`中的name指set方法中的参数，而不是对象的属性

```xml
<bean id="userService" class="com.service.UserService">
  <property name="name">
    <value>陈嘉盛</value>
  </property>
  <!-- 在userservice中引用 byeService bean  -->
  <property name="bs" ref="byeService" />
</bean>
<bean id="byeService" class="com.service.ByeService">
  <property name="name" value="小明" />
</bean>
```



## 第二讲、spring原理



spring是一种容器框架，用于配置bean、维护bean之间的关系

**bean ： java中的任何一种对象，包括javabean/service/action/数据源/dao/ioc（控制反转，inverse of control）或 di(dependency injection，依赖注入)**





### 过程

当ClassPathXmlApplicationContext执行时，spring容器对象就被创建（applicationContext对象），同时applicationContext.xml通过java的反射机制将其中配置的bean创建出来（内存）

```java
ApplicationContext ac = new ClassPathXmlApplicationContext("applicationContext.xml");
UserService uc = (UserService) ac.getBean("userService");
```



```xml
<bean id="userService" class="com.service.UserService">
  <property name="name">
    <value>陈嘉盛</value>
  </property>
  <!-- 在userservice中引用 byeService bean  -->
  <property name="bs" ref="byeService" />
</bean>
<bean id="byeService" class="com.service.ByeService">
  <property name="name" value="小明" />
</bean>
```

通过java反射机制来创建对象：

```java
userService = Class.forName("com.service.UserService");
userService.setName(...)

byeService = Class.forName("com.service.ByeService")
byeService.setName(...)
userService.setByeService(byeService)
applicationContext = new HashMap();
applicationContext.put("userService" ,userService)
applicationContext.put("bs" ,byeService) 
  
```

内存中：

```
内存(applicationContext对象引用)[结构类似HashMap]
id                        	对象
userService(ox123)          UserService[name ,byeService(ox234)]
bs(ox234)	                ByeService[name]
```



ioc（inverse of control) 控制反转：把创建对象（bean）和维护对象（bean）的关系的权力从程序中转移到spring的容器文件（applicationContext.xml），而程序本身不再维护



di（dependency injection）依赖注入：实际上di和ioc是同一个概念，spring设计者认为di更能表示spring的核心。



model层（业务层 + dao层 + 持久层）

持久层：主要解决关系模型和对象模型之间的阻抗



## 第三讲、di配合接口编程

di配合接口编程，减少层（web层）和业务层的耦合度



例子：

beans.xml中有如下：

```xml
<bean id="ChangeLetter" class="com.inter.LowwerLetter" >
  <property name="str" value="ABCDE" />
</bean>

<bean id="ChangeLetter" class="com.inter.UpperLetter">
  <property name="str">
    <value>abcde</value>
  </property>
</bean>

<!-- 其中id为接口的名称，class为实现接口方法的全类名 -->
```

有一个ChangeLetter接口，里面有一个方法Change

```java
package com.inter;

public interface ChangeLetter {
	//声明一个方法
	public void change();
}
```

对于这个接口有两个实现类：

UpperLetter.class

```java
@Override
public void change() {
  // TODO Auto-generated method stub
  System.out.println(str.toUpperCase());
}
```

LowwerLetter.class

```java
@Override
public void change() {
  // TODO Auto-generated method stub
  System.out.println(str.toLowerCase());
}
```

在调用方法时有以下：

```java
@SuppressWarnings("resource")
ApplicationContext ac = new ClassPathXmlApplicationContext("com/inter/beans.xml");
//获取，不用接口
/*UpperLetter changeLetter = (UpperLetter) ac.getBean("ChangeLetter");
		changeLetter.change();*/

//使用接口访问bean
ChangeLetter changeLetter = (ChangeLetter) ac.getBean("ChangeLetter");
changeLetter.change();
```

beans.xml中两个类的id都为ChangeLetter，当我们想要改变方法的实现时，只需要改变beans.xml里面的内容，而不需改动到调用方法的类中的内容，这样体现了di，即将权力转移到了spring容器文件上，有效地减少层与层之间的耦合





## 第四讲、bean工厂和applicationContext

### 1、区别

applicationContext应用上下文容器中获取bean和从bean工厂容器中获取bean的区别

```java
//从ApplicationContext中获取bean，预先加载，运行速度快，但耗内存
ApplicationContext ac = new ClassPathXmlApplicationContext("com/ioc/beans.xml");//当实例化beans.xml时，该文件中配置的bean被实例（该bean scope是singleton）

//从bean工厂中取出student
@SuppressWarnings("deprecation")
BeanFactory factory = new XmlBeanFactory(new ClassPathResource("com/ioc/beans.xml"));//节省内存，但一般不用
factory.getBean("student");//延时创建对象，如果我们使用beanfactory去获取bean，当只是实例化该容器时，那么容器的bean不会被实例化，只有当get某个bean的时候才会实时的创建
```

**没特殊要求则使用第一种**



### 2、bean的scope

**prototype不会预先加载，因为不清楚要加载几个对象，而singleton会预先加载，因为只加载一个。默认的scope是singleton**

```
singleton： 在每个Spring IoC容器中一个bean定义对应一个对象实例。

prototype： 一个bean定义对应多个对象实例。

request：  在一次HTTP请求中，一个bean定义对应一个实例；即每次HTTP请求将会有各自的bean实例， 它们依据某个bean定义创建而成。该		
		   作用域仅在基于web的Spring ApplicationContext情形下有效。

session：  在一个HTTP Session中，一个bean定义对应一个实例。该作用域仅在基于web的Spring ApplicationContext情形下有效。

global session：在一个全局的HTTP Session中，一个bean定义对应一个实例。典型情况下，仅在使用portlet context的时候有效。该作用域

                 仅在基于web的Spring ApplicationContext情形下有效。
```



## 第五讲、bean的生命周期

**aop：面向切面编程，针对所有对象编程**



### 1、ApplicationContext获取的bean

1、  实例化**（此处与bean的构造方法有关系）**

当程序加载beans.xml文件时发生，把bean（前提是scope = singleton）实例化到内存中

2、	调用set方法设置属性

3、  如果实现了bean名字关注接口（BeanNameAware），则可以通过setBeanName获取id号

4、  如果实现了bean工厂关注接口（BeanFactoryAware），则可以获取bean工厂

5、  如果实现了bean工厂关注接口（ApplicationContextAware），则可以获取bean工厂的上下文信息

6、  如果bean与一个后置处理器关联，则会自动去调用postProcessBeforeInitialization、postProcessAfterInitialization方法

实现后置处理器（类似过滤器）

**首先创建类**（实现接口BeanPostProcessor）：

```java
public class MyBeanPostProcesser implements BeanPostProcessor {

  @Override
  public Object postProcessAfterInitialization(Object arg0, String arg1) throws BeansException {
    // TODO Auto-generated method stub
    System.out.println("postProcessAfterInitialization 函数被调用");		
    return arg0;
  }

  @Override
  public Object postProcessBeforeInitialization(Object arg0, String arg1) throws BeansException {
    // TODO Auto-generated method stub
    System.out.println("postProcessBeforeInitialization 函数被调用");
    return arg0;
  }

}
```

**然后在beans.xml中配置**：

```xml
<bean id="MyBeanPostProcesser" class="com.beanLife.MyBeanPostProcesser"/>
```

可实现某些需求，比如获取实例化对象的时间、过滤每个调用对象的ip、给所有对象添加属性等

7、后置处理器中间还有两个过程（在处理器before方法之后，after方法之前）（一般用得少，暂时没有demo）：

（1）InitializingBean

（2）<bean init-method="init"/>则可以在bean定义自己初始化方法

8、bean使用

9、容器关闭：通过实现DisposableBean接口来调用方法destory或者在<bean destory-method="方法名"/>调用定制的销毁方法



**常见的过程为：**1->2->6->8->9



### 2、bean工厂获取的bean

当bean是在工厂创建时，生命周期会简单一些，少了后置处理器这个过程



## 第六讲、装配Bean

告诉容器哪些bean以及容器如何使用依赖注入将它们配合在一起



### 1、配置bean的细节

#### （1）内部bean

```xml
<bean id="department" class="com.collection.Department">
  <property name="employeeName" >
    <!-- 第一方法引用 -->
    <ref bean="bean对象名"/>
    <!-- 第二方法 内部bean -->
    <bean>
    	<property>
      			...
      	</property>
    </bean>
  </property>
```



#### （2）为集合类型注值

department类：

```java
private String name;
private String[] employeeName;
private List<Employee> empList;
private Set<Employee> empSet;
private Map<String, Employee> empMap;
private Properties pp;
```

employee类：

```java
private String name;
```

注入值：

```xml
<bean id="department" class="com.collection.Department">
  <property name="name" value="财务部" />
  <!-- 给数组注值 -->
  <property name="employeeName" >
    <list>
      <value>小明</value>
      <value>小小明</value>
      <value>大明</value>
    </list>
  </property>
  
<!-- 为department类中的List<Employee> empList注值 -->
  <property name="empList">
    <list>
      <ref bean="emp1"/>
      <ref bean="emp2"/>
    </list>
  </property>
  
<!-- 为department类中的Set empSet注值 -->
  <property name="empSet">
    <set>
      <ref bean="emp1"/>
      <ref bean="emp2"/>
    </set>
  </property>
  
  <!-- 为department类中的Map<String, Employee> empMap注值 -->
  <!-- key：八大类型，直接写值
	   key-ref：指向某个对象
  -->
   <property name="empMap">
    <map>
      <entry key="1" value-ref="emp1"/>
      <entry key="2" value-ref="emp2"/>
    </map>
  </property>

  <!-- 给属性集合配置 -->
  <property name="pp">
    <props>
      <prop key="pp1">abcd</prop>
      <prop key="pp2">hello</prop>
    </props>
  </property>
 
</bean> 

<!-- 给Employee的name属性注值 -->
<bean id="emp1" class="com.collection.Employee">
  <property name="name" value="北京"/>
</bean>

<bean id="emp2" class="com.collection.Employee">
  <property name="name" value="天京"/>
</bean>
```



另：循环输出map的方法

```java
//方法1
for(Entry<String ,Employee> entry : dep.getEmpMap().entrySet()){
  System.out.println(entry.getKey() + " " + entry.getValue().getName());
}

//方法2 迭代器
Map<String, Employee> map = dep.getEmpMap();
Iterator it = (Iterator) dep.getEmpMap().keySet().iterator();
while(it.hasNext()){
  String key = (String) it.next();
  Employee emp = map.get(key);
  System.out.println(emp.getName());
}
```

输出properties的方法

```java
//方法1
Properties pp = dep.getPp();
System.out.println(pp.get("pp1").toString());

//方法2 迭代器
for(Entry<Object, Object> entry : pp.entrySet()){
  System.out.println(entry.getKey().toString() + " " + entry.getValue().toString());
}

//方法3 通过枚举取出
Enumeration en = pp.keys();
while(en.hasMoreElements()){
  String key = (String) en.nextElement();
  System.out.println(key + " " + pp.getProperty(key));
}
```



#### （3）继承配置

学生类：

```java
protected String name;
protected int age;
```

毕业生类：

```java
public class Graduate extends Student {

	private String degree;

	public String getDegree() {
		return degree;
	}

	public void setDegree(String degree) {
		this.degree = degree;
	}
}
```

配置：

```xml
<!-- 配置一个学生对象 -->
<bean id="student" class="com.inherit.Student">
  <property name="name" value="chenjiasheng" />
  <property name="age" value="20" />
</bean>	

<!-- 配置一个毕业生对象 -->
<bean id="graduate" parent="student" class="com.inherit.Graduate">
  <property name="degree" value="学士" />
</bean>
```

#### （4）给属性注入null

```xml
<property name="name">
	<null/>
</property>
```



### 2、通过构造函数注入值

可用set方式给bean注入值，还可以通过构造函数注入值！



Employee类

```java
private String name;
private int age;

public Employee() {
  super();
}

public Employee(String name, int age) {
  super();
  this.name = name;
  this.age = age;
}
```

beans.xml中

```xml
<!-- 配置一个雇员 -->
<bean id="employee" class="com.constructor.Employee">

  <!-- 通过构造函数来注入属性值 -->
  <constructor-arg index="0" type="java.lang.String" value="大明" />
  <constructor-arg index="1" type="int" value="20" />

</bean>
```



set注入的缺点是无法清晰表达哪些属性必须的，哪些是可选的，构造函数注入的优势是通过构造强制依赖关系，不可能实例化不完全的或无法使用的bean



### 3、自动装配bean的属性值



**autowire定义了找寻bean信息的方法，若有set方式注入，优先以set方式**

| no          |                                          |
| ----------- | ---------------------------------------- |
| byName      | 根据属性名自动装配。此选项将检查容器并根据名字查找与属性完全一致的bean，并将其与属性自动装配。例如，在bean定义中将autowire设置为by name，而该bean包含*master*属性（同时提供*setMaster(..)*方法），Spring就会查找名为`master`的bean定义，并用它来装配给master属性。 |
| byType      | 如果容器中存在一个与指定属性类型相同的bean，那么将与该属性自动装配。如果存在多个该类型的bean，那么将会抛出异常，并指出不能使用*byType*方式进行自动装配。若没有找到相匹配的bean，则什么事都不发生，属性也不会被设置。如果你不希望这样，那么可以通过设置`dependency-check="objects"`让Spring抛出异常。 |
| constructor | 与*byType*的方式类似，不同之处在于它应用于构造器参数。如果在容器中没有找到与构造器参数类型一致的bean，那么将会抛出异常。 |
| autodetect  | 通过bean类的自省机制（introspection）来决定是使用*constructor*还是*byType*方式进行自动装配。如果发现默认的构造器，那么将使用*byType*方式。 |



Master类

```java
private String name;
private Dog dog;
```

Dog类

```java
private String name;
private int age;
```

**byName的配置**：通过id找到dog对象，将两个对象关联起来

```xml
<!-- 配置一个master对象 -->
<bean id="master" class="com.autowire.Master" autowire="byName">
  <property name="name">
    <value>chenjiasheng</value>
  </property>
</bean>

<!-- 配置一个dog对象 -->
<bean id="dog" class="com.autowire.Dog">
  <property name="name" value="小黄" />
  <property name="age" value="3" />   		
</bean> 
```

**byType的配置**：通过类的类型找到dog对象，多个dog类会抛异常

**constructor的配置**：通过构造函数找到dog对象，需要有与bean相同类型的参数的构造函数

master类要有构造函数：

```java
public Master(Dog dog){
  this.dog = dog;
}
```

**autodetect的配置**：有构造函数则先用constructor，没有就用byType

**default的配置**：default是指xsi:schemaLocation中的default-autowire中的值，默认为no；**在此处指定装配方法后所有的bean的装配方法都为该方法**

```xml
xsi:schemaLocation="    
http://www.springframework.org/schema/aop http://www.springframework.org/schema/aop/spring-aop-4.0.xsd  
http://www.springframework.org/schema/beans http://www.springframework.org/schema/beans/spring-beans-4.0.xsd  
http://www.springframework.org/schema/context http://www.springframework.org/schema/context/spring-context-4.0.xsd  
http://www.springframework.org/schema/jee http://www.springframework.org/schema/jee/spring-jee-4.0.xsd  
http://www.springframework.org/schema/tx http://www.springframework.org/schema/tx/spring-tx-4.0.xsd"
default-autowire="default"
>   
```

**no的配置**：不自动装配



### 4、spring提供的特殊bean



BeanNameAware ,BeanFactoryAware ,ApplicationContextAware、实现了BeanPostProcessor接口的后置处理器都是特殊bean



#### 分散配置

db.properties

```properties
name=root
drivername=com.mysql.jdbc.Driver
url=jdbc:mysql://localhost:3306/web
pwd=123456
```

在beans.xml上配置（使用占位符${}代替bean装配文件中的硬编码配置）

```xml
<context:property-placeholder location="classpath:com/dispatch/db.properties"/>
<bean id="dbUtil" class="com.dispatch.DBUtil">
  <property name="name" value="${name}" />
  <property name="drivername" value="${drivername}" />
  <property name="url" value="${url}" />
  <property name="pwd" value="${pwd}" />
</bean>
```

注：当通过context:property-placeholder引入属性文件的时候，有多个文件时需要使用逗号间隔，例如

```xml
<context:property-placeholder location="classpath:com/dispatch/db.properties ,db2.properties"/>
```

## 第七讲、aop

### 1、原理

是什么：面向切面（方面）编程（aspect oriented programming），是对所有对象或者一类对象编程，核心是（不  还），也即“在不增加代码的基础上还增加相应功能”；将邻域业务与公共业务结合起来，实现公共业务的重复利用，本质还是动态代理。

### 2、名词解释

*关注点*：增加的某个业务。如日志、缓存、事务等。

*切面（Aspect）*：一个关注点的模块化，这个关注点可能会横切多个对象（可以理解为将增加的业务封装成一个类）。

*连接点（Joinpoint）*：在程序执行过程中某个特定的点，比如某方法调用的时候或者处理异常的时候。

*通知（Advice）或增强*：在切面的某个特定的连接点上执行的动作。

*目标对象（Target Object）*： 被一个或者多个切面所通知的对象。也被称做*被通知（advised）*对象。 

*织入（Weaving）*：把切面连接到其它的应用程序类型或者对象上，并创建一个被通知的对象的过程。

### 3、五类通知

#### 1、通过bean配置通知

（1）**首先要定义接口、被代理对象以及通知（功能）**

接口TestServiceInter：

```java
public interface TestServiceInter {
	public void sayHello();
}
```

被代理对象类Test1Service：

```java
public class Test1Service implements TestServiceInter{

	private String name;
	
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	@Override
	public void sayHello() {
		// TODO Auto-generated method stub
		System.out.println("hi" + " " + name);
	}
}
```

前置通知MyMethodBeforeAdvice（MethodBeforeAdvice为spring定义的bean）：

```java
public class MyMethodBeforeAdvice implements MethodBeforeAdvice {

	/**
	 * method：被调用方法名
	 * args：给method传递的参数
	 * target：目标对象
	 */
	@Override
	public void before(Method method, Object[] args, Object target) throws Throwable {
		// TODO Auto-generated method stub
		System.out.println("记录日志" + " " + method.getName());
	}

}
```

（2）**其次在beans.xml中配置被代理对象、前置通知以及代理对象**

```xml
<!-- 配置被代理对象 -->
   <bean id="test1Service" class="com.aop.Test1Service">
   	<property name="name" value="chenjiasheng" />
   </bean>
   
   <!-- 配置前置通知 -->
   <bean id="MyMethodBeforeAdvice" class="com.aop.MyMethodBeforeAdvice" />

   <!-- 配置环绕通知 -->
   <bean id="mymethodinterceptor" class="com.aop.MyMethodInterceptor" />
   
   <!-- 配置后置通知 -->
   <bean id="myafterreturningadvice" class="com.aop.MyAfterReturningAdvice" />
   
   <!-- 配置异常通知 -->
   <bean id="mythrowsadvice" class="com.aop.MyThrowsAdvice" />
   
   <!-- 配置代理对象 -->
   <bean id="proxyFactoryBean" class="org.springframework.aop.framework.ProxyFactoryBean">
     
```

在配置代理对象时要声明需要代理的接口集、将通知织入代理对象以及声明被代理对象

```xml
<!-- 配置代理对象 -->
<bean id="proxyFactoryBean" class="org.springframework.aop.framework.ProxyFactoryBean">

  <!-- 声明需要代理的接口集，数组类的注值 -->
  <property name="proxyInterfaces">
    <list>
      <value>
        com.aop.TestServiceInter
      </value>
    </list>
  </property>
  
  <!-- 把通知织入到代理对象 -->
  <property name="interceptorNames">
    <!-- 相当于把MyMethodBeforeAdvice前置通知和代理对象关联起来，我们也可以把通知看成拦截器 -->
    <!-- 若有多个value，应该写成集合注入的形式-->
    <value>MyMethodBeforeAdvice</value><!-- 此处value值应该与上面配置通知的id相同-->
  </property>

  <!-- 声明被代理对象 -->
  <property name="target" ref="test1Service" />

</bean>
```

（3）**最后在实际类中调用：**

```java
public static void main(String[] args) {
  ApplicationContext ac = new ClassPathXmlApplicationContext("com/aop/beans.xml");
  TestServiceInter ts = (TestServiceInter) ac.getBean("proxyFactoryBean");
  ts.sayHello();
  //((TestServiceInter2)ts).sayBye();
}
```



#### 2、通过`<aop:config>`配置

首先需要引入依赖

```xml
<dependency> 
		<groupId> org.aspectj</groupId > 
		<artifactId> aspectjweaver</artifactId > 
		<version> 1.8.7</version > 
</dependency>
```

在xml中配置：

```xml
<bean id="testService" class="com.TestServiceImpl" />
<bean id="log" class="aop.Log" /><!-- 实现了前置接口的类 -->
<bean id="afterLog" class="aop.AfterLog" /><!-- 实现了后置接口的类 -->

<aop:config>
  <!-- 切入点 -->
  <aop:pointcut expression="execution(* com.TestServiceImpl.*(..))" id="pointcut"/><!-- 告诉spring在哪些业务上实现增强 -->
  <aop:advisor advice-ref="log" pointcut-ref="pointcut"/><!-- 告诉spring要用哪些公共的业务，以及切入点id -->
  <aop:advisor advice-ref="afterLog" pointcut-ref="pointcut" />
</aop:config>
```

其中`expression`中`.*`指包下的所有类或者类下的所有方法，`(...)`表示所引用方法的参数





#### 五类通知实现的接口（按照调用的顺序）

前置通知 MethodBeforeAdvice（使用方法函数前调用）

环绕通知 MethodInterceptor（在方法函数中的首端、尾端调用）

异常通知 ThrowsAdvice（有异常时被调用，后置通知前）

后置通知 AfterReturningAdvice （使用方法函数后调用）

引入通知 NameMatchMethodPointCut（定义切入点）

例：

```xml
<bean id="myMethodBeforeAdviceFilter" class="org.springframework.app.support.NameMatchMethodPointCut">
  <property name="advice" ref="MyMethodBeforeAdvice" />
  <property name="mappedNames">
  		<list>
  			<value>sayHello</value>
  		</list>
  </property>
</bean>
```

(表示只在sayhello中调用前置处理，而saybye中没有)

#### 3、自定义类实现aop

类Log：

```java
public class Log {

	public void before(){
		System.out.println("方法执行前");
	}
	
	public void after(){
		System.out.println("方法执行后");
	}
	
}
```

beans.xml

```xml
<bean id="testService" class="com.TestServiceImpl" />
  	<bean id="log" class="aop2.Log" />
 
  	<aop:config>
  		<aop:aspect ref="log"><!-- 关联Log -->
  			<aop:pointcut expression="execution(* com.TestServiceImpl.*(..))" id="pointcut"/>
  			<aop:before method="before" pointcut-ref="pointcut"/><!-- 前置通知 -->
  			<aop:after method="after" pointcut-ref="pointcut"/><!-- 后置通知 -->
  		</aop:aspect>
  	</aop:config>
```

实现类App1：

```java
public class App1 {
	public static void main(String[] args) {
		@SuppressWarnings("resource")
		ApplicationContext ac = new ClassPathXmlApplicationContext("aop2/beans.xml");
		TestService ts = (TestService) ac.getBean("testService");
		ts.sayHello();
	}
}
```

#### 4、注解定义

beans.xml配置

```xml
<bean id="testService" class="com.TestServiceImpl" />
<bean id="log" class="aop3.Log" />
 
<aop:aspectj-autoproxy />
```

类配置：

```java
@Aspect
public class Log {

	@Before("execution(* com.TestServiceImpl.*(..))")
	public void before(){
		System.out.println("方法执行前");
	}
	
	@After("execution(* com.TestServiceImpl.*(..))")
	public void after(){
		System.out.println("方法执行后");
	}
	
	@Around("execution(* com.TestServiceImpl.*(..))")
	public Object around(ProceedingJoinPoint pj) throws Throwable{
		System.out.println("环绕前");
		System.out.println("签名： " + pj.getSignature());
		//执行目标方法
		Object result = pj.proceed();
		System.out.println("环绕后");
		return result;
	}
}
```

类实现：

```java
public class App1 {

public static void main(String[] args) {
		
		@SuppressWarnings("resource")
		ApplicationContext ac = new ClassPathXmlApplicationContext("aop3/beans.xml");
		TestService ts = (TestService) ac.getBean("testService");
		ts.sayHello();
		
	}
}
```





另：接口可以强转，如

```java
TestServiceInter ts = (TestServiceInter) ac.getBean("proxyFactoryBean");
TestServiceInter2 ts2 = (TestServiceInter2)ts;
```

 

spring的aop中，当通过代理对象去实现aop时，获取的ProxyFactoryBean是代理对象的类型，如果目标对象实现了接口，则spring使用jdk动态代理技术，如果目标对象没有实现接口，则spring使用CGLIB技术



## 第八、注解

### 一、基于注解定义bean

#### 1、使用注解定义bean

`@Component`注解在类声明处对类进行标注，被spring容器识别，它与`<bean id="userDao" class="com.UserDao" /> `等效

此外，还有三种作用相同的注释：

`@Repository`用于对dao实现类进行标注

`@Service`用于对Service实现类进行注释

`@Controller`用于对Controller实现类进行注解



#### 2、扫描注解定义的bean

配置完注解后需要在beans.xml中配置`<context:component-scan base-package="xxx"/>`，扫描类包以应用注解定义bean的方式

```xml
<context:component-scan base-package="com.smart.dao"/>
<context:component-scan base-package="com.smart.service"/>
```

`base-package`指定一个需要扫描的基包；

`resource-pattern="xxx"`过滤扫描基包中的类

**还有其他具体的子元素可查书**



#### 3、自动装配bean

两种方式：在类的属性上注入`@Autowired`、在set方法上注入`@Autowired`(一般采用set方法)

set方法

```java
@Autowired
public void setUserDao(UserDao userDao) {
  this.userDao = userDao;
}

@Autowired
public void setLoginLogDao(LoginLogDao loginLogDao) {
  this.loginLogDao = loginLogDao;
}
```

属性

```java
@Autowired
private UserDao userDao;
@Autowired
private LoginLogDao loginLogDao;
```



#### 4、对集合类进行标注

```java
@Autowired
private List<Plugin> plugins;
@Autowired
private Map<String ,Plugin> pluginMaps;
```

若变量是list或map集合类，则spring容器将匹配的集合元素类型的所有bean都注入进来，map中key是bean的名字，value是所有实现了Plugin的bean；当有多个bean时，其加载顺序不确定，因此通过`@Order(value = 1)`来指定加载顺序，其中value值越小，越被优先加载



#### 5、延迟依赖注入

```java
@Lazy
@Repository
public class LogDao{
  
}

...

@Service
 public class LoginService implements BeanNameAware{
   
   @Lazy
   @Autowired
   public void setLogDao(LogDao logDao){
   ...  
 }
```

对bean实施延迟依赖注入，需要在属性及目标bean上标注@Lazy，缺一不可



#### 6、bean作用范围及生命方法

Spring提供了`@Scope`注解，通过它显示指定bean的作用范围，如`Scope("prototype")`    默认为singleton



通过注解`@PostConstruct`和`@PreDestory`来定义bean初始化及容器销毁前执行的方法



### 二、使用基于java类的配置信息启动spring容器



#### 1、直接通过被@Configuration标记的类启动Spring容器

Spring提供了一个AnnotationConfigApplicationContext类，直接通过标注`@Configuration`的java类启动Spring容器

例：

被标注的类

```java
@Configuration
public class AppConf {
  @Bean
  public UserDao userDao(){
    return new UserDao();	
  }

  @Bean
  public LogonService logonService(){
    LogonService logonService = new LogonService();
    logonService.setUserDao(userDao());
    return logonService;
  }
```

启动容器：

```java
ApplicationContext ctx = new AnnotationConfigApplicationContext(DaoConfig.class,ServiceConfig.class);
```

**个人感悟**：类AppConf可相当于beans.xml，里面可以通过注解定义bean等

```java
@Bean
  public LogonService logonService(){
    LogonService logonService = new LogonService();
    logonService.setUserDao(userDao());
    return logonService;
  }
```

相当于：

```xml
<bean id="logonService" class="com.service.LogonService">
  <property name="xxxx" ref="userDao" />
</bean>
<bean id="userDao" class="com.dao.UserDao">
  <property name="xxxx" value="userDao()" />
</bean>
```



还可以在`@Configuration`后引入xml配置文件,如`@ImportResource("classpath:com/smart/beans.xml")`





![](http://ot0aou666.bkt.clouddn.com/bean%E4%B8%8D%E5%90%8C%E9%85%8D%E7%BD%AE%E6%96%B9%E5%BC%8F1.png)





![](http://ot0aou666.bkt.clouddn.com/bean%E4%B8%8D%E5%90%8C%E9%85%8D%E7%BD%AE%E6%96%B9%E5%BC%8F2.png)



## 插入：spring内部工作机制

![](http://ot0aou666.bkt.clouddn.com/Ioc%E7%9A%84%E6%B5%81%E6%B0%B4%E7%BA%BF.png)



上图为实例一个bean的具体流程，书中有说明spring容器在加载配置文件后的各项处理，以及对于BeanDefinition等类都有作说明，此笔记暂不记录流程文字及各个类的详细功能



### 三、引用bean的属性值

1、`@value("#{xxxx}")`注解可以**引用其他bean的属性**，例如：



UserDao类

```java
@Repository
public class UserDao {

	String name = "111";
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	
}
```

UserService类：

```java
@Service
public class UserService {
	@Value("#{userDao.name}")
	private String name;

	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
}
```

则UserService的name值为`name=="111"`



可通过`#{beanName.propName}`表达式引用某个bean里面的某个属性值，如：

```xml
<bean id="sysConfig" class="com.smart.beanprop.SysConfig"
	      init-method="initFromDB"
	      dataSource-ref="dataSource"/>

<bean class="com.smart.beanprop.ApplicationManager"
      maxTabPageNum="#{sysConfig.maxTabPageNum}"
      sessionTimeout="#{sysConfig.sessionTimeout}"/>  
```





### 四、使用外部属性文件(以连接数据库为例)

#### 1、使用PropertyPlaceholderConfigurer属性文件

jdbc.properties

```properties
driverName=com.jdbc.mysql.Driver
url=jdbc:mysql://localhost:3306/web
username=root
password=123456
```

beans.xml

```xml
<!-- 引入属性文件 -->
<bean class="com.smart.placeholder.EncryptPropertyPlaceholderConfigurer"
	    location="com/smart/placeholder/jdbc.properties"
		fileEncoding="utf-8"/>

<bean id="dataSource" class="org.apache.commons.dbcp.BasicDataSource"
		destroy-method="close" 
       	driverClassName="${driverClassName}"
      	url="${url}"
		username="${userName}"
      	password="${password}" />
```



#### 2、使用`<context:property-placeholder/>`引用属性文件

使用context命名空间定义属性文件：

```xml
<!-- 引入属性文件 -->
<context:property-placeholder
		location="com/smart/placeholder/jdbc.properties"/>
```



#### 3、基于注解及基于java类的配置中引用属性

```java
@Component
public class MyDataSource {
    @Value("${driverClassName}")
	private String driverClassName;
    
    @Value("${url}")
	private String url;
    
    @Value("${userName}")
	private String userName;
    
    @Value("${password}")
	private String password;
    
    public String toString(){
    	 return ToStringBuilder.reflectionToString(this);
    }	
}
```



## 第九、配置数据源

### 1、DBCP数据源

DBCP是一个依赖commons-pool对象池机制的数据库连接池，**因此在类路径下必须包括commons-pool的类包**，以下是DBCP配置mysql数据源的片段：

```xml
<bean id="dataSource" class="org.apache.commons.dbcp.BasicDataSource"
		destroy-method="close" 
		driverClassName="${driverClassName}" 
		url="${url}"
		username="${userName}" 
		password="${password}"/>
```

BasicDataSource提供了close（）方法关闭数据源，因此必须设定destory-method="close"属性，以便spring容器关闭时，数据源正常关闭

**其余属性在书中查找**



### 2、C3P0数据源

配置代码与DBCP数据源类似，拥有更丰富的配置属性，对数据源进行各种有效的控制，**详细配置属性查书**





## 另：spring的数据源实现类

spring自身提供了简单的数据源实现类DriverManagerDataSource；每次调用getConnection()方法获取新连接时，只是简单地创建一个新连接，因此此数据源类比较适合在单元测试或简单的独立应用中使用,因为它不需要额外的依赖类

```java
DriverManagerDataSource ds = new DriverManagerDataSource();
ds.setDriverClasssName("com.mysql.jdbc.Driver");
ds.setUrl("jdbc:mysql://localhost:3306/web");
ds.setUsername("root");
ds.setPassword("123456");
Connection con = ds.getConnection();
```





## 第十、配置事务管理器(Spring JDBC和MyBatis)

事务管理：无须关注资源获取、复用、释放事务同步和异常处理等操作

```xml
<!-- 配置数据源 -->
<bean id="dataSource" class="org.apache.commons.dbcp.BasicDataSource"
      destroy-method="close" 
      p:driverClassName="com.mysql.jdbc.Driver"
      p:url="jdbc:mysql://localhost:3306/sampledb"
      p:username="root"
      p:password="123456" />

<!-- 配置事务管理器 -->
<bean id="transactionManager"
      class="org.springframework.jdbc.datasource.DataSourceTransactionManager"
      p:dataSource-ref="dataSource" />
```

在幕后，DataSourceTransactionManager使用DataSource的Connection的commit（）、rollback（）等方法管理事务



### 另：

spring将dao中重复代码（包括连接、关闭数据库等）抽取成一个模板供使用，ThreadLocal类保证了多个线程使用同一个模板时不会出现线程问题（通过为每个线程创造一个本地变量）



## 第十一、事务传播行为

当调用一个基于spring的service接口方法时，它将运行于spring管理的事务环境中，service接口方法可能会在内部调用其他service接口方法以共同完成一个完整的业务操作，因此会产生服务接口方法嵌套调用的情况，spring通过事务传播行为控制当前的事务如何传播到被嵌套调用的目标服务接口方法中；

事务传播行为如下：

![](http://ot0aou666.bkt.clouddn.com/%E4%BA%8B%E5%8A%A1%E4%BC%A0%E6%92%AD%E8%A1%8C%E4%B8%BA.png)





## 第十二、使用xml配置声明式事务



Spring的声明式事务管理是通过Spring AOP实现的，通过事务的声明性信息，Spring负责将事务管理增强逻辑动态织入业务方法的相应连接点中。这些逻辑包括获取线程绑定资源、开始事务、提交/回滚事务、进行异常转换和处理等工作。

其中回滚规则使用户能够指定什么样的异常导致自动回滚、什么样的异常不影响事务提交，这些规则均可在配置文件中声明指定

### 1、声明式事务配置

BbtForum

```java
public class BbtForum {
    public ForumDao forumDao;

    public TopicDao topicDao;

    public PostDao postDao;

    public void addTopic(Topic topic) throws Exception {
        topicDao.addTopic(topic);
		if(true) throw new PessimisticLockingFailureException("fail");
        postDao.addPost(topic.getPost());
    }

    public Forum getForum(int forumId) {
        return forumDao.getForum(forumId);
    }

    public void updateForum(Forum forum) {
        forumDao.updateForum(forum);
    }

    public int getForumNum() {
        return forumDao.getForumNum();
    }

    public void setForumDao(ForumDao forumDao) {
        this.forumDao = forumDao;
    }

    public void setPostDao(PostDao postDao) {
        this.postDao = postDao;
    }

    public void setTopicDao(TopicDao topicDao) {
        this.topicDao = topicDao;
    }
}
```

使用TransactionProxyFactoryBean配置

```xml
<!-- applicationContext-dao.xml中配置好Dao bean、数据源等并在此定义 -->
<import resource="classpath:applicationContext-dao.xml" />
<!-- 声明事务管理器 -->
<bean id="txManager"
      class="org.springframework.jdbc.datasource.DataSourceTransactionManager">
  <property name="dataSource" ref="dataSource" />
</bean>
<!-- 需要实施事务增强的目标业务Bean -->
<bean id="bbtForumTarget" 
      class="com.smart.service.BbtForum"
      p:forumDao-ref="forumDao"
      p:topicDao-ref="topicDao"
      p:postDao-ref="postDao"/>
<!-- 使用事务代理工厂类为目标业务bean提供事务增强 -->
<bean id="bbtForum"
      class="org.springframework.transaction.interceptor.TransactionProxyFactoryBean"
<!-- 指定事务管理器 -->
      p:transactionManager-ref="txManager"
<!-- 指定目标业务Bean -->
      p:target-ref="bbtForumTarget">
<!-- 事务属性配置 -->
  <property name="transactionAttributes">
    <props>
      <prop key="addTopic">
        PROPAGATION_REQUIRED,+PessimisticLockingFailureException
      </prop>
      <prop key="get*">PROPAGATION_REQUIRED,readOnly</prop><!-- 只读事务 -->
      <prop key="*">PROPAGATION_REQUIRED,-tion</prop><!-- 可写事务 -->
    </props>
  </property>
</bean>
```

其中`get*`代表目标类中所有以get为前缀的方法，`key="*"`匹配BbtForum接口的所有方法



#### 异常回滚/提交规则

规则如下：

![](http://ot0aou666.bkt.clouddn.com/%E4%BA%8B%E5%8A%A1%E6%8F%90%E4%BA%A4%E8%A7%84%E5%88%99.png)

![](http://ot0aou666.bkt.clouddn.com/%E5%BC%82%E5%B8%B8%E5%9B%9E%E6%BB%9A.png)

![](http://ot0aou666.bkt.clouddn.com/%E5%BC%82%E5%B8%B8%E5%9B%9E%E6%BB%9A2.png)



### 2、使用@Transactional注解进行事务配置

业务类：

```java
@Service
@Transactional
public class BbtForum {
    public ForumDao forumDao;

    public TopicDao topicDao;

    public PostDao postDao;

    public void addTopic(Topic topic) throws Exception {
        topicDao.addTopic(topic);
		//if(true) throw new PessimisticLockingFailureException("fail");
        postDao.addPost(topic.getPost());
    }
  ...
}
```

使事务注解生效：

```xml
<bean id="transactionManager"
          class="org.springframework.jdbc.datasource.DataSourceTransactionManager"
          p:dataSource-ref="dataSource"/>
<!-- transaction-manager默认为transactionManager，因此可以省去 -->
<tx:annotation-driven transaction-manager="transactionManager"/><!-- 对标注注解的bean进行加工以织入事务管理切面 -->
```



关于`@Transactional`注解默认设置的属性在大多数情况下都是适用的，此处不记录，可查书





















