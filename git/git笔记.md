# git笔记

标签（空格分隔）： 未分类



## 一、安装git

### 1、输入修改git用户及邮箱

`git config -global user.name chenjiasheng`
`git config -global user.email 876833508@qq.com`

注：git config命令的--global参数，用了这个参数，表示你这台机器上所有的Git仓库都会使用这个配置，当然也可以对某个仓库指定不同的用户名和Email地址。

### 2、查看git用户及邮箱

`git config user.name`
`git config user.email`

## 二、版本库

（一个目录，用于git管理，每个文件的删除、修改都能被git跟踪，以便追踪历史和随时“还原”）

### 1、创建

​	(1)`mkdir learngit` //创建
​	(2)`cd learngit` //进入该文件
​	(3)`pwd` //显示当前目录
​	(4) 将当前文件目录变成git可管理的仓库：`git init`

### 2、添加提交

​	(1)在指定为管理仓库的文件下创建文件后将文件添加到仓库：`git add xxx` //如readme.tx
​	(2)随后将添加的文件提交到仓库中：`git commit -m "xxx"`

**注**：

​	(1)、git commit命令，-m后面输入的是本次提交的说明，可以输入任意内容，当然最好是有意义的，这样你就能从历史记录里方便地找到改动记录；

​	(2)、可将多个文件添加进入创库，即多次add，一次commit

## 三、工作区和暂存区

