# Shiro

## 一、  身份认证

### 1、subject认证主体



一个subject认证主体包含两个信息：

* Principals(`[p'rɪnsəplz]`)：身份。可以是用户名、邮件、手机号码等，以标识一个登录主体身份；
* Credentials(`[krəˈdenʃlz`])：凭证。一般为密码、数字证书等等

在`.ini`文件里面指定一个认证主体为以下代码：

```ini
[users]
jack=123456
#其中jack为身份，123456为凭证
```



验证一个主体可以有效地分为以下几个不同的步骤：

- Step1：收集主体提交的身份和凭证（Collect the Subject's principals and credentials）

  Api实现：

  ```java
  //根据用户名和密码获得一个令牌（token）
  UsernamePasswordToken token = new UsernamePasswordToken(username, password);
  //token.setRememberMe(true); //可选用
  ```

  `UsernamePasswordToken`实现了`AuthenticationToken`接口，因此在下面的`subject.login(token)`方法中，即使参数是`AuthenticationToken`类的，也能直接将`UsernamePasswordToken`类的token传入

  ​

  ​

  ```java
  public class UsernamePasswordToken implements HostAuthenticationToken, RememberMeAuthenticationToken {
      private String username;
      private char[] password;
      private boolean rememberMe;
      private String host;
  ```

  `UsernamePasswordToken`类中有一个password的属性，它是一个`char[] `类型，而不是`String`类型，原因是在`UsernamePasswordToken`中有一个方法是`clear()`，很明显该方法是为了将`password`清空，以下为其代码：

  ```java
  public void clear() {
    this.username = null;
    this.host = null;
    this.rememberMe = false;
    if (this.password != null) {
      for(int i = 0; i < this.password.length; ++i) {
        this.password[i] = 0;
      }
      this.password = null;
    }
  }
  ```

  当用`String password = "xx"`对密码进行赋值时，在编译时期会创建一个String对象并存放在字符串常量池中；而当用`String password = new String("xx")`创建String对象时，则会在常量池中查找有没有池对象与“xx” equals，如果有则将其value值与hashcode传入`String`的构造方法中，如果没有则会创建一个对象再传入构造方法，因此无论是哪种方法，常量池中都会存在一个`String`对象“xx”，所以无论是否有将password设为`null`，都能在内存中找到从而导致信息泄露。因此对于password这种安全敏感数据，采用`char[]`类型

  ​

  ​

- Step2：提交该身份和凭证（Submit the principals and credentials）

  Api实现：

  ```java
  //得到当前执行的用户
  Subject currentUser = SecurityUtils.getSubject();
  //进行认证
  currentUser.login(token);//参数是AuthenticationToken类但传入的是UsernamePasswordToken类的token
  ```

  步骤二中，先通过SecurityUtils获取当前执行用户再进行身份认证

  ​

- Step3：如果提交成功则允许访问，否则重新尝试身份认证或阻止访问(Handling Success or Failure)

  Api实现：

  ```java
  try {
      currentUser.login(token);
  } catch ( AuthenticationException ae ) {
      //unexpected error?
      //Handel error
  }
  ```

  在步骤三中用`try catch`来处理身份认证失败的异常

  ​

### 2、身份认证过程

