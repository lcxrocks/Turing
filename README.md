## 图灵机模拟程序

本程序是用C++实现的一个兼容多平台的命令行工具`turing`，用于模拟多带（确定性、多条无限纸带）图灵机的模拟器。

```has 
输入：turing [-v|--verbose] [-h|--help] <tm> <input>`
输出：第一条纸带运行结束后的非空字符（若为空则输出单个空格字符）
```

其中

- `<tm>`为提前准备好的一个图灵机定义文件，文件定义方式请参考注1
- `<input>`为要在首条纸带上输入的字符串，请在串的两端加上双引号，例：`"your input"`

本模拟器还拥有两种功能：

> `-h|--help`：输出使用方法：`Usage: turing [-v|--verbose] [-h|--help] <tm> <input>`
>
> `-v|--verbose`：逐步模拟输入串的处理过程，并打印在输出流中



### 编译方式

1. 使用文件夹`turing-project`中的`Makefile`即可编译出二进制文件`turing`；

   ``` bash
   $ make
   ```

2. 将当前文件夹加入`PATH`（可选，若不加也可以直接使用`./turing [arguments]`来执行）；

   ```bash
   $ export PATH=$PATH:$(pwd)
   ```

3. 使用`turing [arguments]`来执行程序。

   ```bash
   $ turing [arguments]
   ```

   

### 注：

1. 图灵机文件`<tm>`的定义请参考[这个网站](https://ptc-nju.github.io/#%E9%99%84%E5%BD%95-%E5%9B%BE%E7%81%B5%E6%9C%BA%E7%A8%8B%E5%BA%8F%E8%AF%AD%E6%B3%95)，请严格遵守这里的语法；

2. 该模拟器会检测输入是否合法，若不合法则会在错误流中输出错误信息，错误编号：

   > `[exitcode:0] wrong argument number`：命令行输入参数数量有误，需要按照`-h`的输入格式使用
   >
   > `[exitcode:11]  syntax error --> Illegal state in transition function. Check your <tm> file.`：`<tm>`文件定义有误，转移函数中出现了状态函数未定义的状态；
   >
   > `[exitcode:12] syntax error --> Illegal character in transition function (old_char). Check your <tm> file.`：`<tm>`文件定义有误，转移函数中出现了纸带集合中未定义的字符；
   >
   > `[exitcode:13] syntax error --> Illegal character in transition function (new_char). Check your <tm> file.`：`<tm>`文件定义有误，转移函数中出现了纸带集合中未定义的字符；
   >
   > `[exitcode:14] syntax error --> Illegal definition format. Check your <tm> file.`：`<tm>`文件定义有误，定义方式出现问题（等于号、空格、大括号位置有误）；
   >
   > `[exitcode:2] illegal input`：输入字符串中出现了输入字母表中不存在的符号，使用`-v`选项查看更详细的错误原因