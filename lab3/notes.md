# 实验笔记
## 关于 execv 函数
- 参数列表的最后一个指针必须是 NULL（本次实现中数组开得较大后面没有被初始化的元素默认为 NULL）
- 按照惯例，参数列表的第一个参数是执行文件的文件名
- 好好运用 man 指令来指导编程
