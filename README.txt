Problem statement:
User Level Thread Library
For this part you will implement a cooperative User Level Thread (ULT) library for Linux that can replace
the default PThreads libary. Cooperative user level threading is conceptually similar to a concept known as
coroutines, in which a programming language provides a facility for switching execution between different
contexts, each of which has its own independent stack. A very simple program using coroutines might look
like this:
void coroutine1()
{
// Some work
yield(coroutine2);
}
2CS 519 - Homework 0 Due by February 12th, 2015
void coroutine2()
{
// Some different work
if (!done)
yield(coroutine1);
}
int main()
{
coroutine1();
}
Note that cooperative threading is fundamentally different than the preemptive threading done by many
modern operating systems in that every thread *must* yield in order for another thread to be scheduled.
3.1 Basic User Level Thread Library
Write a non-preemptive cooperative user-level thread library that operates similarly to the Linux pthreads
library, implementing the following functions:
• mypthread create
• mypthread exit
• mypthread yield
• mypthread join
Please note that we are prefixing the typical function names with ”my” to avoid conflicts with the standard
library. In this ULT model, one thread yields control of the processor when one of the following conditions
is true:
• thread exits by calling mypthread exit
• thread explicitly yields by calling mypthread yield
• thread waits for another thread to terminate by calling mypthread join
You should use the functionality provided by the Linux functions setcontext(), getcontext(), and swapcontext()
in order to implement your thread library. See the Linux manual pages for details about using these
functions.
3.2 Requirements
We will provide a Makefile, test program, and simple template. You may not modify the test program, so
your library must provide exactly the API that we specify. You should implement all of your code in the
provided files mypthread.h, and mypthread.c.


Solution:
Please refer to Report.pdf