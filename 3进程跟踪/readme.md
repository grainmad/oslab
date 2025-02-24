
main.c   
`fork()`前打开`/var/process.log`


fprintk.c
内核级文件打印 

N、J、R、W 和 E 中的任意一个，分别表示进程新建(N)、进入就绪态(J)、进入运行态(R)、进入阻塞态(W) 和退出(E)；
`fprintk(3, "%ld\t%c\t%ld\n", current->pid, 'R', jiffies);`

kernel.h
添加函数fprintk声明


## 打印进程状态变化 
寻找所有p->state变化的文件，全局搜索`->state`

sched.c
    schedule()

exit.c

fork.c
添加打印语句

为什么fork() == 0 就创建成功?

```
jmp 指令
无条件跳转： jmp 指令会直接将程序的执行流跳转到指定的地址，没有任何额外的操作。
不保存返回地址： 由于是无条件跳转，jmp 指令不会保存当前指令的下一条指令的地址，因此无法直接返回到跳转前的指令处。
用途：
实现循环
跳转到程序的特定部分
错误处理
call 指令
调用子程序： call 指令用于调用子程序（函数）。
保存返回地址： 在跳转到子程序之前，call 指令会将当前指令的下一条指令的地址（即返回地址）压入栈中。
子程序返回： 子程序执行完毕后，使用 ret 指令从栈中弹出返回地址，从而返回到调用处。
用途：
实现模块化编程
复用代码
```


## 结果

``` cpp
for (i = 0; i < 10; i++) {
    proc_id_arr[i] = fork();

    if (proc_id_arr[i] < 0) {
        printf("Failed to fork child process %d!\n", i + 1);
        return -1;
    } else if (proc_id_arr[i] == 0) { 
        cpuio_bound(40, 10-i, i);
        return 0;
    } else {    
        ;
    }
}
```

进程 15～24

```
(Unit: tick)
Process   Turnaround   Waiting   CPU Burst   I/O Burst
      0        35528        67           8         468
      1           26         1           1          24
      2           24         4          20           0
      3         3004         0           4        2999
      4        35720         3          68       35649
      5            3         0           2           0
      6            7         0           6           0
      7            4         0           3           0
      8          280         1          12         267
      9           53         0          52           0
     10          106         0         106           0
     11           30         0          29           0
     12           81         1          80           0
     13            3         1           2           0
     14        22011         1          10       22000
     15        22008     18007        4000           0
     16        21742     17722        3600         420
     17        21091     17053        3200         837
     18        20017     15949        2800        1267
     19        18804     14732        2400        1672
     20        17132     13027        2000        2104
     21        14742     10601        1600        2540
     22        12722      8570        1200        2952
     23         9971      5781         800        3389
     24         6867      2647         400        3819
     25            4         1           3           0
     26            1         0           0           0
Average:     9703.00   4598.85
Throughout: 0.08/s
```

15~24
10个相同io和cpu执行时间的进程，不同时间片

``` cpp
int main(int argc, char * argv[])
{	
	int i;
	int t;
	int proc_id_arr[10];     
    for (i = 0; i < 10; i++) {
        proc_id_arr[i] = fork();

        if (proc_id_arr[i] < 0) {
            printf("Failed to fork child process %d!\n", i + 1);
            return -1;
        } else if (proc_id_arr[i] == 0) { 
			printf("%d process priority %d\n", i+1, nice(i)); /*sys_nice 优先级减少i，默认初始15 返回值被修改为当前优先级*/
            cpuio_bound(40, 5, 5);
            return 0;
        } else {    
            ;
        }
    }

    for (i = 0; i < 10; i++) {
        printf("Child PID: %d\n", proc_id_arr[i]);
    }
	for (i = 0; i < 10; i++) {
        t=waitpid(proc_id_arr[i], NULL, 0); /*有10个子进程需要等待*/
		printf("Child PID: %d, ws: %d\n", proc_id_arr[i], t); 
    }

	return 0;
}
```

```
(Unit: tick)
Process   Turnaround   Waiting   CPU Burst   I/O Burst
      0        38405        67           8         453
      1           26         1           1          24
      2           24         4          20           0
      3         3004         0           4        2999
      4        38895         9          68       38818
      5            3         0           2           0
      6          276         1          14         261
      7           52         0          51           0
      8          104         0         103           0
      9           29         0          28           0
     10           80         1          79           0
     11            4         0           3           0
     12            2         0           1           0
     13            4         0           3           0
     14        20586         1          10       20575
     15        14872     10764        2000        2107
     16        15520     11387        2000        2133
     17        16295     12187        2000        2107
     18        16961     12865        2000        2095
     19        17803     13703        2000        2100
     20        18489     14402        2000        2087
     21        19138     15014        2001        2122
     22        19686     15548        2000        2138
     23        20016     15898        2000        2117
     24        20577     16491        2000        2085
     25            4         0           3           0
     26            1         0           0           0
Average:    10402.07   5123.81
Throughout: 0.07/s

```

截取10个进程初期运行结果，分析。  
14号进程是父进程，以此创建15～24号进程。期间没有发生调度。  
随后由于wait子进程，15号阻塞。  
此时就绪队列中15～24号时间片都是15  
schedule()中由于从后向前遍历任务数组，获取到的就是24。  
24号执行时，修改了优先级从15变为9。  
我们从图中发现，每个进程第一次执行的时间片是一致的。因为优先级是在进程刚运行时修改的，只有再次调度时间片才会变化。  
注意到15号进程第一次运行时间很久，估计是调度时再次调度到了15号。  
此时10个进程的各自优先级决定了时间片的最大值。后续的运行时间片明显不同。  


