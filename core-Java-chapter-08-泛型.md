# 泛型

()源码 ---> (原生类型)字节码 ----> (原生类型)运行期程序

泛型与类型擦除 Java中的参数化类型只在源码中存在，在编译后的字节码中，已经被替换为原来的原生类型了，并且在相应的地方插入了强制转换代码。对于运行期的Java 语言来说，ArrayList<Integer>和ArrayList<String>就是同一个类。所以说泛型技术实际上就是 Java语言的一颗语法糖，Java语言中的泛型实现方法称为类型擦除，基于这种方法实现的泛型称为伪泛型

=========================================================================

**语法糖**（Syntactic sugar），也译为**糖衣语法**，是由[英国](https://zh.wikipedia.org/wiki/%E8%8B%B1%E5%9B%BD)[计算机](https://zh.wikipedia.org/wiki/%E8%AE%A1%E7%AE%97%E6%9C%BA)科学家[彼得·兰丁](https://zh.wikipedia.org/wiki/%E5%BD%BC%E5%BE%97%C2%B7%E5%85%B0%E4%B8%81)发明的一个术语，指计算机语言中添加的某种语法，这种语法对语言的功能没有影响，但是更方便[程序员](https://zh.wikipedia.org/wiki/%E7%A8%8B%E5%BA%8F%E5%91%98)使用。语法糖让程序更加简洁，有更高的可读性。

语言处理器，包括编译器和静态分析器，在处理的过程中，首先把“加糖”的
结构变成基本的结构，这个过程叫做“去糖”。

===========================================================================

## 1. 类型参数的好处

早前的版本：

```java
public class ArrayList {
    private Object[] elementData;
	...
  	public Object get(int i){...}
  	public void add(Object o){...}
}
```

自Java SE5及以后的版本

增加了类型参数这一个概念，也就是下面的T，有了T，编译器可以进行检查，避免插入错误类型的对象。

而后会结果语言处理器(这里包括编译器和静态分析器)，在处理过程中，会把"加糖"的结构变成基本的结构。这个过程叫做"去糖"

```java
public class ArrayList<T> {
    private Object[] elementData;
  	...
    public T get(int i){...}
  	public void add(T t){...}
}
```

那用法区别：

| 版本           | get             | add              | 其他   |
| ------------ | --------------- | ---------------- | ---- |
| 早前版本         | 需要强制转换          | 没有错误检查           |      |
| Java SE 5及以后 | 编译器知道必须是指定的类型参数 | 同样编译器也知道是指定的类型参数 |      |

## 2. 定义简单泛型类

一个泛型类(generic class)就是具有一个或多个类型变量的类。

```java
package genericclass;

public class Pair<T> {
    private T first;
    private T second;

    public Pair(T first, T second) {
        this.first = first;
        this.second = second;
    }

    public Pair() {
    }

    public T getSecond() {
        return second;
    }

    public void setSecond(T second) {
        this.second = second;
    }

    public T getFirst() {

        return first;
    }

    public void setFirst(T first) {
        this.first = first;
    }
}
```

## 3. 泛型方法

```java
package genericclass;

public class ArrayAlg {
    public static <T> T getMiddle(T... a){
        return a[a.length/2];
    }
    public static void main(String[] args) {
        ArrayAlg.getMiddle("hello", 0, null);
    }
}
```

## 4. 类型变量的限定

```java
//extends Comparable 就是是对类型变量设置限定
public static <T extends Comparable> T min(T[] a){
  if (a == null || a.length == 0) return null;
  T smallest = a[0];
  for (int i = 1; i < a.length; i++)
  {
    if (smallest.compareTo(a[i]) > 0) {
      smallest = a[i];
    }
  }
  return smallest;
}
```

- 这里的限定，原意是T应该是绑定类型的子类型 

  由于java设计者并不打算添加新的关键字，因此选择使用extends，因为更符合这里子类的概念

- 而这里的绑定类型可以是类也可以是接口

- 分隔语法： boundingType1 & boundingType2

## 5.泛型代码和虚拟机

### 5.1.类型擦除

定义一个泛型类型 ---> 都自动提供了一个相应的原始类型(raw type)。

**原始类型的名字就是删去类型参数后的泛型类型名。**

擦除类型变量，并替换为限定类型，没有就为Object

```java
package genericclass;

public class Pair<T> {
  // T ----> Object/限定类型
    private T first;
    private T second;

    public Pair(T first, T second) {
        this.first = first;
        this.second = second;
    }

    public Pair() {
    }

    public T getSecond() {
        return second;
    }

    public void setSecond(T second) {
        this.second = second;
    }

    public T getFirst() {

        return first;
    }

    public void setFirst(T first) {
        this.first = first;
    }
}

```

### 5.2.翻译泛型表达式

当程序调用泛型方法时，如果擦除返回类型，编译器则会自动插入强制类型转换。

```java
Pair<Employee> buddies = ...;
Employee buddy = buddies.getFirst();//这个是泛型方法，会在结果字节码中插入类型转换
```

getFirst的返回类型后将返回给Object类型。编译器自动插入Employee的强制类型转换，也就是说，编译器把这个方法调用翻译为两条虚拟机指令:

- 对原始方法Pair#getFirst
- 将返回的Object类型强制转换为Employee类型。


### 5.3. 翻译泛型方法

类型擦除也会出现在泛型方法中

```java
public static <T> T test(T... a)
```

擦除后：

```java
public static <Object> Object test(Object... a)
```

擦除带来的问题：

1. 子类参数类型

   ```java
   class A extends Pair<Date>{
       public void setFirst(Date first){
           ...
             //这是重写方法
       }
   }
   // ----> 擦除后：
   class A extends Pair(){
       public void setFirst(Date first){
           ...
       }
   }
   ```

   这里是A类的一个方法，同时 继承父类的方法： public void setFirst(Object first),//但事实上是应该一样才行，希望对setFirst方法的调用具有多态性，并调用最合适的那个方法。

   那类型擦除带来的问题就是不能想其他父子类一样正常的发挥他的多态性

   在这里解决方法就是：桥接方法。

   ```java
   Pair<Date> a = new A();
   a.setFirst(new Date());
   ```

   如何理解桥接方法：可以看这么一条语句，去跟踪a#setFirst，

   Pair<Date>类型只有一个方法setFirst(Object)，虚拟机用a引用的对象调用这方法。这个对象的实际类型是A，因而将会调用A.setFirst(Object)方法。这个方法就是桥接方法。这个方法会调用A.setFirst(Date).如此便是我们所要达到的效果。

   **父类#setFirst(Object)----->(根据多态性)子类#setFirst(Object)------->(将子类的setFirst(Object)桥接到最应该被调用的方法也就是setFirst(Date))-----> 子类#setFirst(Date)**

   2.子类返回类型

   根据上面的桥接方法，会出现下面的这种情况

   ```java
   Date getFirst() // 这是在子类定义的
   Object getFirst() //重写父类的方法
   ```

   解决方法：

   实际上这样编写代码肯定不行，好在，java虚拟机中，用参数类型和返回类型是可以确定一个方法的。因此，编译器可能产生两个仅仅返回类型不同的方法字节码，而虚拟机可以正确处理这样的情况。

ps：

注意了： 桥接方法其实用处不仅仅在泛型上，还会用在当： 一个方法覆盖另一个方法时，指定一个更严格的返回类型

 ```java
public class Employee implements Clonnable{
    public Employee clone() throws CloneNotSupportException{...}
}

//实际上Employee类就有两个克隆方法
Employee clone() //这是上面重写的那个方法
Object clone()//这是桥接方法，用来重写父类的clone，并去调用上面那个方法
 ```

桥接方法： 用于解决泛型和多态发生的冲突

## 6.约束与局限性

### 6.1. 不能用基础类型实例化类型参数

### 6.2.运行时类型查询只适用于原始类型

```java

public class Awful<T> {


}    
class  Main{
	public static void main(String[] args) {
        Awful<String> a = new Awful<String>();
        Awful<Integer> c = new Awful<Integer>();
        //1. 尝试在运行时判断泛型类型即类型参数
//        System.out.println(a instanceof Awful<String>);
        //2.尝试强制转换
//        Awful<Integer> b = (Awful<Integer>)a;
        //3.两个getClass返回的都是getClass
//        System.out.println(c.getClass() == a.getClass());
    }
}
```

### 6.3. 不能创建参数化类型的数组

```java
//这是不行的 Error
//Awful<String>[] as = new Awful<String>[10];
```

为什么：

- 正常数组遇到将错误的元素类型赋值给数组，会报异常ArrayStoreException

```java
		Integer[] integers = new Integer[10];
        Object[] objarray = integers;
//        objarray[0] = "Hello";
        //会报异常ArrayStoreException
```

- 可是对于泛型类型，擦除会使得这种机制无效。以下赋值

```java
Awful<String>[] as = new Awful<String >[10];//是错误的
```

只能检查原始类型，而不能检查类型参数的类型

### 6.4.Varargs警告

这是出现在java虚拟机必须建立一个泛型的数组也就是T[]的时候。

```java
class Awful2{
    static <T> void addAll(Collection<T> coll, T...ts){
        for (T t : ts)
        {
            coll.add(t);
        }
    }
}
```

### 6.5.不能实例化变量

```java
new T(...), new T[...], T.class
```

上面这样构造

因为擦除之后会变成Object或者其限定类型，这显然是不符合我们的本意

### 6.6.不能构造泛型数组

```java
public static <T extends Comparable> T[] minmax(T[] a){
    T[] mm = new T[2];
}//报Error
```

数组本身也有类型，用来监控存储在虚拟机中的数组，而使用泛型，数组本身的类型就只是它的限定类型，不能是其他的

### 6.7.泛型类的静态上下文类型变量无效

```java
//泛型类的静态上下文类型变量无效
class Awful5<T>{

//    public static T singleInstance;
    //Error
//    public static T getSingleInstance();
    //Error
}
```

### 6.8.不能抛出或捕获泛型类的实例

```java
//不能抛出或捕获 泛型类的实例
//Error 泛型不能继承Exception
/*
class Awful6<T> extends Exception{

}*/
```

### 6.9.可以消除对受查异常的检查

```java
//消除受查异常的检查
class Awful8{
    static <T extends Throwable> void throwAs() throws ClassNotFoundException {
        try{

        }catch (Throwable t) {

            Awful8.throwAs(t);

            // 在这里 编译器会认为t是一个非受查异常因此在这个方法不会throw出去
        }
    }

    static <T extends Throwable> void throwAs(Throwable e) throws T, RuntimeException, ClassNotFoundException {
        throw (T) e;
        
    }
}
```

抛出非受查异常方法，被调用的时候，无需catch住，因为对于泛型，编译器会把所有异常都转换为编译器所认为的非受查异常

### 6.10.注意擦除冲突

```java
class Foo implements Comparable<Foo>{...}
class Bar extends Foo implements Comparable<Bar>{...}
```

这样子Bar实现了Comparable<Bar>和Comparable<Foo> 那么根据类型擦除，它会产生两个桥接方法，而这两个桥接方法，一个桥接方法int compareTo(Object other)将调用int CompareTo(Foo foo)，另一个就会调用int CompareTo(Bar bar)就会产生冲突

## 7.泛型类型的继承规则

```java
class Foo {
}
class Bar extends Foo{

}
public class Main{
    @Test
    public void test(){
        Foo[] foos = new Foo[10];
        Bar[] bars = new Bar[10];
        foos[0] = new Bar();
//        bars[0] = new Foo();
//        bars1.add(new Foo());
        //如果说ArrayList<Foo> ArrayList<Bar>是父子关系，那么就说明父类引用可以指向子类对象,如下，但是显然是不行的
//        ArrayList<Foo> foos2 = new ArrayList<Bar>();
        
    }
}
ArrayList<Foo>和ArrayList<Bar>毫无关系
```

# ======================================



# ======================================

补充点： - 编译器泛型操作

​		- 擦除带来的问题

​		- 结构化信息的保留(这里涉及字节码文件结构)

### 5.1. 类型擦除

虚拟机当中： **没有擦除概念**， 所有对象都是普通类。

1. 擦除概念
2. 底层原理
3. 带来的问题及其具体的情况还有局限性
4. 桥接方法

#### 要点：

1. 概念：

   定义一个泛型类型 ，都自动提供了一个相应的原始类型(raw Type)。原始类型的名字就是删除类型参数后的泛型类型名。擦除类型变量，并替换为限定类型（没有限定类型的时候就默认Object类）。

   Java泛型的处理几乎在编译器中进行，编译器生成的bytecode不包含泛型信息，泛型信息会在编译处理时被擦除。
   类型擦除可以简单理解为将泛型java代码转换成普通java代码，只不过编译器更直接点，将泛型java代码直接转换成普通的java字节码。
   小结：类型擦除过程：

   1. 所有泛型参数用其限定类型替换
   2. 移除所有的类型参数

2. 编译器处理泛型的操作：

   - Code specialization(代码特殊化)。在实例化一个泛型类或泛型方法时都需要产生一份新的目标代码(字节代码or二进制代码):比如:针对一个泛型list，可能需要针对String，Integer，Float 产生目标代码
   - Code Sharing(代码共享)。对每个泛型类只生成唯一的一份目标代码;该泛型类的所有实例都映射到这份目标代码上，需要的时候执行类型检查和类型转换。

   C++的Template模板时典型的Code Specialization,这样的坏处：代码膨胀(Code Bloat);在引用类型系统中，浪费空间。因为引用类型集合中元素本质上都是一个指针。没必要为每个类型都产生一份执行代码。

   上面的坏处也是 ：**Java编译器采用Code Sharing(代码共享)方式处理泛型**的主要原因。

3. 类型擦除

   类型擦除：通过类型参数合并，将泛型类型实例关联到同一份字节码上。编译器只为泛型类型生成一份字节码，并将其实例关联到这份字节码上。类型擦除的关键：从泛型类型中清除类型参数的相关信息，并且在必要的时候添加类型检查和类型转换的方法。



####类型擦除带来的问题


​        正是由于类型擦除的隐蔽存在，直接导致了众多的泛型灵异问题。

   - 不能用同一泛型类的实例区分方法签名？

     参数类型明明不一样啊，一个List<String>，一个是List<Integer>，

   - 不能同时catch同一个泛型异常类的多个实例？

     同理，如果定义了一个泛型一场类GenericException<T>，不能catch GenericException<Integer>和GenericException<String>，因为他们是一样的

   - 泛型类的静态变量是共享的？

     所有的泛型类实例都关联到同一份字节码上，泛型类的所有静态变量是共享的。

#### 注意

   1.虚拟机中没有泛型，只有普通类和普通方法
   2.所有泛型类的类型参数在编译时都会被擦除，都用它们的限定类型替换
   3.创建泛型对象时请指明类型，让编译器尽早的做参数检查（Effective Java，第23条：请不要在新代码中使用原生态类型）
   4.不要忽略编译器的警告信息，那意味着潜在的ClassCastException等着你。

#### 保留结构化信息

是不是<>里面的类型参数会被擦除呢？？

答案是错误的

`collection/src/java/Erasure`

```java
package Erasure;

import java.util.ArrayList;
import java.util.List;

public class Demo01 <T>{
    public List<T> name;
    public T psw;
    public void aTestMethod(List<T> s){

    }

    public Demo01(List<T> name) {
        this.name = name;
    }

    public Demo01() {
    }

}
class Demo02 {
    public Demo01<String> name;
    public void aTestMethod(Demo01<String> demo01){

    }
}
class Demo03 extends Demo01<String>{

}

final class Test02{
    public static void main(String[] args) throws NoSuchFieldException, NoSuchMethodException {
        Demo01<String> demo01 = new Demo01();
        Demo02 demo02 = new Demo02();
        Demo03 demo03 = new Demo03();
        System.out.println("Demo01");
        System.out.println(demo01.getClass().getField("name").getGenericType());
        System.out.println(demo01.getClass().getMethod("aTestMethod", List.class).getGenericParameterTypes()[0]);
        System.out.println("Demo02");
        //demo02表示：结构化信息不会被擦除
        System.out.println(demo02.getClass().getField("name").getGenericType());
        System.out.println(demo02.getClass().getMethod("aTestMethod", Demo01.class).getGenericParameterTypes()[0]);
        System.out.println("Demo03");
        //demo03表示：类型擦除会擦除类型参数，只保留限定类型
        System.out.println(demo03.getClass().getField("name").getGenericType());
        System.out.println(demo03.getClass().getMethod("aTestMethod", List.class).getGenericParameterTypes()[0]);
    }
}
```

会发现每一个类型参数都被保留了，而且在运行期可以通过反射机制获取到。那么到底什么是“类型擦除”？至少某些东西被擦除了吧？是的。事实上，除了结构化信息外的所有东西都被擦除了 —— 这里结构化信息是指与类结构相关的信息，而不是与程序执行流程有关的。

而其他的信息都被擦除掉了



那什么是结构化信息呢？这里就涉及到类文件结构的具体信息了，不做详细描述。

#### 桥接方法

桥接方法是java虚拟机对多态的一种合成方法，java虚拟机在泛型使用，以及继承过程中，会选择性的调用方法。

简单点说：在实际情况下Employee会有两个方法：Employee#clone和Object#clone，而新合成的方法会调用新定义的方法即Employee#clone；在泛型中，我们看见

#### 局限性

1. 不能用基本类型实例化类型参数

2. 运行时的类型查询只适用于原始类型

   ```java
   if(a instanceof Demo<String>){
       
   }
   if(a instanceof Demo<T>){
       
   }
   ```

   上面两个语句编译器会执行一样的操作，去查看a是否是任意参数类型的Demo，同时a#getClass也是只能得到原始类型而不包括参数类型

3. 不能创建参数化的数组

   就是说不能执行： 

   ```Demo&lt;String&gt;[] demo = new Demo&lt;String&gt;[10];java
   Demo<String>[] demo = new Demo<String>[10];
   ```

   擦除之后会变成Demo[]可以把他转换成Object，数组会记住元素类型，而对于泛型，他也只能记住原始类型，不能记住类型参数，若试图存储其他类型的元素就会抛出数组存储异常ArrayStore异常，

4. Varargs警告

   若向参数可变的方法传递一个泛型类型的实例

   ```java

   class Varargs{
       public static <T> void addAll(Collection<T> coll, T... ts){
           //Java虚拟机就必须建立Demo<String>数组，这就违反了上一个规则
           for (T t : ts)
           {
               coll.add(t);
           }
       }
   	//对于这种情况，我们就会得到一个警告，而不是一个错误，可以有两种方法来抑制
     	//1.@SupressWarnings("unchecked")标注方法
   	//2.@SafeVarargs标注addAll方法
       public static void main(String[] args) {
           Collection<Demo01<String>> table = new ArrayList<Demo01<String>>();
           Demo01<String> demo01 = new Demo01<String>();
           Demo01<String> demo02 = new Demo01<String>();
           addAll(table,demo01,demo02);
       }
   ```

5. 不能实例化类型变量

   不能使用像` new T()`这样的表达式中的类型变量

   就是说不能

   ```java
   public Demo(){
       psw = new T();
     //这样的话就相当于new Object(),是不可以的
   }
   //解决方法：java8会让调用着提供一个构造器表达式
   public Demo(Supplier<T> suplier){
     psw = supplier.get(); 
   }
   @Test
   public void test(){
       new Demo(String::new);
   }
   ```

6. 不能构造泛型数组

   就是不能new T[2]，因为擦除之后会变为Object[]又或者是他的边界类型

7. 泛型类的静态上下文中类型变量无效

   ```java
   //    static T aStaticField;
   /*
   	public static T getAStaticField(){
         return aStaticField;
   	}
   */
   ```

8. 桥接方法的使用要避免擦除冲突

   ```java
   package Erasure;
   interface AInterface<T>{
       void method(T first);

   }
   public class Demo05 implements AInterface<String>{

       @Override
       public void method(String first) {
           System.out.println("继承AInterface");
       }
   }
   package Erasure;

   //public class Demo06 extends Demo05 implements AInterface<Integer>{
   //
   //}

   //不能使同一个类同时成为同一个原始类型的两个不同参数化的接口的子类
   //因为会与桥接方法产生的冲突
   ```

   ​

   ​

   ​

   ​

   ​

   ​

   ​

http://docs.oracle.com/javase/tutorial/java/generics/bridgeMethods.html#bridgeMethods

   # Type Erasure

   Generics were introduced to the Java language to provide tighter type checks at compile time and to support generic programming. To implement generics, the Java compiler applies type erasure to:

   - Replace all type parameters in generic types with their bounds or `Object` if the type parameters are unbounded. The produced bytecode, therefore, contains only ordinary classes, interfaces, and methods.
   - Insert type casts if necessary to preserve type safety.
   - Generate bridge methods to preserve polymorphism in extended generic types.

   Type erasure ensures that no new classes are created for parameterized types; consequently, generics incur no runtime overhead.

