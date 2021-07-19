
# Enrique's C language For Beginners (ECFB Language).

## Introduction

This project was born with two ideas in mind. One, to create a simple-enough-to-learn programming language, based on C/C++ for non-developers to engage into the developer world. Two, to allow me, and developers with more time in the business on how programming languages work, and how they can be done by any developer out there. Sometimes developers tend to learn one language to the fullest without understanding its core, or even thinking how it was made or what happens behind them. Understanding the different parts of a programming language will allow developers not to only have more knowledge on how languages work, but also on how different languages have similar attributes, that could be learnt faster if developers learn the attributes, rather than the language itself.

Of course, this project wasn't made completely without proper knowledge of the tools and the IDE. I used different sources for my research, not only on the explanations of the tools but also in the creation of the language. I will expose them at the end of this readme. For now, let's move to the basic requirements for this project.

## Requirements

For this project, the developers will need to have an IDE that allows to work with C++, and that can connect other tools to the pipeline. These tools are for the creation of the C++ files that will be used to compile this command line tool. The tools in particular are Flex, Bison and LLVM. The tools for creating the lexicon and grammar are Flex and Bison respectively. LLVM is a library that will help us to create the intermediate code to be compiled. We're not going to use assembly directly, but instead we will use a library to create code in another language than ECFB.

> I should warn the developer, I'm using a Macbook and my IDE is Xcode, as I'm a current iOS developer in duty, and I like to use it for experimental programs with C/C++ rather than using Eclipse-like IDEs. If the developer is coming from Windows, I recommend using Visual Studio since it has great capabilities for developing in C/C++, but the developer will have to create build rules for our Flex/Bison files in it's pipeline. For linux, I don't the have experience to give an advice.

### Flex

Flex is the tool that will be used to generate the lexical analyzer, meaning the parser for the tokens that can be inside our language. I won't get into specifics about what Flex is, but rather if the developer is interest on knowing more about it, it can click this [link](https://en.wikipedia.org/wiki/Flex_(lexical_analyser_generator)). For now, lets focus on how to install it. 

