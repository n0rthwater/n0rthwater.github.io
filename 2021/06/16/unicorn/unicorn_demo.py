# python2中使用python3的打印函数
from __future__ import print_function
from unicorn import *
from unicorn.x86_const import *
from pwn import *

# code to be emulated
context(log_level="info",arch="amd64",os="linux")
shellcode = asm("mov eax,0")
print(shellcode)