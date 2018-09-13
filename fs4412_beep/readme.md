基于三星f4412开发板的pwm驱动程序

在ARM机insmod fs4412_beep.ko（有些kernel不能自动创建文件节点，需要mknod /dev/fs4412 c 253 0）

交叉编译test.c 测试程序

在ARM机运行./a.out 蜂鸣器一响一灭




























