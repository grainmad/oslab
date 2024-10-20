
# C调用汇编

在C语言中调用汇编函数时，参数通常是通过栈传递的。当一个函数被调用时，系统会为它创建一个栈帧，栈帧中包含了函数的局部变量、参数和返回地址等信息。  

栈帧的典型结构：

栈底（EBP）： 指向当前栈帧的底部。  
返回地址： 调用该函数的指令的下一条指令的地址。  
局部变量： 函数内部定义的局部变量。  
参数： 传递给函数的参数。  
汇编中获取参数的方法  
在汇编代码中，我们可以通过栈指针（ESP）和基址指针（EBP）来访问栈帧中的数据。  

第一个参数通常位于 [ebp+8]，第二个参数位于 [ebp+12]，以此类推。  

在函数返回前，恢复原来的基址指针，并将栈指针调整到函数调用前的状态。
mov esp, ebp
pop ebp



# 汇编调用C

以系统调用fork()为例子

在汇编调用c函数copy_process时，栈顶是copy_process调用完后下一条指令，然后依次是第一个参数，第二个参数...

`call xxx` 会在栈内压入调用完成后的下一个指令，在`xxx`内`ret`则跳转该指令。


``` asm
system_call:
	cmpl $nr_system_calls-1,%eax
	ja bad_sys_call
	push %ds
	push %es
	push %fs
	pushl %edx
	pushl %ecx		# push %ebx,%ecx,%edx as parameters
	pushl %ebx		# to the system call
	movl $0x10,%edx		# set up ds,es to kernel space
	mov %dx,%ds
	mov %dx,%es
	movl $0x17,%edx		# fs points to local data space
	mov %dx,%fs
	call sys_call_table(,%eax,4)
	pushl %eax
	movl current,%eax
	cmpl $0,state(%eax)		# state
	jne reschedule
	cmpl $0,counter(%eax)		# counter
	je reschedule


sys_fork:
	call find_empty_process
	testl %eax,%eax
	js 1f
	push %gs
	pushl %esi
	pushl %edi
	pushl %ebp
	pushl %eax
	call copy_process
	addl $20,%esp

```
压栈情况

中断 int 0x80  
ss esp eflags cs eip   

sys_call 压入  
ds es fs edx ecx ebx sys_fork调用后跳转位置  

sys_fork  
gs esi edi ebp eax copy_process调用后跳转位置  

``` c
int copy_process(int nr,long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx,
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss)
{
}
```

