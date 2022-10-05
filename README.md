# GRT网络库
本质是muduo的自我实现及改进版本，因此较多地方直接使用了muduo的源代码或者对其进行了些许的更改  
稍微测试之后发现，效率同muduo库并没有显著的差距
自用网络库，本质还是muduo库，用来自己做做项目之类的
目前些许不关紧要的小BUG，并且没有实现client方面的内容 以后再管

# 主要更改
不再使用boost库，全部用相应的C++11函数取代

改变muduo中onMessageCallback由所在IO线程直接调用的情况
将其分解为三个函数 ：readFunction , computFunction , writeFunction
readFunction 和 writeFunction 由所在IO线程调用 ， computFunction则由本库中自制的threadPool调用和计算  
即更改为了reactors + threadPool的并发模型  
同时用户可以自主设置IO线程和计算线程的数量

使用者无需自己调用对socket的读写，可读取的信息已经存入readFunction的inputBuffer之中，
用户也只要将需要发送的信息传入computFunction和writeFunction的outputBuffer之中即可，socket的读写由网络库自动调用  

对threadPool的BlockingQueue进行了一个小创新（目前未知效率是否有提升，懒得测)  
利用了一个缓冲区分离了pushtask和gettask所调用的queue，push和get之间不需要抢占锁了

thread mutex condition等全部改用C++11 而不是muduo库中自制的那些

用C++11的std::chrono代替了原来muduo中的TimeStamp,并且简化了Timer以及TimerQueue的设计

# 架构图
![9848IBD$3@GVU%QIQ74PGS0](https://user-images.githubusercontent.com/94041901/194054797-139ac03c-d1ab-448f-b2bd-f32e88323dd2.png)

#代码示例
![YISM3 VC`PA77P_U2_F_5LX](https://user-images.githubusercontent.com/94041901/194057492-1aa7dc48-638d-42ae-8d01-9cc6821bc51f.png)

# 计划
修BUG 修BUG 修BUG   
精简代码 精简代码 精简代码  
加入http处理部分  

# 最后
远离socket 远离socket 远离socket  
不要造轮子 不要造轮子 不要造轮子  