![](http://www.liaoxuefeng.com/files/attachments/001384907702917346729e9afbf4127b6dfbae9207af016000/0)

第一步是将工作区(电脑能看到的目录，如learngit文件)中的文件用`git add`把文件添加到stage的暂存区(.git即版本库内的东西，自动创建的第一个分支master以及指向分支的指针HEAD都是.git内的东西)
第二步是用`git commit`把暂存区的所有内容提交到当前分支(git为我们自动创建第一个分支 master或自定义分支)

## 四、基本语句

### 1、掌握仓库当前状态：`git status`

多频率出现的状态：

(1)文件被修改过但没有commit提交

```cmake
$ git status
  on branch master
  Changes not staged for commit:
    (use "git add <file>..." to update what will be committed)
    (use "git checkout -- <file>..." to discard changes in working directory)

     modified:   readme.txt

no changes added to commit (use "git add" and/or "git commit -a")
```



(2)当前没有需要提交的修改且工作目录是干净（working directory clean）的

```
On branch master
nothing to commit, working directory clean
```



### 2、查看文件修改的内容：`git diff xxx //如readme.txt`

### 3、回退版本

以时间为顺序，每个文件都会有不同的版本，版本名称为`git config -m "XXX"`语句中的说明，且每个版本都有各自的版本id。每次修改完文件后提交，相当于修改后保存文件。一旦把文件改乱了，或者误删了文件，可以从最近的一个commit恢复，然后继续工作,方法以下：

​	(1)查询提交历史，确定回退到哪个版本的版本id：`git log` 或`git log --pretty=oneline`

​	   `HEAD`表示当前版本，`HEAD^`表示上一版本，`HEAD^^`表示上上版本，`HEAD~100`表示往上100个版本

​	(2)从当前版本到其他版本：`git reset --hard id号`

查看文件的内容： `cat 文件名`
查看操作命令历史：`git reflog`

### 4、撤销修改

（1）丢弃工作区的修改：`git checkout -- fileName`

注：丢弃修改有两种情况:

​	a、readme.txt自修改后还没有被放到暂存区，现在，撤销修改就回到和版本库一模一样的状态；
​	b、readme.txt已经添加到暂存区后，又作了修改，现在，撤销修改就回到添加到暂存区后的状态。

（2）丢弃暂存区的修改：

分两步:
​	a、将暂存区修改回退到工作区：`git reset HEAD fileName`
​	b、丢弃工作区修改：`git checkout -- fileName`

### 5、删除文件

文件删除：
1、
(1)删除工作区和暂存区的文件：`git rm fileName`
(2)删除版本库的文件(提交)：`git commit -m "说明"`
2、
误删(手动的删除，而不是执行`git rm`)(版本库内还有)，将版本库内的版本替换工作区的版本:`git checkout -- fileName`(撤销删除)或者回退到某一版本:`git reset --hard xx`
注：执行`git rm`无法撤销删除，但可以使用回退

### 6、创建与合并分支

(1)、HEAD指针指向分支，分支指向提交

(2)、查看分支：`git branch`

​        创建分支：`git branch <name>`

​        切换分支：`git checkout <name>`

​        创建+切换分支：`git checkout -b <name>`

​        合并某分支到当前分支：`git merge <name>`

​        删除分支：`git branch -d <name>`

(3)、流程图
![](http://www.liaoxuefeng.com/files/attachments/001384908811773187a597e2d844eefb11f5cf5d56135ca000/0)
（创建dev分支）

![](http://www.liaoxuefeng.com/files/attachments/0013849088235627813efe7649b4f008900e5365bb72323000/0)
（增加了dev指针，HEAD指向改变，提交一次dev往前移动一步）

![](http://www.liaoxuefeng.com/files/attachments/00138490883510324231a837e5d4aee844d3e4692ba50f5000/0)
（合并master与dev分支）

![](http://www.liaoxuefeng.com/files/attachments/001384908867187c83ca970bf0f46efa19badad99c40235000/0)
（删除dev分支）



### 7、分支管理

分支合并（`no-ff`表示禁用`Fast forward`）：`git merge --no-ff -m "merge with no-ff" dev`(dev为分支名)

分支策略：
(1)、首先，master分支仅用来发布新版本，平时不能在上面干活；

(2)、干活都在dev分支上，也就是说，dev分支是不稳定的，到某个时候，比如1.0版本发布时，再把dev分支合并到master上，在master分支发布1.0版本

(3)、你和你的小伙伴们每个人都在dev分支上干活，每个人都有自己的分支，时不时地往dev分支上合并就可以了。

![](http://www.liaoxuefeng.com/files/attachments/001384909239390d355eb07d9d64305b6322aaf4edac1e3000/0)
（如图）

### 8、分支冲突

分支合并解决冲突：当Git无法自动合并分支时，就必须首先解决冲突。解决冲突后，再提交，合并完成。(可能修改了同个文件)

用`git log --graph`或`git log --graph --pretty=oneline --abbrev-commit`命令可以看到分支合并图

### 9、bug分支

修复bug时，我们会通过创建新的bug分支进行修复，然后合并，最后删除

当手头工作没有完成时，先把工作现场`git stash`一下，然后去修复bug，修复后，再git stash pop，回到工作现场。

多次stash时，可先用git stash list查看，再用`git stash apply stash@{0}`恢复指定stash

### 10、feature（功能）分支

开发一个新feature，最好新建一个分支；

如果要丢弃一个没有被合并过的分支，可以通过`git branch -D <name>`强行删除。

## 五、远程操作

### 1、克隆仓库

​	(1)、克隆仓库：`git clone git@github.com:Garsona/xxxxx.git`
​	(2)、支持多种协议，例如ssh支持的原生git协议、http协议（例如`https://github.com/Garsona/learngit.git`）

### 2、添加远程库

`git remote`：列出所有远程主机

`git remote -v`：参看远程主机网址，结果应为`origin  https://github.com/Garsona/TestProject.git (fetch)`
​                                                                           `origin  https://github.com/Garsona/TestProject.git (push)`

​			    其中origin表示远程主机名，后面是它的网址

`git remote add 主机名 网址`：关联一个远程库

`git remote rm 主机名`：删除远程主机

### 3、合并分支

`git pull <远程主机名> <远程分支名>:<本地分支名>`：取回远程主机某个分支的更新，再与本地的指定分支合并，实际上		    

 等同于先做了`git feach` (更新) 然后做了`git merge` (合并)

### 4、推送

`git push <远程主机名> <本地分支名>:<远程分支名>`：用于将本地分支的更新，推送到远程主机

因此可得，分支推送顺序的写法是<来源地>:<目的地>，所以`git pull`是<远程分支>:<本地分支>，而`git push`是<本地分支>:<远程分支>



十五、多人协作：
1、
首先，可以试图用`git push origin branch-name`推送自己的修改；

如果推送失败，则因为远程分支比你的本地更新，需要先用`git pull`试图合并；

如果合并有冲突，则解决冲突，并在本地提交；

没有冲突或者解决掉冲突后，再用`git push origin branch-xxx`推送就能成功！

如果`git pull`提示“no tracking information”，则说明本地分支和远程分支的链接关系没有创建，用命令`git branch --set-upstream branch-name origin/branch-name`。

2、
（1）查看远程库信息，使用`git remote -v`；

（2）本地新建的分支如果不推送到远程，对其他人就是不可见的；

（3）从本地推送分支，使用`git push origin branch-name`，如果推送失败，先用`git pull`抓取远程的新提交；

（4）在本地创建和远程分支对应的分支，使用`git checkout -b branch-name origin/branch-name`，本地和远程分支的名称最好一致；

（5）建立本地分支和远程分支的关联，使用`git branch --set-upstream branch-name origin/branch-name`；

（6）从远程抓取分支，使用`git pull`，如果有冲突，要先处理冲突。


基本流程：
1、克隆仓库：`git clone git@github.com:Garsona/xxxxx.git`
2、在本地创建和远程分支对应的分支：`git checkout -b branch-name origin/branch-name`
3、在分支上进行文件修改，提交commit
4、从本地推送分支：`git push origin branch-name`
5、倘若出现冲突，即自己与他人修改了同一个文件，应该先设置本地dev与origin/dev链接：`git branch --set-upstream dev origin/dev`,然后进行合并：`git pull`，但合并会有冲突，我们需要在冲突显示出他人与自己的消息中选取有用的信息进行合并，解决冲突，然后将其提交commit，并push到github中：`git push origin dev`

### 5、标签

​	(1)、标签（指向某个commit的指针，与某个commit绑在一起）

命令`git tag <name>`用于新建一个标签，默认为HEAD，也可以指定一个commit id；例如：`git tag v0.1 2857573`

`git tag -a <tagname> -m "blablabla..." id号`可以指定标签信息；

`git tag -s <tagname> -m "blablabla..." id号`可以用PGP签名标签(需安装pgp)；

命令`git tag`可以查看所有标签

​	(2)、操作标签
命令`git push origin <tagname>`可以推送一个本地标签；

命令`git push origin --tags`可以推送全部未推送过的本地标签；

命令`git tag -d <tagname>`可以删除一个本地标签；

删除一个远程标签:  先删除本地标签，再用命令`git push origin :refs/tags/<tagname>`删除远程标签

## 六、push时忽略特殊文件

### 1、创建.gitignore文件

​	（1）在需要创建  .gitignore 文件的文件夹, 右键选择Git Bash 进入命令行，进入项目所在目录。
​	（2） 输入 touch .gitignore(tough后面有空格)在文件夹就生成了一个“.gitignore”文件。然后在.gitignore文件里输入你要忽略的文件夹及其文件就可以了。

### 2

​	将文件强制添加到git：`git add -f xxx`

### 3

​	查找.gitignore对于某个文件类型的编写规则：`git check-ignore -v xxx`，配置文件可以直接在线浏览：https://github.com/github/gitignore

## 七、配置别名

更改某个操作的名称： `git config --global alias.欲改名称 '操作名称'`
配置文件放在.git/config文件中：cat .git/config