To install it in Mac, just run these commands (I'm presumming the developer already has [homebrew](https://brew.sh) inside its computer)

```bash
brew install flex
which flex
flex -V
```

I truly encourage the developer to run the command `flex --help` and see the different options the developer can use.

### Bison

Having the lexical tokens is not enough, the compiler needs to establish its grammar rules and functionality behind every rule. For that we use a tool called Bison. Bison takes a similar approach as Flex when parsing the grammar rules, except it's file, a `.yacc` file (selected by this project, normally it would be a `.y` file) and it has more sections in it, besides the include header for the code, and the rules. These sections will help us establishing the tokens for the terminal rules, the types for the non terminal rules and the precedence for some operators.

To install it in Mac, just run these commands.

```bash
brew install bison
which bison
bison -V
```
Same as Flex, I truly encourage the developer to run  the command `bison --help` and see the different options the developer can use.

### LLVM

LLVM is our final goal to approach. LLVM is not an acronym per se, but its the name of the project. It works as a compiler and a toolchain that allows the development of languages that are independent of the operative system. The LLVM project will help us to translate the semantic ASL tree into code to be executed by the JIT compiler. If the developer wants to learn more about it, I recommend to go and read the documentation by going to the website (llvm.org)[https://llvm.org]. For now we will only focus on installing it and start working on it.

To install, simply run these bash commands.

```bash
brew install llvm
which llvm-config

echo 'export PATH="/usr/local/opt/llvm/bin:$PATH"' >> ~/.zshrc
export CPPFLAGS="-I/usr/local/opt/llvm/include"
export LDFLAGS="-L/usr/local/opt/llvm/lib"
```

> The exports here will be suggested at the end of installation. I truly recommend the developer to follow them

> The installation will take at least an hour, or an hour and a half. My computer is Macbook from 2018, and still, it took an hour and a half.

> The project might give you an error in a header file inside llvm include folder. Simply modify it directly. The class is PreservedAnalysis, and it should be Preservedanalysis. It might be and issue with the version I got, which is 9.0.0_1

It should be interest to note that the project already knows which c++ flags and which linker flags to use, but if the developer is interest to know, just go to the build settings and look for c++ flags and other linker flags. The developer might get interest to see what libraries we're linking. Also, there's one library that we're getting directly in the build phases, which is `libtermcap.tbd`. If the developer wants to find more about the libraries, it can go to the terminal and use the command line tool `llvm-config`.

```bash
llvm-config --libs core mcjit native --cxxflags --ldflags # to see all the flags that this project needs
```

### Why are these tools needed.

If the developer opens the project with Xcode, and goes to the build rules, it will see that there are multiple rules regarding files with extensions `.lex` and `.yacc`. These files will be translated into C++ code ultimately and they will be used in the compiler, but it's always easier to use tools for these kind of projects.

Also, the use of LLVM allows the developer to get the same project and use it in different environments, and not only in Unix based operative systems.

## The language

The programming language syntax for this compiler is very similar to a C like program, but very restrictive at this moment. For now the only things it can build are

* Integer, double  and boolean instances (boolean d = false, int x = 2 or double y = 2.0)
* Simple binary operations (+, -, *, /, % )
* Simple binary comparisons between numbers (<, <=, >, >=, ==, !=)
* And/Or operations (&&, ||)
* Simple unary operators (-, !)
* Functions (int test(...) { ... return x } or void test2()  )
* The use of the funtion echoi (echoi(x + 2)), echod (echod(y + 2.4), or echob (echob(false)))
* conversions between doubles and ints
* strings and printf, gets functions
* If and While statements

```c
int a = 2

int test(int x) {
    int y = x + a
    int z = y * x + 2
    int ww
    ww = z * -2
    double dd = ww
    string s = "The result is %f"
    printf(s, dd)
    return (int)dd % 7
    
}

void test2(string t) {
    printf(t)
    //gets()
    double de = 2.4 + 3 - 4.2 + 5
}

boolean b = ((4 + 3) > (2.3 - 4.6)) && (3 > 4)
boolean c = b && true
boolean dee = c || false
if (b) {
    printf("B is true")
    int x = 100
    echoi(x)
} else {
    printf("B is false")
    if (c) {
        printf("C is true")
    } else {
        printf("C is false")
    }
}
echob(!b)
echob(c)
echob(dee)
echoi(-1)
echod(-4.5)
echob(true)
echob(true != false)
echoi(test(5.5))

int xx = 0

while (xx < 10) {
    if ((xx % 2) == 0) {
        printf("Prime")
    } else {
        printf("Not Prime")
    }
    xx = xx + 1
}

test2("Good bye")
```

The results of this code after compilation and execution.

```bash

Generating code...
Running code...
B is false
C is false
True
False
False
-1
-4.500000
True
True
The result is -98.000000
0
Prime
Not Prime
Prime
Not Prime
Prime
Not Prime
Prime
Not Prime
Prime
Not Prime
Good bye
```

The project is taking the file inside the examples folder. This file is being passed in the arguments array. This value can be modified by going to the scheme and edit it. Just select the scheme -> Edit scheme -> Run -> Arguments.

This version is the first one with the basics of a good programming language that can explain, even a student with no previous experience, the notions of how a good structural programming language works. It does not have more intermediate stuff such as breaking and continuing in loops, switch statements, structs and classes. That's the job of a more elaborate language.

**NOTE:** The developer can see the debug printout by going to the `main.cpp` file and uncomment the line `#define DEBUGPRINT`.

## References

* https://llvm.org
* https://gnuu.org/2009/09/18/writing-your-own-toy-compiler/
* https://www.geeksforgeeks.org/flex-fast-lexical-analyzer-generator/
* https://aquamentus.com/flex_bison.html
* https://github.com/Virtual-Machine/llvm-tutorial-book


