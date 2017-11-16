

## 基本流程

当我们对SpringMVC控制的资源发起请求时，这些请求都会被SpringMVC的DispatcherServlet处理，接着Spring会分析看哪一个HandlerMapping定义的所有请求映射中存在对该请求的最合理的映射。然后通过该HandlerMapping取得其对应的Handler，接着再通过相应的HandlerAdapter处理该Handler。HandlerAdapter在对Handler进行处理之后会返回一个ModelAndView对象。在获得了ModelAndView对象之后，Spring就需要把该View渲染给用户，即返回给浏览器。在这个渲染的过程中，发挥作用的就是ViewResolver和View。当Handler返回的ModelAndView中不包含真正的视图，只返回一个逻辑视图名称的时候，ViewResolver就会把该逻辑视图名称解析为真正的视图View对象。View是真正进行视图渲染，把结果返回给浏览器的。

## 一、视图解析器

### 1、InternalResourceViewResolver

使用最广的`InternalResourceViewResolver`解析器：

![](http://ot0aou666.bkt.clouddn.com/InternalResourceViewResolver%E8%A7%86%E5%9B%BE%E8%A7%A3%E6%9E%90%E5%99%A8.png)



### 2、视图解析链

![](http://ot0aou666.bkt.clouddn.com/%E8%A7%86%E5%9B%BE%E8%A7%A3%E6%9E%90%E9%93%BE.png)

## 二、路径问题

1、由浏览器发起的路径，如重定向等，“/”表示%tomcat根目录%/webapps根目录

2、由服务器发起的路径，如请求转发、表单请求等，“/”表示项目中的webapp根目录

```java
/**
 * redirect:registerSuccess.jsp  --> /web/user/registerSuccess.jsp /web/user/login.jsp
 * redirect:/registerSuccess.jsp --> /web/registerSuccess.jsp      /web/login.jsp
 * forward:/register.jsp --> /web/register.jsp
 * forward:register.jsp -->  /web/user/register.jsp
 */
```

