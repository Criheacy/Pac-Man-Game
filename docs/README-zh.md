## 《吃豆人》小游戏

一个使用C++的SDL2库实现的吃豆人小游戏。



### 怎么玩？

下载发行版本并运行文件夹中的 `Pac-Man.exe` （对于Linux用户则运行 `Pac-Man` 文件）。

使用键盘上的上下左右方向键来控制角色（即“吃豆人”）。你的目标是在躲避幽灵们的追击的同时吃光屏幕上所有的黄色豆子；同时场地四角还有四颗“大力丸”，它们能在危险关头救你一命。屏幕中央还有短暂出现的特殊食物，在规定时间内吃掉它能获得分数奖励！



### 怎么构建项目？

首先，你的需要先安装 SDL2 和 SDL2-image 库。

对于Linux用户，只需要在终端输入一句 `sudo apt-get install libsdl2-dev libsdl2-image-dev` 即可。

对于Windows和Mac用户来说，你需要从[SDL2官方网站](http://www.libsdl.org/download-2.0.php)下载**开发环境**，然后设置环境变量并在你的IDE中打开项目。SDL2-image库也是同理。安装过程中的细节请参照[Lazy Foo's Tutorial](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php)。



SDL2 和 SDL2-image 库安装完成之后，使用 cmake 来构建（编译+链接）这个项目。

对于Linux用户，在终端中切换至项目工作目录，并输入以下命令：

```
mkdir build
cd build
cmake ../src
```

对于Window和Mac用户，只需要使用IDE来构建项目即可。

