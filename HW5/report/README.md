# hw5 report

|||
|-:|:-|
|Name|Tony Chu|
|ID|110xxxxxx|

## How much time did you spend on this project
About 25 hours.😵‍💫

## Project overview
This is a project for us to practice the last phase of the construction of compiler, that is, "Code Generator".

In this project, the RISC-V code generator was implemented using the stack machine model. The basic process of the stack machine involves three steps:

1. Retrieve the arguments from the stack.
2. Perform the calculation to obtain the result.
3. Place the calculated result back onto the stack.

The process begins with the parser invoking the visitor to initiate the visitor pattern. Throughout this visiting process, the code is simultaneously exported. Critical aspects to consider include the sequence of visits and determining whether it's an lvalue or rvalue. For lvalues, retrieving the address is essential, whereas for rvalues, extracting the value is required.

And I used this to write the assembly code.
```c++
void CodeGenerator::dumpInstruction(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(this->m_output_file, format, args);
    va_end(args);
}
```

Additionally, it's important to pay attention to other details like address labels and similar elements.
## What is the hardest you think in this project
At first, I don't know why the assembly code cannot be saved under my implementation.

So I modified some code than it can successfully save the files, I was very happy about this. 👍

The bonus part is very hard for me, I only implemented boolean type because I want to take the winter vacation soon 🤣🤣🤣, this was the last thing I have to do in this semester.

And this is my first time implementing assembly code, I am not familiar with it, taking me lots of time to deal with it.

## Feedback to T.A.s
Ok, this is the last assignment of this course, and I think that this assignment is not easy as well.

However, I thought this assignment is easy at the beginning because the spec says it is easy and the professor only said HW3 and HW4 are more difficult, he didn't mentioned hw5. So I started doing this assignment at 1/09, three days before deadline. And this made me very stressful because this is very difficult for me 😖.   

I thought it was a good experience to learn how to write assembly code, but it was a little difficult for me to accomplish all the bonus. I hope TAs can give the solution of the bonus part after the deadline of this homework.

By the way, the `CI / Grade (push)` really saved my score this time. So I recommand HW1, HW2, HW3 should have this grader as well.

The workload of this course is really really really heavy, but through implementation, I can better understand the actual operation of the compiler and get closer to the underlying software. I believe that completing this course will definitely be a great help to my future of software development.

Finally, I would like to express my sincere gratitude to the professor and TAs for their careful course planning. It has been a long time since I have taken such a solid and rewarding course 💯💯.
