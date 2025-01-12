# eda 

## 内容

- '组合逻辑环路分析':赛题指南
- 'eda_problem.cpp':赛题一到四源代码
- 'text.txt':输入格式示例
- '2loop_test':双环测试样例，包含以下内容:

  - '2loop_test1.txt':一个负反馈环的双环SCC
  
  - '2loop_test1.png':2loop_test1.txt的对应电路

  - '2loop_test2.txt':两个负反馈环的双环SCC

  - '2loop_test2.png':2loop_test2.txt对应的电路图

- '3loop_test':三环测试样例，其中文件命名规则与2loop_test相同
- 'module_info':官方测试样例
- 'result.xlsx':样例测试用时结果
- 'q5':赛题五相关内容
- 'report':project 报告

## eda_problem.cpp 用法

eda_problem.cpp编译好后，在命令行中输入

./eda_problem.exe  <test.txt>

其中<test.txt>为测试样例路径，格式与文件夹之中test.txt相同即可

## 测试文件格式

module name is combLogic

0 : gate1 output1 input11 input12

1 : gate2 output2 input21 input22

2 : gate3 output3 input31 input32

……

其中，gate为逻辑门名称，output为门输出信号线名称，input为门输入信号线名称（非门只有一个输入）


## 输出

- result1.txt至result4.txt为一至四题对应的输出
- 命令行内将会输出对应的用时