![](http://ot0aou666.bkt.clouddn.com/shiro%E8%BA%AB%E4%BB%BD%E8%AE%A4%E8%AF%81.png)

如图：

1. 代码调用`subject.login(token)`方法后，传入用户的身份和凭证事例token

2. 将subject实例委托给`SecurityManager`，`SecurityManager`实例调用`securityManager.login(token)`方法

3. `SecurityManager`接收到token信息后委托内置的认证器`ModularRealmAuthenticator`实例，调用`modularRealmAuthenticator.authenticate(token)`方法，在认证过程中对设置的一个或多个Realm实例进行适配

4. 若配置了多个Realm，`ModularRealmAuthenticator`会根据配置的`AuthenticationStrategy`(认证策略)来进行多Realm的认证过程。如果仅配置了一个Realm，则被直接调用而无需配置认证策略

5. 判断每个Realm是否支持token，如果支持，realm则调用`getAuthenticationInfo(token)`方法进行实际认证处理，我们可以通过覆盖Realm的`doGetAuthenticationInfo`方法来自定义我们的认证过程



### 3、SecurityManager

安全管理类实现了多种支持，包括缓存`CachingSecurityManager`、域`RealmSecurityManager`、身份验证器`AuthorizingSecurityManager`、授权`AuthorizingSecurityManager`、Session管理`SessionSecurityManager`，按顺序逐个继承

![](http://ot0aou666.bkt.clouddn.com/shiro%20SecurityManager.png)



```java
public String login(User user , HttpServletRequest request){
  Subject subject = SecurityUtils.getSubject();
  UsernamePasswordToken token = new UsernamePasswordToken(user.getUserName() ,user.getPassword());
  try{
    subject.login(token);//subject.login
    Session session = subject.getSession();
    session.setAttribute("info" ,"session的数据");
    return "redirect:/Success.jsp";
  }
```

我们以登录操作为例，说明一下身份认证的过程是怎么实现的。上面是Controller类的一段登录功能的代码，调用了`Subject`接口的`login`方法，而实际上，` DelegatingSubject` 类是`Subject`接口的实现类，`Subject`接口的`login`、`logout`等方法都是由` DelegatingSubject`实现的，而 ` DelegatingSubject`中的`login`方法又是调用了`SecurityManager`接口的`login`方法，如以下代码

```java
//DelegatingSubject类实现Subject接口
public class DelegatingSubject implements Subject {
  ...
  protected transient SecurityManager securityManager;
  ...
  public void login(AuthenticationToken token) throws AuthenticationException {
    clearRunAsIdentitiesInternal();
    Subject subject = securityManager.login(this, token);//调用securityManager.login方法

    PrincipalCollection principals;
    ...
  }
}
```

其中，`SecurityManager`接口的`login`方法是由`DefalutSecurityManager`类实现的，因此它在`login`方法中能调用上面图片中的所有继承类的方法来完成登录认证，这也就解释了为什么` DelegatingSubject`类不直接声明`DefalutSecurityManager`类的变量，而是声明`SecurityManager`类的变量。

再看`login`方法，在`DefalutSecurityManager`类中的`login`方法中，有一个身份认证方法`authenticate(AuthenticationToken token)`，如下代码

```java
public class DefaultSecurityManager extends SessionsSecurityManager {
  ...
  ...
  public Subject login(Subject subject, AuthenticationToken token) throws AuthenticationException {
    AuthenticationInfo info;
    try {
      info = authenticate(token);
    } catch (AuthenticationException ae) {
      try {
        onFailedLogin(token, ae, subject);
      }
  ...
}
```

该方法实际上是由接口`Authenticator`（所称为身份认证器）声明，如下代码

```java
public interface Authenticator {
   public AuthenticationInfo authenticate(AuthenticationToken authenticationToken)
            throws AuthenticationException;
}
```

由此可看出，从最开始，`UsernamePasswordToken ` 的用户身份信息传进来后，会先经过`SecurityManager` ，再由`SecurityManager`与`Authenticator`身份认证这一部分进行交互来完成身份验证这一安全操作过程，符合了上图身份认证的过程



### 4、realm域

（`ModularRealmAuthenticator`继承了`AbstractAuthenticator`，`AbstractAuthenticator`实现了`Authenticator`接口，`Authenticator`接口的

`authenticate`方法由`AbstractAuthenticator`实现，而`authenticate`方法中的`doAuthenticate`由`AbstractAuthenticator`定义，`ModularRealmAuthenticator`重写）



#### 前言：

**无论是ini配置文件定义的域，或者shiro自带的域，还是我们自定义的域，都要经过以下过程：**

适配realm的最底层使用了上面说到的身份验证的方法`authenticate(AuthenticationToken authenticationToken)`，顺着上面的方法继续往下延伸，该方法的实现中调用了`doAuthenticate`方法

```java
public AuthenticationInfo authenticate(AuthenticationToken authenticationToken) throws AuthenticationException{
    ...
    //authenticate方法的实现
    AuthenticationInfo info;
    try {
      info = doAuthenticate(token);//此处调用方法
      if (info == null) {
        String msg = "No account information found for authentication token [" + token + "] by this " +
          "Authenticator instance.  Please check that it is configured correctly.";
        throw new AuthenticationException(msg);
      }
    } 
}
```

`ModularRealmAuthenticator`类（一般常用的验证器类）中重写了方法`doAuthenticate`，如下代码：

```java
//doAuthenticate方法
protected AuthenticationInfo doAuthenticate(AuthenticationToken authenticationToken) throws AuthenticationException {
    assertRealmsConfigured();
    Collection<Realm> realms = getRealms();//获取realm
    if (realms.size() == 1) {
      return doSingleRealmAuthentication(realms.iterator().next(), authenticationToken);
    } else {
      return doMultiRealmAuthentication(realms, authenticationToken);
    }
}
```

假如realm是自定义的，可通过spring的配置文件中以bean的形式注入，在`ModularRealmAuthenticator`类被set进去，因此直接用`getRealms()`获取realm即可。

该方法定义中调用了两个方法：

`doSingleRealmAuthentication`和`doMultiRealmAuthentication`，也即处理单个域身份认证和处理多个域身份认证方法，在`doMultiRealmAuthentication`中循环了Realm这个类`for (Realm realm : realms)`。如下代码：

```java
//doSingleRealmAuthentication
protected AuthenticationInfo doSingleRealmAuthentication(Realm realm, AuthenticationToken token) {
   if (!realm.supports(token)) {
     String msg = "Realm [" + realm + "] does not support authentication token [" +
       token + "].  Please ensure that the appropriate Realm implementation is " +
       "configured correctly or that the realm accepts AuthenticationTokens of this type.";
     throw new UnsupportedTokenException(msg);
   }
   AuthenticationInfo info = realm.getAuthenticationInfo(token);
   if (info == null) {
     String msg = "Realm [" + realm + "] was unable to find account data for the " +
       "submitted AuthenticationToken [" + token + "].";
     throw new UnknownAccountException(msg);
   }
   return info;
}
```

```java
//doMultiRealmAuthentication
protected AuthenticationInfo doMultiRealmAuthentication(Collection<Realm> realms, AuthenticationToken token) {
  
    ...

    for (Realm realm : realms) {
      	  aggregate = strategy.beforeAttempt(realm, token, aggregate);
      if (realm.supports(token)) {
          ...
          try {
            info = realm.getAuthenticationInfo(token);
          } 
          ...
    }
}
```

两个方法中通过调用`realm.supports(token)`判断是否支持传入的token，若不支持则抛出异常

在两个方法中都有以下代码：`info = realm.getAuthenticationInfo(token);`，`getAuthenticationInfo`（获取用户真实身份信息的方法）代码如下：

```java
 public final AuthenticationInfo getAuthenticationInfo(AuthenticationToken token) throws AuthenticationException {
      AuthenticationInfo info = getCachedAuthenticationInfo(token); 
      if (info == null) {
           //otherwise not cached, perform the lookup:
           info = doGetAuthenticationInfo(token);//使用doGetAuthenticationInfo方法，进行身份验证并获取用户真实信息
           log.debug("Looked up AuthenticationInfo [{}] from doGetAuthenticationInfo", info);
           if (token != null && info != null) {
             cacheAuthenticationInfoIfPossible(token, info);
           }
       } else {
         	log.debug("Using cached authentication info [{}] to perform credentials matching.", info);
       }
   	   if (info != null) {
            assertCredentialsMatch(token, info);
       } else {
            log.debug("No AuthenticationInfo found for submitted AuthenticationToken [{}].  Returning null.", token);
       }

       return info;
 }
```

如果缓存中没有用户的身份信息，则调用`doGetAuthenticationInfo(token)`方法，获取到用户的真实信息，并对用户信息进行校对，如果不抛异常且返回信息不为空，则执行`assertCredentialsMatch(token, info)`方法进行二次校验（密码）：

```java
protected void assertCredentialsMatch(AuthenticationToken token, AuthenticationInfo info) throws AuthenticationException {
        CredentialsMatcher cm = getCredentialsMatcher();//获取文凭适配器，在AuthenticatingRealm类内已经set好
        if (cm != null) {
          	//如果比较不正确，则抛出异常
            if (!cm.doCredentialsMatch(token, info)) {
                //not successful - throw an exception to indicate this:
                String msg = "Submitted credentials for token [" + token + "] did not match the expected credentials.";
                throw new IncorrectCredentialsException(msg);
            }
        } else {
            throw new AuthenticationException("A CredentialsMatcher must be configured in order to verify " +
                    "credentials during authentication.  If you do not wish for credentials to be examined, you " +
                    "can configure an " + AllowAllCredentialsMatcher.class.getName() + " instance.");
        }
    }
```

其中，执行`doCredentialsMatch`方法判断用户的真实信息（从realm中获取）与用户输入的信息（从java代码或者网页提交）是否吻合，代码如下：

```java
public boolean doCredentialsMatch(AuthenticationToken token, AuthenticationInfo info) {
        Object tokenCredentials = getCredentials(token);//从用户输入的token中获取密码
        Object accountCredentials = getCredentials(info);//从用户的真实信息info中获取密码
        return equals(tokenCredentials, accountCredentials);//进行比较
    }
```



返回到`login`方法，抛异常执行`onFailedLogin`方法，无异常执行`createSubject(token, info, subject)`与`onSuccessfulLogin(token, info, loggedIn)`方法

```java
public Subject login(Subject subject, AuthenticationToken token) throws AuthenticationException {
		AuthenticationInfo info;
        try {
            info = authenticate(token);
        } catch (AuthenticationException ae) {
            try {
                onFailedLogin(token, ae, subject);
            } catch (Exception e) {
                if (log.isInfoEnabled()) {
                    log.info("onFailedLogin method threw an " +
                            "exception.  Logging and propagating original AuthenticationException.", e);
                }
            }
            throw ae; //propagate
        }
        Subject loggedIn = createSubject(token, info, subject);
        onSuccessfulLogin(token, info, loggedIn);
        return loggedIn;
    }
```



接下来分析一下ini配置的realm、自定义配置的realm、shiro自带的realm是如何加工以到达上面那一步并执行`doGetAuthenticationInfo(token)`方法的：



#### a、ini配置的realm

```java
public static void main(String[] args) {

		//读取配置文件，初始化SecurityManager工厂
		Factory<SecurityManager> factory = new IniSecurityManagerFactory("classpath:jdbc_realm");
		//获取实例
		SecurityManager securityManager = factory.getInstance();
		//将securityManager绑定到SecurityUtils
		SecurityUtils.setSecurityManager(securityManager);
		//得到当前执行的用户
		Subject currentUser = SecurityUtils.getSubject();
		//创建token令牌，用户名/密码
		UsernamePasswordToken token = new UsernamePasswordToken("jack", "123456");
		//身份认证
		try {
			currentUser.login(token);
			System.out.println("身份认证成功");
		}catch (AuthenticationException e){
			e.printStackTrace();
			System.out.println("身份认证失败，请重新输入密码");
		}
		//退出登录
		currentUser.logout();
	}
```

如上代码是设置ini配置文件后用于用户登录验证身份的，最初的时候读取了ini配置文件并初始化工厂，进入`IniSecurityManagerFactory`类的构造方法，有如下代码：

```java
public IniSecurityManagerFactory(String iniResourcePath) {
        this(Ini.fromResourcePath(iniResourcePath));
    }
```

可以看到它使用了Ini类的静态方法`fromResourcePath(iniResourcePath)`，查看该方法，有如下代码：

```java
public static Ini fromResourcePath(String resourcePath) throws ConfigurationException {
        if (!StringUtils.hasLength(resourcePath)) {
            throw new IllegalArgumentException("Resource Path argument cannot be null or empty.");
        }
        Ini ini = new Ini();
        ini.loadFromPath(resourcePath);
        return ini;
    }
```

再进入`ini.loadFromPath(resourcePath)`方法：

```java
public void loadFromPath(String resourcePath) throws ConfigurationException {
        InputStream is;
        try {
            is = ResourceUtils.getInputStreamForPath(resourcePath);
        } catch (IOException e) {
            throw new ConfigurationException(e);
        }
        load(is);
    }
```

将ini配置文件变成输入流后又调用`load(is)`方法，`load(is)`代码如下：

```java
public void load(InputStream is) throws ConfigurationException {
        if (is == null) {
            throw new NullPointerException("InputStream argument cannot be null.");
        }
        InputStreamReader isr;
        try {
          	//DEFAULT_CHARSET_NAME == UTF-8
            isr = new InputStreamReader(is, DEFAULT_CHARSET_NAME);
        } catch (UnsupportedEncodingException e) {
            throw new ConfigurationException(e);
        }
        load(isr);
    }
```

根据上面的输入流创建`InputStreamReader`类对象并将编码设为UTF-8后又调用`load(isr)`方法：

```java
public void load(Reader reader) {
        Scanner scanner = new Scanner(reader);
        try {
            load(scanner);
        } finally {
            try {
                scanner.close();
            } catch (Exception e) {
                log.debug("Unable to cleanly close the InputStream scanner.  Non-critical - ignoring.", e);
            }
        }
    }
```

在此方法中，创建了shiro自带的扫描器类`Scanner`，并调用`load(scanner)`。从该方法开始，ini配置文件中的信息将会被转换。



在`load(Scanner scanner)`方法中，通过scanner一行一行地扫描ini配置文件内的信息。在Ini类中声明了一个变量sections

```java
public class Ini implements Map<String, Ini.Section> {
  ...	
  private final Map<String, Section> sections;
  ...
  public void load(Scanner scanner) {

        String sectionName = DEFAULT_SECTION_NAME;
        StringBuilder sectionContent = new StringBuilder();

        while (scanner.hasNextLine()) {

            String rawLine = scanner.nextLine();
            String line = StringUtils.clean(rawLine);

            if (line == null || line.startsWith(COMMENT_POUND) || line.startsWith(COMMENT_SEMICOLON)) {
                //skip empty lines and comments:
                continue;
            }

            String newSectionName = getSectionName(line);
            if (newSectionName != null) {
                //found a new section - convert the currently buffered one into a Section object
                addSection(sectionName, sectionContent);

                //reset the buffer for the new section:
                sectionContent = new StringBuilder();

                sectionName = newSectionName;

                if (log.isDebugEnabled()) {
                    log.debug("Parsing " + SECTION_PREFIX + sectionName + SECTION_SUFFIX);
                }
            } else {
                //normal line - add it to the existing content buffer:
                sectionContent.append(rawLine).append("\n");
            }
        }

        //finish any remaining buffered content:
        addSection(sectionName, sectionContent);
    }
  ...
}
```

在load方法中，声明了sectionName和sectionContent变量，分别代表该节的名称及内容，例如sectionName为`users`，sectionContent为`jack=123`。随后调用了`void addSection(String name, StringBuilder content)`方法，在内创建一个新的section，而在`Section`类的构造方法中，它将sectionContent分为了=前的key以及=号后的value，并将其设置进一个`LinkedHashMap<String,String>`中，因此=号前与=号后的关系为`key-value`关系，如下代码：

```java
public static class Section implements Map<String, String> {
      ...
	  private final Map<String, String> props;
  	  ...
      private Section(String name, String sectionContent) {
          if (name == null) {
            throw new NullPointerException("name");
          }
          this.name = name;
          Map<String,String> props;//创建Map
          if (StringUtils.hasText(sectionContent) ) {
            props = toMapProps(sectionContent);//调用自定义方法将=号前与=号后的关系设置进map并返回map
          } else {
            props = new LinkedHashMap<String,String>();
          }
          if ( props != null ) {
            this.props = props;
          } else {
            this.props = new LinkedHashMap<String,String>();
          }
      }
      ...
}
```

在`addSection`方法中，new完Section对象后，会将sectionName以及new出来的Section放置进Ini声明的映射变量`Map<String, Section> sections`中

```java
private void addSection(String name, StringBuilder content) {
  if (content.length() > 0) {
    String contentString = content.toString();
    String cleaned = StringUtils.clean(contentString);
    if (cleaned != null) {
      Section section = new Section(name, contentString);//创建新的Section对象
      if (!section.isEmpty()) {
        sections.put(name, section);//将SectionName与section放置进sections中
      }
    }
  }
}
```

**注：**`Ini`类中有`Map<String, Section> sections`变量，String代表sectionName，Section类继承了Map且内部有`Map<String ,String>`类型变量，表示=前（资源）与=后（权限）的`key-value`关系。`sections`可看作`Map<String, Map<String ,String>>`类型，**`ShiroFilterFactoryBean`类中`setFilterChainDefinitions`方法获得sectionName为urls的section后将对应关系set进`FilterChainDefinitionMap`中。**

调用完`addSection`方法后，ini配置文件中的信息就已经存到了`Ini`类中定义的`Map<String, Section> sections`变量中。回到`IniSecurityManagerFactory`类的构造方法

```java
public IniSecurityManagerFactory(Ini config) {
        setIni(config);
    }
```

调用了`setIni(config)`方法，将配置的ini文件set进`IniFactorySupport`类；回到最初的代码，执行下一行：

```java
SecurityManager securityManager = factory.getInstance();//获取securityManager
```

查看`factory.getInstance()`方法，最终追踪到了`IniFactorySupport`类的`createInstance`方法中，如下代码：

```java
 public T createInstance() {
        Ini ini = resolveIni();

        T instance;

        if (CollectionUtils.isEmpty(ini)) {
            log.debug("No populated Ini available.  Creating a default instance.");
            instance = createDefaultInstance();
            if (instance == null) {
                String msg = getClass().getName() + " implementation did not return a default instance in " +
                        "the event of a null/empty Ini configuration.  This is required to support the " +
                        "Factory interface.  Please check your implementation.";
                throw new IllegalStateException(msg);
            }
        } else {
            log.debug("Creating instance from Ini [" + ini + "]");
            instance = createInstance(ini);
            if (instance == null) {
                String msg = getClass().getName() + " implementation did not return a constructed instance from " +
                        "the createInstance(Ini) method implementation.";
                throw new IllegalStateException(msg);
            }
        }

        return instance;
    }
```

由于上面说到加工完的ini文件已经set进`IniFactorySupport`类中，所以此处执行`instance = createInstance(ini);`，查看由`IniSecurityManagerFactory`类重写的`createInstance(ini)`方法，最后跟踪到`IniSecurityManagerFactory`类中的`createSecurityManager`方法：

```java
private SecurityManager createSecurityManager(Ini ini, Ini.Section mainSection) {

        Map<String, ?> defaults = createDefaults(ini, mainSection);
        Map<String, ?> objects = buildInstances(mainSection, defaults);

        SecurityManager securityManager = getSecurityManagerBean();

        boolean autoApplyRealms = isAutoApplyRealms(securityManager);

        if (autoApplyRealms) {
            //realms and realm factory might have been created - pull them out first so we can
            //initialize the securityManager:
            Collection<Realm> realms = getRealms(objects);
            //set them on the SecurityManager
            if (!CollectionUtils.isEmpty(realms)) {
                applyRealmsToSecurityManager(realms, securityManager);
            }
        }

        return securityManager;
    }
```

执行第一行代码，返回的是一个key为字符串“iniRealm”，value为一个`IniRealm`类的对象的Map对象；执行第二行代码，`ReflectionBuilder`类的`buildInstances(mainSection, defaults)`代码方法如下：

```java
private Map<String, ?> buildInstances(Ini.Section section, Map<String, ?> defaults) {
        this.builder = new ReflectionBuilder(defaults);
        return this.builder.buildObjects(section);
    }
```

第一行代码中的构造方法中，将`ReflectionBuilder`类的变量`Map<String, ?> objects`设为defaults，随后在`buildObjects(section)`方法中，执行了`LifecycleUtils.init(objects.values())`方法，传入的参数是刚才设入的`IniRealm`类的对象（defaults的“iniRealm”的value值），追踪init方法，最后来到了`IniRealm`类的`onInit`方法，而此方法中调用了另一个方法`processDefinitions(ini)`，代码如下：

```java
private void processDefinitions(Ini ini) {
        if (CollectionUtils.isEmpty(ini)) {
            log.warn("{} defined, but the ini instance is null or empty.", getClass().getSimpleName());
            return;
        }

        Ini.Section rolesSection = ini.getSection(ROLES_SECTION_NAME);
        if (!CollectionUtils.isEmpty(rolesSection)) {
            log.debug("Discovered the [{}] section.  Processing...", ROLES_SECTION_NAME);
            processRoleDefinitions(rolesSection);
        }

        Ini.Section usersSection = ini.getSection(USERS_SECTION_NAME);
        if (!CollectionUtils.isEmpty(usersSection)) {
            log.debug("Discovered the [{}] section.  Processing...", USERS_SECTION_NAME);
            processUserDefinitions(usersSection);
        } else {
            log.info("{} defined, but there is no [{}] section defined.  This realm will not be populated with any " +
                    "users and it is assumed that they will be populated programatically.  Users must be defined " +
                    "for this Realm instance to be useful.", getClass().getSimpleName(), USERS_SECTION_NAME);
        }
    }
```

通过`getSection`获取到roles和users相关的section，然后分别执行`processRoleDefinitions(rolesSection)`与`processUserDefinitions(usersSection)`方法：



```java
protected void processRoleDefinitions(Map<String, String> roleDefs) {
        if (roleDefs == null || roleDefs.isEmpty()) {
            return;
        }
        for (String rolename : roleDefs.keySet()) {
          	//获取权限
            String value = roleDefs.get(rolename);

            SimpleRole role = getRole(rolename);
            if (role == null) {
                role = new SimpleRole(rolename);
                add(role);
            }
			//将权限设进Set<Permission> permissions中
            Set<Permission> permissions = PermissionUtils.resolveDelimitedPermissions(value, getPermissionResolver());
            role.setPermissions(permissions);
        }
    }
```

```java
 protected void processUserDefinitions() throws ParseException {
        String userDefinitions = getUserDefinitions();
        if (userDefinitions == null) {
            return;
        }

        Map<String, String> userDefs = toMap(toLines(userDefinitions));

        processUserDefinitions(userDefs);
    }

    protected void processUserDefinitions(Map<String, String> userDefs) {
        if (userDefs == null || userDefs.isEmpty()) {
            return;
        }
        for (String username : userDefs.keySet()) {

            String value = userDefs.get(username);

            String[] passwordAndRolesArray = StringUtils.split(value);

            String password = passwordAndRolesArray[0];

            SimpleAccount account = getUser(username);
            if (account == null) {
                account = new SimpleAccount(username, password, getName());
                add(account);
            }
            account.setCredentials(password);

            if (passwordAndRolesArray.length > 1) {
                for (int i = 1; i < passwordAndRolesArray.length; i++) {
                    String rolename = passwordAndRolesArray[i];
                    account.addRole(rolename);

                    SimpleRole role = getRole(rolename);
                    if (role != null) {
                        account.addObjectPermissions(role.getPermissions());
                    }
                }
            } else {
                account.setRoles(null);
            }
        }
    }
```

注意两段代码中的`add(role);`和`add(account);`

```java
 public void addRole(String name) {
        add(new SimpleRole(name));
    }

    protected void add(SimpleRole role) {
        ROLES_LOCK.writeLock().lock();
        try {
            roles.put(role.getName(), role);
        } finally {
            ROLES_LOCK.writeLock().unlock();
        }
    }
```

```java
protected void add(SimpleAccount account) {
        String username = getUsername(account);
        USERS_LOCK.writeLock().lock();
        try {
            this.users.put(username, account);
        } finally {
            USERS_LOCK.writeLock().unlock();
        }
    }
```

其实是分别将username和rolename分别设入`SimpleAccountRealm`类中的变量`Map<String, SimpleAccount> users`和`Map<String, SimpleRole> roles`。至此，ini配置文件的信息将都被分好并设置进对应类的变量中。回到`AbstractFactory`类的`getInstance`方法，获取到`securityManager`实例。



接着执行`Subject currentUser = SecurityUtils.getSubject();`，内部通过调用`SecurityManager`接口的`createSubject(SubjectContext context)`方法获得subject，然后获取令牌`usernamePasswordToken`，执行`login`方法，如上述所说，到达`doGetAuthenticationInfo`方法。虽然`IniRealm`类没有重写`doGetAuthenticationInfo`方法，但它继承于`SimpleAccountRealm`类，`SimpleAccountRealm`类重写了`doGetAuthenticationInfo`方法，代码如下：

```java
protected AuthenticationInfo doGetAuthenticationInfo(AuthenticationToken token) throws AuthenticationException {
        UsernamePasswordToken upToken = (UsernamePasswordToken) token;
        SimpleAccount account = getUser(upToken.getUsername());

        if (account != null) {
	
            if (account.isLocked()) {
                throw new LockedAccountException("Account [" + account + "] is locked.");
            }
            if (account.isCredentialsExpired()) {
                String msg = "The credentials for account [" + account + "] are expired";
                throw new ExpiredCredentialsException(msg);
            }

        }

        return account;
    }
```



#### b、shiro自带的realm

shiro自带的realm均重写了`doGetAuthenticationInfo`方法或继承了其他realm类来调用`doGetAuthenticationInfo`方法，此处不再进行展开，下面是`JdbcRealm`类的`doGetAuthenticationInfo`方法：

```java
protected AuthenticationInfo doGetAuthenticationInfo(AuthenticationToken token) throws AuthenticationException {
        UsernamePasswordToken upToken = (UsernamePasswordToken) token;
        String username = upToken.getUsername();
        // Null username is invalid
        if (username == null) {
            throw new AccountException("Null usernames are not allowed by this realm.");
        }
        Connection conn = null;
        SimpleAuthenticationInfo info = null;
        try {
            conn = dataSource.getConnection();
            String password = null;
            String salt = null;
            switch (saltStyle) {
            case NO_SALT:
                password = getPasswordForUser(conn, username)[0];
                break;
            case CRYPT:
                // TODO: separate password and hash from getPasswordForUser[0]
                throw new ConfigurationException("Not implemented yet");
                //break;
            case COLUMN:
                String[] queryResults = getPasswordForUser(conn, username);
                password = queryResults[0];
                salt = queryResults[1];
                break;
            case EXTERNAL:
                password = getPasswordForUser(conn, username)[0];
                salt = getSaltForUser(username);
            }
            if (password == null) {
                throw new UnknownAccountException("No account found for user [" + username + "]");
            }
            info = new SimpleAuthenticationInfo(username, password.toCharArray(), getName());
           
            if (salt != null) {
                info.setCredentialsSalt(ByteSource.Util.bytes(salt));
            }

        } catch (SQLException e) {
            final String message = "There was a SQL error while authenticating user [" + username + "]";
            if (log.isErrorEnabled()) {
                log.error(message, e);
            }

            // Rethrow any SQL errors as an authentication exception
            throw new AuthenticationException(message, e);
        } finally {
            JdbcUtils.closeConnection(conn);
        }

        return info;
    }
```

#### c、自定义的realm

自定义的realm需要继承`AuthorizingRealm`（权限域类）（`AuthorizingRealm`（权限域）类继承了`AuthenticatingRealm`（身份认证域）类），重写`doGetAuthorizationInfo`和`doGetAuthenticationInfo`方法，以达到上述目的，一般我们自定义的realm都会从数据库获取信息来得到用户的真实身份和权限信息，如下demo：

```java
public class MyRealm extends AuthorizingRealm{

	@Resource
	private UserService userService;

	/**
	 * 为当前用户授予角色和权限
	 * @param principalCollection principalCollection身份
	 * @return AuthorizationInfo 授权信息
	 */
	@Override
	protected AuthorizationInfo doGetAuthorizationInfo(PrincipalCollection principalCollection) {
		String userName = (String) principalCollection.getPrimaryPrincipal();
		SimpleAuthorizationInfo simpleAuthorizationInfo = new SimpleAuthorizationInfo();
		simpleAuthorizationInfo.setRoles(userService.getRoles(userName));
		simpleAuthorizationInfo.setStringPermissions(userService.getPermissions(userName));
		return simpleAuthorizationInfo;
	}

	/**
	 * 验证当前登录的用户
	 * @param authenticationToken authenticationToken
	 * @return AuthenticationInfo
	 * @throws AuthenticationException AuthenticationException
	 */
	@Override
	protected AuthenticationInfo doGetAuthenticationInfo(AuthenticationToken authenticationToken) throws AuthenticationException {
		System.out.println("MyRealm1");
		String userName = (String) authenticationToken.getPrincipal();
		User user = userService.getUser(userName);

		if(user != null){
			AuthenticationInfo authenticationInfo = new SimpleAuthenticationInfo(user.getUserName() ,user.getPassword()
					,"xx");
			return authenticationInfo;
		}else{
			return null;
		}
	}
}
```

总结：三类不同的realm在身份认证中最终都会调用`doGetAuthenticationInfo`方法来获取`AuthenticationInfo`信息，只是彼此间的实现路径有所不同，同时也符合了`Subject -> SecurityManager -> Authorizer -> realm`这一流程。



#### 关于多个realm

有时要进行多次身份认证和权限认证，我们可以定义多个realm，当需要定义多个realm时，需要先设置策略，实现类`ModularRealmAuthenticator`中有一个认证策略接口，认证策略分为以下三种：

![img](http://images2015.cnblogs.com/blog/702434/201703/702434-20170325090456846-1697870464.png)

![img](http://images2015.cnblogs.com/blog/702434/201703/702434-20170325090526752-325109313.png)

`ModularRealmAuthenticator`类默认为`AtLeastOneSuccessfulStrategy`策略

```java
public ModularRealmAuthenticator() {
    this.authenticationStrategy = new AtLeastOneSuccessfulStrategy();
}
```

或者在spring配置文件中以bean形式注入



多个realm需要在spring配置文件中有如下配置：

```xml
<!-- 配置自定义域 -->
<bean id="myRealm" class="shiroWeb.realm.MyRealm" />
<bean id="myRealm2" class="shiroWeb.realm.MyRealm2" />

<!-- 配置策略：AtleastOneSuccessfulStrategy、 AllSuccessfulStrategy、 FirstSuccessfulStrategy
	 也可自定义策略，然后将class改为自定义策略类
-->
<bean id="authcStrategy" class="org.apache.shiro.authc.pam.AllSuccessfulStrategy" />

<!--配置验证器-->
<bean id="authenticator" class="org.apache.shiro.authc.pam.ModularRealmAuthenticator">
  <!-- 将策略配置给验证器 -->
  <property name="authenticationStrategy" ref="authcStrategy" />
  <!-- 将自定义域配置给验证器 -->
  <property name="realms">
    <list>
      <ref bean="myRealm" />
      <ref bean="myRealm2"/>
    </list>
  </property>
</bean>

<!-- 配置安全管理器 -->
<bean id="securityManager" class="org.apache.shiro.web.mgt.DefaultWebSecurityManager">
  <!-- Single realm app.  If you have multiple realms, use the 'realms' property instead. -->
  <!-- 配置自定义域，验证的顺序按照配置顺序为准 -->
  <property name="realms">
    <list>
      <ref bean="myRealm2" />
      <ref bean="myRealm" />
    </list>
  </property>
  <!-- 将验证器配给安全管理器 -->
  <property name="authenticator" ref="authenticator" />
  <!-- By default the servlet container sessions will be used.  Uncomment this line
             to use shiro's native sessions (see the JavaDoc for more): -->
  <!-- <property name="sessionMode" value="native"/> -->
</bean>
```

按照以上配置，用户进行身份认证或权限认证时则会先经过MyRealm2，再经过MyRealm



## 二、权限认证

### 1、流程

![](http://ot0aou666.bkt.clouddn.com/shiro%E6%9D%83%E9%99%90%E8%AE%A4%E8%AF%81.png)



Shiro为我们定义了一个抽象的权限描述字串： 
【资源】：【操作】：【实例】

例如：”user:create,update”,”user:delete:100110”，等等。

权限认证流程与身份认证流程类似，以`subject.isPermitted(String permission)`方法为例。`SecurityManager`继承了`Authorizer`接口，由`DelegatingSubject`类实现`isPermitted(String permission)`方法，如下代码：

```java
public boolean isPermitted(String permission) {
        return hasPrincipals() && securityManager.isPermitted(getPrincipals(), permission);
}
```

而继承于`Authorizer`的`SecurityManager`类的`isPermitted(String permission)`方法由`AuthorizingRealm`实现。所以当调用了`isPermitted(String permission)`方法时，会通过`SecurityManager`调用`Authorizer`接口定义、`AuthorizingRealm`实现的`isPermitted(String permission)`方法，也符合了

`Subject -> SecurityManager -> Authorizer -> realm`这一流程。



分析`securityManager.isPermitted(getPrincipals(), permission)`这个方法，最终来到`AuthorizingRealm`类的`isPermitted`方法：

```java
public boolean isPermitted(PrincipalCollection principals, String permission) {
        Permission p = getPermissionResolver().resolvePermission(permission);//此方法内部构造了WildcardPermission类对象并将permission分解为一部分一部分
        return isPermitted(principals, p);
    }

public boolean isPermitted(PrincipalCollection principals, Permission permission) {
        AuthorizationInfo info = getAuthorizationInfo(principals);//从realm中获取用户的权限信息
        return isPermitted(permission, info);
	}

private boolean isPermitted(Permission permission, AuthorizationInfo info) {
        Collection<Permission> perms = getPermissions(info);//通过用户权限信息获取permission的集合
        if (perms != null && !perms.isEmpty()) {
            for (Permission perm : perms) {
                //将用户所拥有的真实权限perm与要验证的权限permission作比较
              	if (perm.implies(permission)) {
                    return true;
                }
            }
        }
        return false;
    }
```

执行`Permission p = getPermissionResolver().resolvePermission(permission);`代码，将权限分割开，例如`user：create，delete`其会分割为

`{user},{create},{delete}`，如下：

```java
public WildcardPermission(String wildcardString) {
        this(wildcardString, DEFAULT_CASE_SENSITIVE);
    }

public WildcardPermission(String wildcardString, boolean caseSensitive) {
  		setParts(wildcardString, caseSensitive);
}

protected void setParts(String wildcardString, boolean caseSensitive) {
        if (wildcardString == null || wildcardString.trim().length() == 0) {
            throw new IllegalArgumentException("Wildcard string cannot be null or empty. Make sure permission strings are properly formatted.");
        }

        wildcardString = wildcardString.trim();

        List<String> parts = CollectionUtils.asList(wildcardString.split(PART_DIVIDER_TOKEN));//分割冒号

        this.parts = new ArrayList<Set<String>>();
        for (String part : parts) {
            Set<String> subparts = CollectionUtils.asSet(part.split(SUBPART_DIVIDER_TOKEN));//分割逗号
            if (!caseSensitive) {
                subparts = lowercase(subparts);
            }
            if (subparts.isEmpty()) {
                throw new IllegalArgumentException("Wildcard string cannot contain parts with only dividers. Make sure permission strings are properly formatted.");
            }
            this.parts.add(subparts);//将分割好的集合add进List<Set<String>>变量中
        }

        if (this.parts.isEmpty()) {
            throw new IllegalArgumentException("Wildcard string cannot contain only dividers. Make sure permission strings are properly formatted.");
        }
    }
```

调用其他realm（自定义realm等）重写的`doGetAuthorizationInfo`方法获取权限信息（与身份认证大同小异）

```java
public final AuthorizationInfo getAuthorizationInfo(AuthenticationToken token) throws AuthenticationException {	
    AuthorizationInfo info = null;
    ...
    if (info == null) {
      // Call template method if the info was not found in a cache
      info = doGetAuthorizationInfo(principals);
      // If the info is not null and the cache has been created, then cache the authorization info.
      if (info != null && cache != null) {
        if (log.isTraceEnabled()) {
          log.trace("Caching authorization info for principals: [" + principals + "].");
        }
        Object key = getAuthorizationCacheKey(principals);
        cache.put(key, info);
      }
}	
```

获取完后，调用第三个`isPermitted`方法进行匹配，用户所拥有的真实权限perm与要验证的权限permission作比较的方法`perm.implies(permission`如下：

```java
/**
	(1)授权（user），校验（user），结果（通过）
	(2)授权（user），校验（user:create），结果（通过）
	(3)授权（user:create），校验（user:update），结果（不通过）
	(4)授权（user:create），校验（user:create,update），结果（不通过）
	(5)授权（user:create,update），校验（user:update），结果（通过）
	(6)授权（user:create），校验（user:*），结果（不通过）
	(7)授权（user:*），校验（user:create），结果（通过）
**/

public boolean implies(Permission p) {
        // By default only supports comparisons with other WildcardPermissions
  		// 只支持与另一个WildcardPermission作比较
        if (!(p instanceof WildcardPermission)) {
            return false;
        }

        WildcardPermission wp = (WildcardPermission) p;

        List<Set<String>> otherParts = wp.getParts();//用户真实权限的part

        int i = 0;
        for (Set<String> otherPart : otherParts) {
            // If this permission has less parts than the other permission, everything after the number of parts contained
            // in this permission is automatically implied, so return true
          	// 如果当前permission长度比other permission短，校验通过
            // 参见示例（2）
            if (getParts().size() - 1 < i) {
                return true;
            } else {
                Set<String> part = getParts().get(i);//传入的permission的part
              	//如果当前permission的当前Part不是*，
                //且又不能包住other permission的当前Part，则校验不通过
                //否则继续，直到处理完当前permission的所有节
                // 参见示例（3）（4）
                if (!part.contains(WILDCARD_TOKEN) && !part.containsAll(otherPart)) {
                    return false;
                }
                i++;
            }
        }

        // If this permission has more parts than the other parts, only imply it if all of the other parts are wildcards
  		// 上面逻辑没有返回false,开始检查当前permission多出来的part
        // 如果多出来的part只要有一个不为*，则不通过。
        // 参见示例（7）
        for (; i < getParts().size(); i++) {
            Set<String> part = getParts().get(i);
            if (!part.contains(WILDCARD_TOKEN)) {
                return false;
            }
        }

        return true;
    }
```

**小结：身份认证与权限认证的流程大致为此，上述身份认证以login方法为例，权限认证以isPermitted方法为例，其他方法的实现可能会有点不同，但可谓是大同小异。可以归纳为从realm中获取用户的真实信息，然后来和前端输入的用户信息作比较，达到确认身份、控制权限的目的。**



### 2、编程式定义权限

(角色权限设置)

```ini
[users]
jack=123456,role1,role2
john=123456,role1
[roles]
role1=user:select
role2=user:add ,user:update ,user:delete
```

(测试用户是否有该角色)

```java
/**
 * Created by Garson in 22:26 2017/9/4
 * Description : 判断该用户是否有此角色，有has和check两种方法，has返回布尔值，check会抛异常
 */
public class RoleTest {

	@Test
	public void testHasRole(){
		Subject currentUser = ShiroUtil.login("classpath:shiro_role" ,"jack" ,"123456");
		//Subject currentUser = ShiroUtil.login("classpath:shiro_role" ,"jack" ,"12345678");
		System.out.println(currentUser.hasRole("role1")?"有role1这个角色":"没有role1这个角色");
		boolean[] results = currentUser.hasRoles(Arrays.asList("role1" ,"role2"));
		System.out.println(results[0]?"有role1这个角色":"没有role1这个角色");
		System.out.println(results[1]?"有role2这个角色":"没有role2这个角色");
		System.out.println(currentUser.hasAllRoles(Arrays.asList("role1","role2"))?"role1 role2两个角色都有":"两个角色不全有");
	}

	@Test
	public void testCheckRole(){
		Subject currentUser = ShiroUtil.login("classpath:shiro_role" ,"jack" ,"123456");
		//Subject currentUser = ShiroUtil.login("classpath:shiro_role" ,"jack" ,"12345678");
		currentUser.checkRole("role3");
		currentUser.checkRoles(Arrays.asList("role1" ,"role2"));
		currentUser.checkRoles("role1" ,"role2");
	}

}
```

(测试用户是否有该权限)

```java
/**
 * Created by Garson in 13:27 2017/9/5
 * Description : 判断该用户的角色是否含有特定的权限，有has和check两种方法，has返回布尔值，check会抛异常
 */
public class PermissionTest {

	@Test
	public void hasPermissionTest(){
		Subject currentUser = ShiroUtil.login("classpath:shiro_permission" ,"jack" ,"123456");
		//Subject currentUser = ShiroUtil.login("classpath:shiro_role" ,"jack" ,"12345678");
		System.out.println(currentUser.isPermitted("user:select")?"有select权限":"没有select权限");
		System.out.println(currentUser.isPermitted("user:update")?"有update权限":"没有update权限");
		boolean[] results = currentUser.isPermitted("user:select" ,"user:update" ,"user:delete");
		System.out.println(results[0]?"有select权限":"没有select权限");
		System.out.println(results[1]?"有update权限":"没有update权限");
		System.out.println(results[2]?"有delete权限":"没有delete权限");
		System.out.println(currentUser.isPermittedAll("user:select" ,"user:update"));
		currentUser.logout();
	}

	@Test
	public void checkPermissionTest(){
		Subject currentUser = ShiroUtil.login("classpath:shiro_permission" ,"john" ,"123456");
		//Subject currentUser = ShiroUtil.login("classpath:shiro_role" ,"jack" ,"12345678");
		currentUser.checkPermissions("user:select" ,"user:update" ,"user:delete");
		currentUser.logout();
	}

}
```

### 3、注解式定义权限

(1)`@RequiresAuthentication`

验证用户是否登录，等同于方法subject.isAuthenticated() 结果为true时，即注解要求在访问或调用被注解的类/实例/方法时，Subject在当前的session中已经被验证

```java
@RequiresAuthentication

public void updateAccount(Account userAccount) {

    //this method will only be invoked by a

    //Subject that is guaranteed authenticated

    ...

}
```

(2)`@RequiresUser`

验证用户是否被记忆，即在访问或调用被注解的类/实例/方法时，他们被认证或者在被前一个Session记住，user有两种含义：

一种是成功登录的（subject.isAuthenticated() 结果为true）；

另外一种是被记忆的（subject.isRemembered()结果为true）。

```java
@RequiresUser

public void updateAccount(Account userAccount) {

    //this method will only be invoked by a

    //Subject that is guaranteed authenticated

    ...

}
```



(3)`@RequiresGuest`

验证是否是一个guest的请求，与@RequiresUser完全相反，即在访问或调用被注解的类/实例/方法时，他们没有被认证或者在被前一个Session记住

 换言之，RequiresUser == !RequiresGuest。

此时subject.getPrincipal()结果为null.

```java
@RequiresGuest

public void signUp(User newUser) {

    //this method will only be invoked by a

    //Subject that is unknown/anonymous

    ...

}
```

(4)`@RequiresRoles`

例如：@RequiresRoles("aRoleName");

 	   void someMethod();

如果subject中有aRoleName角色才可以访问方法someMethod。如果没有这个权限则会抛出异常`AuthorizationException`

(5)`@RequiresPermissions`

例如： @RequiresPermissions({"file:read", "write:aFile.txt"} )
  void someMethod();

要求subject中必须同时含有file:read和write:aFile.txt的权限才能执行方法someMethod()。否则抛出异常`AuthorizationException`

```java
@RequiresPermissions("account:create")

public void createAccount(Account account) {

    //this method will only be invoked by a Subject

    //that is permitted to create an account

    ...

}

@RequiresRoles("administrator")

public void deleteUser(User user) {

    //this method will only be invoked by an administrator

    ...

}
```





## 三、用ini配置文件集成web

### 1、shiro.ini基本配置

以下为shiro.ini

```ini
[main]
#身份认证 未认证通过跳转页面
authc.loginUrl=/login
#角色认证 未认证通过跳转页面
roles.unauthorizedUrl=/unauthorized.jsp
#权限认证 未认证通过跳转页面
perms.unauthorizedUrl=/unauthorized.jsp
#用户
[users]
jack=123456,admin
john=123456,teacher
mary=123456
#角色
[roles]
admin=user:*
teacher=student:*
#设定访问url的权限
[urls]
/login=anon
/admin=authc
#拥有teacher角色或user角色才能访问/student
/student=roles[teacher]
#拥有create这个权限动作才能访问/teacher
/teacher=perms["user:create"]
```



`[main]`中等号左侧为资源，右侧为授权状态，其实质是使用了过滤器，以下为一些过滤器的参考：

| 过滤器  名称 | 功 能                                      | 配 置                                      |
| ------- | :--------------------------------------- | ---------------------------------------- |
| anon    | 任何用户发送的请求都能够访问                           | -                                        |
| authc   | 经过认证的请求可访问，否则将会将请求重定向到 ini 配置文件配置的authc.loginUrl 资源，进行认证操作 | authc.loginUrl=/login.jsp                                                                                                                                                                authc.successUrl=/                                                                                                                                                                                #认证成功后重定向到此资源                                                                                                                             authc.usernameParam=username                                                                                                                                                       #从请求参数中获取key=username的value作为用户名                                                                                      authc.passwordParam=password                                                                                                                                                          #从请求参数中获取key=password的value作为密码                                                                                    authc.rememberMeParam=rememberMe                                                                                                                                               #从请求参数中获取key=rememberMe的value作为是否记住密码的标识 |
| logout  | 结束会话                                     | logout.redirectUrl=/   #结束会话后重定向到此资源     |
| port    | 指定请求端口                                   | port=8080   #8080端口的请求可通过                |

### 2、url匹配方式

+ `？`匹配一个字符 ，例如/admin?则可以匹配/admin1，但不能匹配多个字符或多路径
+ `*`匹配多个字符，例如/admin*则可以匹配/admin123，但不能匹配多路径
+ `/**`匹配多个路径，例如/admin/**则可以匹配/admin/1

### 3、shiro session基础知识

#### a、session

shiro的session不论在javaEE下还是web下都能使用，创建并获取session的信息或将信息保存在session中

```java
Session session = subject.getSession();//创建session
System.out.println("session id: " + session.getId());//获取会话唯一标识
System.out.println("session host:" + session.getHost());//获取当前Subject的主机地址
System.out.println("session Timeout:" + session.getTimeout());//获取当前session过期的时间，默认为30分钟
session.setAttribute("info" ,"session的数据");//设置会话属性
session.removeAttribute("key");//删除会话属性
```

#### b、会话管理器

会话管理器管理着应用中所有Subject的会话的创建、维护、删除、失效、验证等工作。如下为各类会话管理器的继承关系图

![](http://ot0aou666.bkt.clouddn.com/SessionManager%E5%85%B3%E7%B3%BB%E7%BB%A7%E6%89%BF%E5%9B%BE.png)

`SecurityManager`接口提供如下方法：

```java
Session start(SessionContext context); //启动会话  
Session getSession(SessionKey key) throws SessionException; //根据会话Key获取会话
```

用于Web环境的`WebSessionManager`接口又提供了如下方法：

```java
boolean isServletContainerSessions();//是否使用Servlet容器的会话 
```

`ValidatingSessionManager`用于验资并过期会话：

```java
void validateSessions();//验证所有会话是否过期
```

shiro提供了三个默认实现：

* `DefaultSessionManager`：`DefaultSecurityManager`使用的默认实现，用于JavaSE环境
* `ServletContainerSessionManager`：`DefaultWebSecurityManager`使用的默认实现，用于Web环境，其直接使用Servlet容器的会话
* `DefaultWebSessionManager`：用于Web环境的实现，可以替代`ServletContainerSessionManager`，自己维护着会话，直接废弃了Servlet容器的会话管理。

在Servlet容器中，默认使用JSESSIONID Cookie维护会话，且会话默认是跟容器绑定的；在某些情况下可能需要使用自己的会话机制，此时我们可以使用DefaultWebSessionManager来维护会话(以下为spring配置文件)：

```xml
<bean id="sessionIdCookie" class="org.apache.shiro.web.servlet.SimpleCookie">
        <property name="maxAge" value="1800" />
        <property name="httpOnly" value="false" />
  		...
</bean>
<!-- 配置sessionManager -->
<bean id="sessionManager" class="org.apache.shiro.web.session.mgt.DefaultWebSessionManager">
        <property name="globalSessionTimeout" value="1800000" />
        <property name="deleteInvalidSessions" value="false" />
        <property name="sessionIdCookie" ref="sessionIdCookie" />
  		...
</bean>
```

`sessionIdCookie`是`sessionManager`创建会话Cookie的模板：

* `sessionIdCookie.name`：       设置Cookie名字，默认为JSESSIONID；

* `sessionIdCookie.domain`：    设置Cookie的域名，默认空，即当前访问的域名；

* `sessionIdCookie.path`：       设置Cookie的路径，默认空，即存储在域名根下；

* `sessionIdCookie.maxAge`：   设置Cookie的过期时间，秒为单位，默认-1表示关闭浏览器时过期Cookie；

* `sessionIdCookie.httpOnly`：如果设置为true，则客户端不会暴露给客户端脚本代码，使用HttpOnly cookie有助于减少某些类型的跨站点脚本攻击；此特性需                     

  ​                                               要实现了Servlet 2.5 MR6及以上版本的规范的Servlet容器支持；

* `sessionManager.sessionIdCookieEnabled`：是否启用/禁用Session Id Cookie，默认是启用的；如果禁用后将不会设置Session Id Cookie，即默认使用了 

  ​                                                                      Servlet容器的JSESSIONID，且通过URL重写（URL中的“;JSESSIONID=id”部分）保存Session Id。

#### c、会话监听器

会话监听器用于监听会话创建、过期及停止事件，自定义监听器实现`SessionListener`接口，如下代码：

```java
public class MySessionListener1 implements SessionListener {  
    @Override  
    public void onStart(Session session) {//会话创建时触发  
        System.out.println("会话创建：" + session.getId());  
    }  
    @Override  
    public void onExpiration(Session session) {//会话过期时触发  
        System.out.println("会话过期：" + session.getId());  
    }  
    @Override  
    public void onStop(Session session) {//退出/会话过期时触发  
        System.out.println("会话停止：" + session.getId());  
    }    
} 
```

#### d、会话存储（SessionDao）

SessionDao用于会话的增删查改：

```java
public interface SessionDAO {
  	//如DefaultSessionManager在创建完session后会调用该方法；如保存到关系数据库/文件系统/NoSQL数据库；即可以实现会话的持久化；返回会话ID；主要此	   处返回的ID.equals(session.getId())；  
	Serializable create(Session session);  
	//根据会话ID获取会话  
	Session readSession(Serializable sessionId) throws UnknownSessionException;  
	//更新会话；如更新会话最后访问时间/停止会话/设置超时时间/设置移除属性等会调用  
	void update(Session session) throws UnknownSessionException;  
	//删除会话；当会话过期/会话停止（如用户退出时）会调用  
	void delete(Session session);  
	//获取当前所有活跃用户，如果用户量多此方法影响性能  
	Collection<Session> getActiveSessions();
}
```

如下为各类SessionDao的关系继承图：

![](http://ot0aou666.bkt.clouddn.com/SessionDao%E7%BB%A7%E6%89%BF%E5%85%B3%E7%B3%BB%E5%9B%BE.png)

* `AbstractSessionDAO`：             提供了SessionDAO的基础实现，如生成会话ID等；
* `CachingSessionDAO`：              提供了对开发者透明的会话缓存的功能，只需要设置相应的CacheManager即可；
* `MemorySessionDAO`：                直接在内存中进行会话维护；
* `EnterpriseCacheSessionDAO`：提供了缓存功能的会话维护，默认情况下使用MapCache实现，内部使用ConcurrentHashMap保存缓存的会话。

自定义SessionDao如下，继承`CachingSessionDAO`类，重写增删查改方法：

```java
public class MySessionDAO extends CachingSessionDAO {  
  
  private JdbcTemplate jdbcTemplate = JdbcTemplateUtils.jdbcTemplate(); 
  
     protected Serializable doCreate(Session session) {  
        Serializable sessionId = generateSessionId(session);//生成会话id
        assignSessionId(session, sessionId); //将会话id设置进session中
        String sql = "insert into sessions(id, session) values(?,?)";  
        jdbcTemplate.update(sql, sessionId, SerializableUtils.serialize(session)); //执行数据库语句
        return session.getId();  
    }  
  
	protected void doUpdate(Session session) {  
        if(session instanceof ValidatingSession && !((ValidatingSession)session).isValid()) {  
            return; //如果会话过期/停止 没必要再更新了
        }  
            String sql = "update sessions set session=? where id=?";  
            jdbcTemplate.update(sql, SerializableUtils.serialize(session), session.getId());  
        }  
  
    protected void doDelete(Session session) {  
        String sql = "delete from sessions where id=?";  
        jdbcTemplate.update(sql, session.getId());  
    }
  
    protected Session doReadSession(Serializable sessionId) {  
        String sql = "select session from sessions where id=?";  
        List<String> sessionStrList = jdbcTemplate.queryForList(sql, String.class, sessionId);  
        if(sessionStrList.size() == 0) return null;  
        return SerializableUtils.deserialize(sessionStrList.get(0));  
    }  
}   
```

spring配置文件配置：

```xml
<!-- 配置sessionIdCookie -->
<bean id="sessionIdCookie" class="org.apache.shiro.web.servlet.SimpleCookie">
        <!-- 配置Cookie各类信息 -->
        <property name="maxAge" value="1800" />
        <property name="httpOnly" value="false" />
 		<property name="name" value="cookie" />
</bean>
<!-- 配置缓存管理器 -->
<bean id="cacheManager" class="org.apache.shiro.cache.ehcache.EhCacheManager">
        <!-- 配置缓存信息 -->
        <property name="cacheManagerConfigFile" value="cacheManagerConfigFile.xml" />
</bean>
<!-- 配置生成会话id的生成器 -->
<bean id="sessionIdGenerator" class="org.apache.shiro.session.mgt.eis.JavaUuidSessionIdGenerator" />
    	<!-- 配置自定义的SessionDao -->
<bean id="MySessionDao" class="shiroWeb.filter.MySessionDao" >
        <property name="cacheManager" ref="cacheManager" />
        <property name="sessionIdGenerator" ref="sessionIdGenerator" />
</bean>
<!-- 配置会话管理器 -->
<bean id="sessionManager" class="org.apache.shiro.web.session.mgt.DefaultWebSessionManager">
        <property name="globalSessionTimeout" value="1800000" />
        <property name="deleteInvalidSessions" value="false" />
        <property name="sessionIdCookie" ref="sessionIdCookie" />
        <property name="sessionDAO" ref="MySessionDao" />
</bean>
<!-- 配置安全管理器 -->
<bean id="securityManager" class="org.apache.shiro.web.mgt.DefaultWebSecurityManager">
        <!-- 将会话管理器配给安全管理器 -->
        <property name="sessionManager" ref="sessionManager" />
        <!-- 将缓存管理器配给安全管理器 -->
        <property name="cacheManager" ref="cacheManager" />
        <!-- By default the servlet container sessions will be used.  Uncomment this line
             to use shiro's native sessions (see the JavaDoc for more): -->
        <!-- <property name="sessionMode" value="native"/> -->
</bean>
```

`cacheManagerConfigFile.xml`的基本配置如下：

```xml
<ehcache name="shiroCache">

    <cache name="shiro-activeSessionCache"
           maxElementsInMemory="10000"
           overflowToDisk="true"
           eternal="true"
           timeToLiveSeconds="0"
           timeToIdleSeconds="0"/>
    
    <cache name="authorizationCache"
           maxElementsInMemory="2000"
           eternal="false"
           timeToIdleSeconds="1800"
           timeToLiveSeconds="0"
           overflowToDisk="false">
    </cache>
    <!--
        “name=”xxx””：对要进行缓存的项进行一个标注；
        “maxElementsInMemory=”2000”：可以缓存的最大的对象个数;
        “eternal=”false”：是否允许自动失效（如果某一个对象长时间不使用）； 
        “timeToIdleSeconds=”1800”：最小的失效时间，1800秒； 
        “timeToLiveSeconds=”0”：最大的保存时间，单位是秒； 
        “overflowToDisk=”false”：如果容量过多，可以将其保存在磁盘 
    -->
</ehcache>
```

#### e、会话验证

Shiro提供了会话验证调度器，用于定期的验证会话是否已过期，如果过期将停止会话；出于性能考虑，一般情况下都是获取会话时来验证会话是否过期并停止会话的；但是如在web环境中，如果用户不主动退出是不知道会话是否过期的，因此需要定期的检测会话是否过期，Shiro提供了会话验证调度器`SessionValidationScheduler`来做这件事情。

spring配置验证调度器如下：

```xml
<bean id="sessionManager" class="org.apache.shiro.web.session.mgt.DefaultWebSessionManager">
        <property name="sessionValidationScheduler" ref="sessionValidationScheduler" />
</bean>
<!-- 配置会话验证调度器 -->
<bean id="sessionValidationScheduler" class="org.apache.shiro.session.mgt.ExecutorServiceSessionValidationScheduler">
        <property name="interval" value="3600000" />
        <property name="sessionManager" ref="sessionManager" />
</bean>
<!-- 
sessionValidationScheduler：会话验证调度器，sessionManager默认就是使用ExecutorServiceSessionValidationScheduler，其使用JDK的
   							ScheduledExecutorService进行定期调度并验证会话是否过期；
sessionValidationScheduler.interval：设置调度时间间隔，单位毫秒，默认就是1小时；
sessionValidationScheduler.sessionManager：设置会话验证调度器进行会话验证时的会话管理器；
sessionManager.globalSessionTimeout：设置全局会话超时时间，默认30分钟，即如果30分钟内没有访问会话将过期；
sessionManager.sessionValidationSchedulerEnabled：是否开启会话验证器，默认是开启的；
sessionManager.sessionValidationScheduler：设置会话验证调度器，默认就是使用ExecutorServiceSessionValidationScheduler。
	
-->
```

实现验证都是调用了`AbstractValidatingSessionManager`类 的`validateSessions`方法进行验证，此处不做延伸

如果在会话过期时不想删除过期的会话，也可在sessionManager中配置：

```xml
<bean id="sessionManager" class="org.apache.shiro.web.session.mgt.DefaultWebSessionManager">
        <property name="deleteInvalidSessions" value="false" />
</bean>
```

默认是开启的，在会话过期后会调用SessionDAO的delete方法删除会话：如会话时持久化存储的，可以调用此方法进行删除



## 四、shiro整合spring

### 1、基本配置

**web.xml**

```xml
<filter>
  <filter-name>shiroFilter</filter-name>
  <filter-class>org.springframework.web.filter.DelegatingFilterProxy</filter-class>
  <init-param>
    <param-name>targetFilterLifecycle</param-name>
    <param-value>true</param-value>
  </init-param>
</filter>

<filter-mapping>
  <filter-name>shiroFilter</filter-name>
  <url-pattern>/*</url-pattern>
</filter-mapping>
```



**spring.xml的配置**

```xml
<!-- 配置自定义域 -->
<bean id="myRealm" class="shiroWeb.realm.MyRealm" />

<!-- 配置安全管理器 -->
<bean id="securityManager" class="org.apache.shiro.web.mgt.DefaultWebSecurityManager">
  <!-- Single realm app.  If you have multiple realms, use the 'realms' property instead. -->
  <property name="realm" ref="myRealm"/>
  <!-- By default the servlet container sessions will be used.  Uncomment this line
             to use shiro's native sessions (see the JavaDoc for more): -->
  <!-- <property name="sessionMode" value="native"/> -->
</bean>

<bean id="shiroFilter" class="org.apache.shiro.spring.web.ShiroFilterFactoryBean">
    <!-- shiro的核心安全接口 -->
    <property name="securityManager" ref="securityManager"/>
    <!-- override these for application-specific URLs if you like: -->
    <!-- 身份认证失败，则跳转到登陆页面的配置 -->
    <property name="loginUrl" value="/login.jsp"/>
    <!-- 权限认证失败，则跳转到指定页面的配置 -->
    <property name="unauthorizedUrl" value="/unauthorized.jsp"/>
    <!-- The 'filters' property is not necessary since any declared javax.servlet.Filter bean  -->
    <!-- defined will be automatically acquired and available via its beanName in chain        -->
    <!-- definitions, but you can perform instance overrides or name aliases here if you like: -->
    <!-- <property name="filters">
              <util:map>
                  <entry key="anAlias" value-ref="someFilter"/>
              </util:map>
          </property> -->
    <!-- shiro连接约束配置，即过滤链的定义,重点！！！ -->
    <property name="filterChainDefinitions">
      <value>
        /login=anon
        /admin*=authc
        /student=roles[teacher]
        /teacher=perms["user:create"]
      </value>
    </property>
</bean>

<!-- 保证实现了shiro内部lifecycle函数的bean执行 -->
<bean id="lifecycleBeanPostProcessor" class="org.apache.shiro.spring.LifecycleBeanPostProcessor"/>

<!-- 开启shiro注解 -->
<bean class="org.springframework.aop.framework.autoproxy.DefaultAdvisorAutoProxyCreator" depends-on="lifecycleBeanPostProcessor"/>
<bean class="org.apache.shiro.spring.security.interceptor.AuthorizationAttributeSourceAdvisor">
  <property name="securityManager" ref="securityManager"/>
</bean>
```



**登录的controller**

```java
@RequestMapping("/login")
public String login(User user , HttpServletRequest request){
    Subject subject = SecurityUtils.getSubject();
    UsernamePasswordToken token = new UsernamePasswordToken(user.getUserName() ,user.getPassword());
    try{
      subject.login(token);
      Session session = subject.getSession();
      System.out.println("session id : " + session.getId());
      session.setAttribute("info" ,"session的数据");
      return "redirect:/Success.jsp";
    }catch (Exception e){
      e.printStackTrace();
      request.setAttribute("user" ,user);
      request.setAttribute("error" ,"用户名或密码错误");
      return "login";
    }
}
```



### 2、filter过滤机制



a、在`web.xml`中配置过滤器`DelegatingFilterProxy`，该过滤器的作用是**将具体工作分派给`org.apache.shiro.spring.web.ShiroFilterFactoryBean`这个类中的静态内部类`SpringShiroFilter`做**。在spring配置文件中注入bean：ShiroFilterFactoryBean后，属性`filterChainDefinitions`也被设置进`ShiroFilterFactoryBean`类中，如下代码：

```java
public void setFilterChainDefinitions(String definitions) {
    Ini ini = new Ini();
    ini.load(definitions);
    Section section = ini.getSection("urls");//spring配置文件中不需要声明[urls]，shiro内部已经弄好
    if (CollectionUtils.isEmpty(section)) {
      section = ini.getSection("");
    }

    this.setFilterChainDefinitionMap(section);
}
```

而其中definitions为String类型，因此使用了ini转换方法（内部使用`LinkedHashMap`保存url和filter的映射关系），与上述ini配置realm的转换方法一致，都是调用了

`load(new Scanner(iniConfig))`方法。

ini的设置为如下：

```ini
[section1]
key1=value1
 
[section2]
key2=value2
 
。。。。
```

ini转换：

在上面代码中，`load`方法起到关键作用。如下代码：

```java
public void load(String iniConfig) throws ConfigurationException {
  	load(new Scanner(iniConfig));
}
```

shiro中自定义了ini类和Section类，在`load(Scanner scanner)`方法中，通过scanner一行一行地扫描ini配置文件内的信息或者从spring配置文件中传进来的`FilterChainDefinitions`。在Ini类中声明了一个变量sections

```java
public class Ini implements Map<String, Ini.Section> {
  ...	
  private final Map<String, Section> sections;
  ...
}
```

在load方法中，声明了sectionName和sectionContent变量，分别代表该节的名称及内容，例如sectionName为`users`，sectionContent为`jack=123`。随后调用了`void addSection(String name, StringBuilder content)`方法，在内创建一个新的section，而在`Section`的构造方法中，它将sectionContent分为了=前的key以及=号后的value，并将其设置进一个`LinkedHashMap<String,String>`中，因此=号前与=号后的关系为`key-value`关系，如下代码：

```java
public static class Section implements Map<String, String> {
      ...
	  private final Map<String, String> props;
  	  ...
      private Section(String name, String sectionContent) {
          if (name == null) {
            throw new NullPointerException("name");
          }
          this.name = name;
          Map<String,String> props;//创建Map
          if (StringUtils.hasText(sectionContent) ) {
            props = toMapProps(sectionContent);//调用自定义方法将=号前与=号后的关系设置进map并返回map
          } else {
            props = new LinkedHashMap<String,String>();
          }
          if ( props != null ) {
            this.props = props;
          } else {
            this.props = new LinkedHashMap<String,String>();
          }
      }
      ...
}
```

在`addSection`方法中，new完Section对象后，会将sectionName以及new出来的Section放置进Ini声明的映射变量`Map<String, Section> sections`中

```java
private void addSection(String name, StringBuilder content) {
  if (content.length() > 0) {
    String contentString = content.toString();
    String cleaned = StringUtils.clean(contentString);
    if (cleaned != null) {
      Section section = new Section(name, contentString);//创建新的Section对象
      if (!section.isEmpty()) {
        sections.put(name, section);//将SectionName与section放置进sections中
      }
    }
  }
}
```

definitions转换完之后，`load`方法中最后调用了`this.setFilterChainDefinitionMap(section)`方法将获得的section设置进去，`setFilterChainDefinitionMap`方法的参数为`Map<String, String>`类型，而`Section`类实现了`Map<String, String>`接口，所以可以set进去

```java
public void setFilterChainDefinitionMap(Map<String, String> filterChainDefinitionMap) {
  	this.filterChainDefinitionMap = filterChainDefinitionMap;
}
```

​	**从spring配置文件中获取到定义的`filterChainDefinitions`信息后通过使用ini转换方法将配置的`filterChainDefinitions`的url与权限的映射关系置入map中，并将其set进`ShiroFilterFactoryBean`类的变量`Map<String, String> filterChainDefinitionMap`，以便后面使用**



b、`ShiroFilterFactoryBean`类的`createInstance()`方法是shiro的filter构造机制的主线，spring通过此方法获取filter实例

```java
protected AbstractShiroFilter createInstance() throws Exception {
        ...
        FilterChainManager manager = createFilterChainManager();
       
        PathMatchingFilterChainResolver chainResolver = new PathMatchingFilterChainResolver();
        chainResolver.setFilterChainManager(manager);//set入FilterChainManager，在后面getExecutionChain方法用到
       
        return new SpringShiroFilter((WebSecurityManager) securityManager, chainResolver);
    }
```

由此可知需先获取到`filterChainManager`，`createFilterChainManager()`方法如下：

```java
protected FilterChainManager createFilterChainManager() {
  		//应用shiro默认的filter：
        DefaultFilterChainManager manager = new DefaultFilterChainManager();
        Map<String, Filter> defaultFilters = manager.getFilters();
        //apply global settings if necessary:
        for (Filter filter : defaultFilters.values()) {
            applyGlobalPropertiesIfNecessary(filter);
        }
        //应用在spring配置中自定义的filter:
        Map<String, Filter> filters = getFilters();//获取spring配置文件中配置的filters
        if (!CollectionUtils.isEmpty(filters)) {
            for (Map.Entry<String, Filter> entry : filters.entrySet()) {
                String name = entry.getKey();
                Filter filter = entry.getValue();
                applyGlobalPropertiesIfNecessary(filter);
                if (filter instanceof Nameable) {
                    ((Nameable) filter).setName(name);
                }
                //'init' argument is false, since Spring-configured filters should be initialized
                //in Spring (i.e. 'init-method=blah') or implement InitializingBean:
                manager.addFilter(name, filter, false);//实例化ini自定义的filter
            }
        }
        //建立链:
        Map<String, String> chains = getFilterChainDefinitionMap();//获取上述setFilterChainDefinitionMap中的filterChainDefinitionMap
        if (!CollectionUtils.isEmpty(chains)) {
            for (Map.Entry<String, String> entry : chains.entrySet()) {
                String url = entry.getKey();//=号前的url
                String chainDefinition = entry.getValue();//=号后的信息
                manager.createChain(url, chainDefinition);
            }
        }
        return manager;
    }
```



而代码第一行的`DefaultFilterChainManager`构造方法中，已经**将shiro默认的filter初始化**：

```java
public DefaultFilterChainManager() {
        this.addDefaultFilters(false);
    }
```

```java
protected void addDefaultFilters(boolean init) {
        DefaultFilter[] arr$ = DefaultFilter.values();
        int len$ = arr$.length;

        for(int i$ = 0; i$ < len$; ++i$) {
            DefaultFilter defaultFilter = arr$[i$];
            this.addFilter(defaultFilter.name(), defaultFilter.newInstance(), init, false);//通过传入过滤器名称和实例在内部将ilter初始化init并add进Map<String, Filter> filters中
        }

    }
```

```java
//枚举类DefaultFilter
public enum DefaultFilter {
    anon(AnonymousFilter.class),
    authc(FormAuthenticationFilter.class),
    authcBasic(BasicHttpAuthenticationFilter.class),
    logout(LogoutFilter.class),
    noSessionCreation(NoSessionCreationFilter.class),
    perms(PermissionsAuthorizationFilter.class),
    port(PortFilter.class),
    rest(HttpMethodPermissionFilter.class),
    roles(RolesAuthorizationFilter.class),
    ssl(SslFilter.class),
    user(UserFilter.class);
  ...
}
```



而在`DefaultFilterChainManager`类中还有一个方法`createChain(String chainName, String chainDefinition)`在`FilterChainManager createFilterChainManager()`创建filter链时被使用，其作用是对权限(=号后的内容)进行分割，如`"authc, roles[admin,user], perms[file:edit]"`将被切割为`{ "authc", "roles[admin,user]", "perms[file:edit]" }`，如下代码：

```java
public void createChain(String chainName, String chainDefinition) {
     	...
        String[] filterTokens = splitChainDefinition(chainDefinition);
        for (String token : filterTokens) {
            String[] nameConfigPair = toNameConfigPair(token);
            addToChain(chainName, nameConfigPair[0], nameConfigPair[1]);
        }
    }
```

并且通过`toNameConfigPair`方法将如`roles[admin,user]`形式的字符串切割成`roles`和`admin,user`，每次分割完之后，都会调用`addToChain`方法接受，如下代码：

```java
//chainName表示url；filterName代表=号前的信息，如roles；chainSpecificFilterConfig代表=号后的信息，如admin,user
public void addToChain(String chainName, String filterName, String chainSpecificFilterConfig) {
        if (!StringUtils.hasText(chainName)) {
            throw new IllegalArgumentException("chainName cannot be null or empty.");
        } else {
            Filter filter = this.getFilter(filterName);//通过filterName获取自定义的filter
            if (filter == null) {
                throw new IllegalArgumentException("There is no filter with name '" + filterName + "' to apply to chain [" + chainName + "] in the pool of available Filters.  Ensure a " + "filter with that name/path has first been registered with the addFilter method(s).");
            } else {
                this.applyChainConfig(chainName, filter, chainSpecificFilterConfig);
                NamedFilterList chain = this.ensureChain(chainName);
                chain.add(filter);
            }
        }
    }
```

分析`applyChainConfig(String chainName, Filter filter, String chainSpecificFilterConfig)`方法

```java
protected void applyChainConfig(String chainName, Filter filter, String chainSpecificFilterConfig) {
        if (filter instanceof PathConfigProcessor) {
            ((PathConfigProcessor) filter).processPathConfig(chainName, chainSpecificFilterConfig);//将url和对应的过滤条件都放到对应filter的appliedPaths中
        } else {
            if (StringUtils.hasText(chainSpecificFilterConfig)) {
                //they specified a filter configuration, but the Filter doesn't implement PathConfigProcessor
                //this is an erroneous config:
                String msg = "chainSpecificFilterConfig was specified, but the underlying " +
                        "Filter instance is not an 'instanceof' " +
                        PathConfigProcessor.class.getName() + ".  This is required if the filter is to accept " +
                        "chain-specific configuration.";
                throw new ConfigurationException(msg);
            }
        }
}
```

其中的`processPathConfig`方法处于`PathMatchingFilter`类中。而此类中有一个变量`Map<String, Object> appliedPaths = new LinkedHashMap();`

传入的url及对应的过滤条件都会放在此变量中

```java
public Filter processPathConfig(String path, String config) {
        String[] values = null;
        if (config != null) {
            values = split(config);//去除过滤条件中存在的逗号
        }
        this.appliedPaths.put(path, values);
        return this;
}
```

回到`addToChain`方法，执行`NamedFilterList chain = this.ensureChain(chainName);`语句，分析`ensureChain`方法

```java
protected NamedFilterList ensureChain(String chainName) {
        NamedFilterList chain = this.getChain(chainName);
        if (chain == null) {
            chain = new SimpleNamedFilterList(chainName);
            this.filterChains.put(chainName, chain); 
        }

        return (NamedFilterList)chain;
    }
```

获取到chainName和chain之后将其设进`Map<String, NamedFilterList> filterChains`中，分析`SimpleNamedFilterList`构造方法

```java
public SimpleNamedFilterList(String name) {
        this(name, new ArrayList());
    }

public SimpleNamedFilterList(String name, List<Filter> backingList) {
    if (backingList == null) {
      throw new NullPointerException("backingList constructor argument cannot be null.");
    } else {
      this.backingList = backingList;
      this.setName(name);
    }
}
```

在构造方法中创建了一个变量`List<Filter> backingList`并将其set进类变量backingList，再次回到`addToChain`方法，执行` chain.add(filter);`，由于返回的是`SimpleNamedFilterList`类的变量，调用的是`SimpleNamedFilterList`类的add方法，将filter增加进backingList中，以便后面`ProxiedFilterChain`调用doFilter方法时使用filter

```java
public boolean add(Filter filter) {
        return this.backingList.add(filter);
    }
```





总结：

1、 定义一个`DefaultFilterChainManager`对象

2， 首先加载默认的filter( `DefaultFilterChainManager()`，`addDefaultFilters` )

3， 如果有配置自定义的filter，则加载xml文件中定义的filter

4， 加载xml文件定义的url和filter映射关系

5， 将url和filter的映射关系解析为以url为键，`NamedFilterList`(此类继承了`List<Filter>`)为值的键值对(filterChains，定义于`DefaultFilterManager`类中)（`createChain` ,  `addToChain`）

6， 在解析的过程中，对每个url和对应的过滤条件，都会放到对应filter的appliedPaths中（在PathMatchingFilter中定义）（`processPathConfig`）

`FilterChainManager`的对象已经创建完毕，并且每个filter也已经实例化完毕。





![](http://ot0aou666.bkt.clouddn.com/shiro%E8%BF%87%E6%BB%A4%E8%BF%87%E7%A8%8B.png)

c、如上图，首先调用在web.xml配置好的`DelegatingFilterProxy`类中的`doFilter`方法

```java
public void doFilter(ServletRequest request, ServletResponse response, FilterChain filterChain)
			throws ServletException, IOException {

		// Lazily initialize the delegate if necessary.
		Filter delegateToUse = this.delegate;
		if (delegateToUse == null) {
			synchronized (this.delegateMonitor) {
				if (this.delegate == null) {
					WebApplicationContext wac = findWebApplicationContext();
					if (wac == null) {
						throw new IllegalStateException("No WebApplicationContext found: " +
								"no ContextLoaderListener or DispatcherServlet registered?");
					}
					this.delegate = initDelegate(wac);
				}
				delegateToUse = this.delegate;
			}
		}

		// Let the delegate perform the actual doFilter operation.
		invokeDelegate(delegateToUse, request, response, filterChain);
	}
```

其中调用了`invokeDelegate(delegateToUse, request, response, filterChain)`方法开始进行过滤工作，该方法内又调用了另外一个`doFilter`方法

```java
protected void invokeDelegate(
			Filter delegate, ServletRequest request, ServletResponse response, FilterChain filterChain)
			throws ServletException, IOException {

		delegate.doFilter(request, response, filterChain);
	}
```

该`doFilter`方法由`OncePerRequestFilter`类实现，如下：

```java
public final void doFilter(ServletRequest request, ServletResponse response, FilterChain filterChain) throws ServletException, IOException {
        String alreadyFilteredAttributeName = this.getAlreadyFilteredAttributeName();
        if (request.getAttribute(alreadyFilteredAttributeName) != null) {
          //如果filter已被执行，则不涉及该filter
            log.trace("Filter '{}' already executed.  Proceeding without invoking this filter.", this.getName());
            filterChain.doFilter(request, response);
        } else if (this.isEnabled(request, response) && !this.shouldNotFilter(request)) {  
          log.trace("Filter '{}' not yet executed.  Executing now.", this.getName());
            request.setAttribute(alreadyFilteredAttributeName, Boolean.TRUE);

            try {
                this.doFilterInternal(request, response, filterChain);//关键
            } finally {
                request.removeAttribute(alreadyFilteredAttributeName);
            }
        } else {
            log.debug("Filter '{}' is not enabled for the current request.  Proceeding without invoking this filter.", this.getName());
            filterChain.doFilter(request, response);
        }

    }
```

可以发现方法最终调用了`doFilterInternal(request, response, filterChain)`方法，且调用的类是`ShiroFilterFactoryBean`类的内部类`SpringShiroFilter`，该方法在`SpringShiroFilter`的父类`AbstractShiroFilter`实现，其作用是在shiro真正验证授权前对subject，session等进行初始化，使后面的过滤工作可以得到subject等以正常进行

```java
//AbstractShiroFilter类实现的doFilterInternal方法
protected void doFilterInternal(ServletRequest servletRequest, ServletResponse servletResponse, final FilterChain chain)
            throws ServletException, IOException {
        Throwable t = null;
        try {
            final ServletRequest request = prepareServletRequest(servletRequest, servletResponse, chain);
            final ServletResponse response = prepareServletResponse(request, servletResponse, chain);
            final Subject subject = createSubject(request, response);//创建subject
            subject.execute(new Callable() {
                public Object call() throws Exception {
                    updateSessionLastAccessTime(request, response);//初始化session
                    executeChain(request, response, chain);//执行过滤链
                    return null;
                }
            });
        } catch (ExecutionException ex) {
            t = ex.getCause();
        } catch (Throwable throwable) {
            t = throwable;
        }
        //…………
    }
```

分析`executeChain(request, response, chain)`方法

```java
protected void executeChain(ServletRequest request, ServletResponse response, FilterChain origChain) throws IOException, ServletException {
        FilterChain chain = this.getExecutionChain(request, response, origChain);
        chain.doFilter(request, response);//引用ProxiedFilterChain类实现的doFilter方法
    }
```

分析`getExecutionChain`方法

```java
protected FilterChain getExecutionChain(ServletRequest request, ServletResponse response, FilterChain origChain) {
        FilterChain chain = origChain;
        FilterChainResolver resolver = this.getFilterChainResolver();//在子类SpringShiroFilter已经set过
        if (resolver == null) {
            log.debug("No FilterChainResolver configured.  Returning original FilterChain.");
            return origChain;
        } else {
            FilterChain resolved = resolver.getChain(request, response, origChain);//获取filterchain
            if (resolved != null) {
                log.trace("Resolved a configured FilterChain for the current request.");
                chain = resolved;
            } else {
                log.trace("No FilterChain configured for the current request.  Using the default.");
            }

            return chain;
        }
    }
```

分析`getChain`方法

```java
public FilterChain getChain(ServletRequest request, ServletResponse response, FilterChain originalChain) {
        FilterChainManager filterChainManager = this.getFilterChainManager();//在ShiroFilterFactoryBean类中的AbstractShiroFilter createInstance()方法中已经set过
        if (!filterChainManager.hasChains()) {
            return null;
        } else {
          //对比存储的url与请求是否匹配
            String requestURI = this.getPathWithinApplication(request);
            Iterator i$ = filterChainManager.getChainNames().iterator();//获取存储的key值url

            String pathPattern;
            do {
                if (!i$.hasNext()) {
                    return null;
                }

                pathPattern = (String)i$.next();
            } while(!this.pathMatches(pathPattern, requestURI));

            if (log.isTraceEnabled()) {
                log.trace("Matched path pattern [" + pathPattern + "] for requestURI [" + requestURI + "].  " + "Utilizing corresponding filter chain...");
            }

            return filterChainManager.proxy(originalChain, pathPattern);
        }
    }
```

`return filterChainManager.proxy(originalChain, pathPattern)`返回的是一个`ProxiedFilterChain`类的对象，而在`proxy`方法中，new出`ProxiedFilterChain`类的对象并在构造方法中将上述提到的`SimpleNamedFilterList`类的`List<Filter> backingList`set进去了，以便下述的doFilter方法使用。所以在`executeChain`方法中，调用由`ProxiedFilterChain`实现的`doFilter`方法，代码如下：

```java
public void doFilter(ServletRequest request, ServletResponse response) throws IOException, ServletException {
        if (this.filters != null && this.filters.size() != this.index) {
            if (log.isTraceEnabled()) {
                log.trace("Invoking wrapped filter at index [" + this.index + "]");
            }
			//调用OncePerRequestFilter的doFilter方法
            ((Filter)this.filters.get(this.index++)).doFilter(request, response, this);
        } else {
            if (log.isTraceEnabled()) {
                log.trace("Invoking original filter chain.");
            }

            this.orig.doFilter(request, response);
        }

    }
```

` ((Filter)this.filters.get(this.index++)).doFilter(request, response, this)`此句代码调用了`OncePerRequestFilter`类的doFilter方法（自定义过滤器与默认的过滤器的doFilter方法由`OncePerRequestFilter`类实现），与上面相同，到最后调用了`doFilterInternal`方法，但此时，调用者已是继承于`AdviceFilter`类的过滤器类，如anoc（AnonymousFilter）等，因此实现的方法也会有所不同，调用类`AdviceFilter`实现的`doFilterInternal`方法：

```java
public void doFilterInternal(ServletRequest request, ServletResponse response, FilterChain chain) throws ServletException, IOException {
        Exception exception = null;

        try {
            boolean continueChain = this.preHandle(request, response);
            if (log.isTraceEnabled()) {
                log.trace("Invoked preHandle method.  Continuing chain?: [" + continueChain + "]");
            }

            if (continueChain) {
                this.executeChain(request, response, chain);
            }

            this.postHandle(request, response);
            if (log.isTraceEnabled()) {
                log.trace("Successfully invoked postHandle method");
            }
        } catch (Exception var9) {
            exception = var9;
        } finally {
            this.cleanup(request, response, exception);
        }

    }
```

此处通过`preHandle(request, response)`方法的返回值来决定后续filter会不会继续执行`executeChain(request, response, chain)`，关注`preHandle`方法，此方法由`PathMatchingFilter`重写

```java
protected boolean preHandle(ServletRequest request, ServletResponse response) throws Exception {
        if (this.appliedPaths != null && !this.appliedPaths.isEmpty()) {
          //判断存储的url与请求是否匹配
            Iterator i$ = this.appliedPaths.keySet().iterator();

            String path;
            do {
                if (!i$.hasNext()) {
                    return true;
                }

                path = (String)i$.next();
            } while(!this.pathsMatch(path, request));

            log.trace("Current requestURI matches pattern '{}'.  Determining filter chain execution...", path);
            Object config = this.appliedPaths.get(path);
            return this.isFilterChainContinued(request, response, path, config);
        } else {
            if (log.isTraceEnabled()) {
                log.trace("appliedPaths property is null or empty.  This Filter will passthrough immediately.");
            }

            return true;
        }
    }
```

将定义的`Map<String, Object> appliedPaths`进行迭代，将key，及url与请求的url进行比较，然后将key和value作为参数传入方法`this.isFilterChainContinued(request, response, path, config)`中，分析该方法

```java
private boolean isFilterChainContinued(ServletRequest request, ServletResponse response, String path, Object pathConfig) throws Exception {
        if (this.isEnabled(request, response, path, pathConfig)) {
            if (log.isTraceEnabled()) {
                log.trace("Filter '{}' is enabled for the current request under path '{}' with config [{}].  Delegating to subclass implementation for 'onPreHandle' check.", new Object[]{this.getName(), path, pathConfig});
            }
			//将value作为参数传入方法
            return this.onPreHandle(request, response, pathConfig);
        } else {
            if (log.isTraceEnabled()) {
                log.trace("Filter '{}' is disabled for the current request under path '{}' with config [{}].  The next element in the FilterChain will be called immediately.", new Object[]{this.getName(), path, pathConfig});
            }

            return true;
        }
    }
```

其中调用了`onPreHandle`方法，由`AccessControlFilter`类进行重写

```java
public boolean onPreHandle(ServletRequest request, ServletResponse response, Object mappedValue) throws Exception {
        return this.isAccessAllowed(request, response, mappedValue) || this.onAccessDenied(request, response, mappedValue);
    }
```

而shiro默认的filter类均继承了`AccessControlFilter`类，且重写了`isAccessAllowed`与`onAccessDenied`方法，例如`RolesAuthorizationFilter`类，即`roles`，为如下代码：

```java
 public boolean isAccessAllowed(ServletRequest request, ServletResponse response, Object mappedValue) throws IOException 	{
        Subject subject = this.getSubject(request, response);
        String[] rolesArray = (String[])((String[])mappedValue);
        if (rolesArray != null && rolesArray.length != 0) {
            Set<String> roles = CollectionUtils.asSet(rolesArray);
            return subject.hasAllRoles(roles);
        } else {
            return true;
        }
    }
```

而当我们想自定义过滤器时也可以继承`AccessControlFilter`等类来实现`isAccessAllowed`、`onAccessDenied`等方法。当一切判断完后，如果能通过则执行` this.executeChain(request, response, chain);`，该方法内部调用`ProxiedFilterChain`类的doFilter方法，当下标index与filters的size不一样大时，则继续循环刚才的流程，但执行的filter不同；当下标index与filters的size一样大时，则跳出过滤器链doFilter的循环。

```java
public void doFilter(ServletRequest request, ServletResponse response) throws IOException, ServletException {
        if (this.filters != null && this.filters.size() != this.index) {
            if (log.isTraceEnabled()) {
                log.trace("Invoking wrapped filter at index [" + this.index + "]");
            }
			//一个一个filter进行doFilter，默认的filter进行过滤
            ((Filter)this.filters.get(this.index++)).doFilter(request, response, this);
        } else {
            if (log.isTraceEnabled()) {
                log.trace("Invoking original filter chain.");
            }

            this.orig.doFilter(request, response);
        }

    }
```

以下为filter关系继承图：

![](http://ot0aou666.bkt.clouddn.com/shiro%E6%8B%A6%E6%88%AA%E5%99%A8%E7%BB%A7%E6%89%BF%E5%9B%BE.png)



**小结：整个过滤机制分为两个部分：准备部分（步骤a、b）和工作部分（步骤c），将spring配置文件中的权限信息通过ini转换方法把权限关系拆分、过滤器名称、实体set进不同的类变量中，并且将默认的过滤器以及自定义的过滤器初始化好，然后通过这些变量为后面代码执行提供filter和其对应的信息，最后循环执行过滤，通过判断（`isAccessAllowed`、`onAccessDenied`等方法，这些方法被默认的过滤器类或自定义的过滤器类重写）来判定filterChain是否能继续执行过滤**



## jdbcRealm基本配置

```ini
[main]
jdbcRealm=org.apache.shiro.realm.jdbc.JdbcRealm
dataSource=com.alibaba.druid.pool.DruidDataSource
#MySQL驱动
dataSource.driverClassName=com.mysql.jdbc.Driver
#MySQL相关参数配置
dataSource.url=jdbc:mysql://localhost:3306/db.shiro?useSSL=false&useUnicode=true&characterEncoding=UTF-8
dataSource.username=root
dataSource.password=123456
#引入相关数据配置
jdbcRealm.dataSource=$dataSource
securityManager.realms=$jdbcRealm
```













