# maven-archetype
# 建立自己的archetype流程

## 1、创建模板项目

准备好要建立的模板，将pom、配置、base类写好后，此项目即为archetype的模板。

## 2、创建骨架项目

在模板项目中执行`mvn archetype:create-from-project`，执行成功后将target/generated-sources/内的archetype目录拷贝到自己的workspace中，然后打开此项目

## 3、改动项目配置并打包

### 1、修改pom文件

- 自己定义骨架项目的maven坐标 (groupId ,artifactId,name,version)
- 修改packing的值为jar

### 2、修改archetype-metadata.xml

在`fileset`标签同级下增加`requiredProperties`标签，内置参数如下图所示标注：

![](http://ot0aou666.bkt.clouddn.com/20141215172845480.jpeg)

### 3、部署到本地仓库

将项目修改后执行`mvn clean install`命令部署到本地仓库中



## 4、使用

在创建module时，选择create from archetype，并add archetype将模板项目的坐标填入点击确认便能生成module