``` cpp
 7067 #               +4                                                                                          
 7068 #               #4                                      -14                                                 
 7069 #               :4                                      #14                                                 
 7070 #               :                                       #   -15                                             
 7071 #               :                                       #   |15 -16 -17                                     
 7072 #               :                                       #   |   |16 |17 -18                                 
 7073 #               :                                       #   |   |   |   |18 -19                             
 7074 #               :                                       #   |   |   |   |   |19 -20 -21                     
 7075 #               :                                       #   |   |   |   |   |   |20 |21 -22 -23             
 7076 #               :                                       #   |   |   |   |   |   |   |   |22 |23 -24         
 7077 #               :                                       #   |   |   |   |   |   |   |   |   |   |24         
 7078 #               :                                       :14 |   |   |   |   |   |   |   |   |   #24         
 7079 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7080 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7081 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7082 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7083 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7084 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7085 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7086 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7087 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7088 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7089 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7090 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7091 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7092 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7093 #               :                                       :   |   |   |   |   |   |   |   |   #23 |24         
 7094 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7095 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7096 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7097 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7098 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7099 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7100 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7101 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7102 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7103 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7104 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7105 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7106 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7107 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7108 #               :                                       :   |   |   |   |   |   |   |   #22 |23 |           
 7109 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7110 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7111 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7112 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7113 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7114 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7115 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7116 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7117 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7118 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7119 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7120 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7121 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7122 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7123 #               :                                       :   |   |   |   |   |   |   #21 |22 |   |           
 7124 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7125 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7126 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7127 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7128 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7129 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7130 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7131 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7132 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7133 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7134 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7135 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7136 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7137 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7138 #               :                                       :   |   |   |   |   |   #20 |21 |   |   |           
 7139 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7140 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7141 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7142 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7143 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7144 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7145 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7146 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7147 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7148 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7149 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7150 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7151 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7152 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7153 #               :                                       :   |   |   |   |   #19 |20 |   |   |   |           
 7154 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7155 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7156 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7157 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7158 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7159 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7160 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7161 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7162 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7163 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7164 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7165 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7166 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7167 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7168 #               :                                       :   |   |   |   #18 |19 |   |   |   |   |           
 7169 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7170 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7171 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7172 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7173 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7174 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7175 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7176 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7177 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7178 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7179 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7180 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7181 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7182 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7183 #               :                                       :   |   |   #17 |18 |   |   |   |   |   |           
 7184 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7185 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7186 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7187 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7188 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7189 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7190 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7191 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7192 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7193 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7194 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7195 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7196 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7197 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7198 #               :                                       :   |   #16 |17 |   |   |   |   |   |   |           
 7199 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7200 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7201 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7202 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7203 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7204 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7205 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7206 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7207 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7208 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7209 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7210 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7211 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7212 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7213 #               :                                       :   #15 |16 |   |   |   |   |   |   |   |           
 7214 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7215 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7216 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7217 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7218 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7219 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7220 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7221 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7222 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7223 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7224 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7225 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7226 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7227 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7228 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7229 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7230 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7231 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7232 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7233 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7234 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7235 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7236 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7237 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7238 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7239 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7240 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7241 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7242 #               :                                       :   #   |   |   |   |   |   |   |   |   |           
 7243 #               :                                       :   |15 #16 |   |   |   |   |   |   |   |           
 7244 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7245 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7246 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7247 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7248 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7249 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7250 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7251 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7252 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7253 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7254 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7255 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7256 #               :                                       :   |   #   |   |   |   |   |   |   |   |           
 7257 #               :                                       :   |   |16 #17 |   |   |   |   |   |   |           
 7258 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7259 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7260 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7261 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7262 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7263 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7264 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7265 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7266 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7267 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7268 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7269 #               :                                       :   |   |   #   |   |   |   |   |   |   |           
 7270 #               :                                       :   |   |   |17 #18 |   |   |   |   |   |           
 7271 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7272 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7273 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7274 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7275 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7276 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7277 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7278 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7279 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7280 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7281 #               :                                       :   |   |   |   #   |   |   |   |   |   |           
 7282 #               :                                       :   |   |   |   |18 #19 |   |   |   |   |           
 7283 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7284 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7285 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7286 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7287 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7288 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7289 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7290 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7291 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7292 #               :                                       :   |   |   |   |   #   |   |   |   |   |           
 7293 #               :                                       :   |   |   |   |   |19 #20 |   |   |   |           
 7294 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7295 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7296 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7297 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7298 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7299 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7300 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7301 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7302 #               :                                       :   |   |   |   |   |   #   |   |   |   |           
 7303 #               :                                       :   |   |   |   |   |   |20 #21 |   |   |           
 7304 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7305 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7306 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7307 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7308 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7309 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7310 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7311 #               :                                       :   |   |   |   |   |   |   #   |   |   |           
 7312 #               :                                       :   |   |   |   |   |   |   |21 #22 |   |           
 7313 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7314 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7315 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7316 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7317 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7318 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7319 #               :                                       :   |   |   |   |   |   |   |   #   |   |           
 7320 #               :                                       :   |   |   |   |   |   |   |   |22 #23 |           
 7321 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7322 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7323 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7324 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7325 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7326 #               :                                       :   |   |   |   |   |   |   |   |   #   |           
 7327 #               :                                       :   |   |   |   |   |   |   |   |   |23 #24         
 7328 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7329 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7330 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7331 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7332 #               :                                       :   |   |   |   |   |   |   |   |   |   #           
 7333 #               :                                       :   #15 |   |   |   |   |   |   |   |   |24     
```