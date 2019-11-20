
# Enrique's C language For Beginners (ECFB Language).

## 1. Introduction

This project was born with two ideas in mind. One, to create a simple-enough-to-learn programming language, based on C/C++ for non-developers to engage into the developer world. Two, to allow me, and developers with more time in the business on how programming languages work, and how they can be done by any developer out there. Sometimes developers tend to learn one language to the fullest without understanding its core, or even thinking how it was made or what happens behind them. Understanding the different parts of a programming language will allow developers not to only have more knowledge on how languages work, but also on how different languages have similar attributes, that could be learnt faster if developers learn the attributes, rather than the language itself.

Of course, this project wasn't made completely without proper knowledge of the tools and the IDE. I used different sources for my research, not only on the explanations of the tools but also in the creation of the language. I will expose them at the end of this readme. For now, let's move to the basic requirements for this project.

## 1. Requirements

For this project, the developers will need to have an IDE that allows to work with C++, and that can connect other tools to the pipeline. These tools are for the creation of the C++ files that will be used to compile this command line tool. The tools in particular are Flex and Bison.

> I should warn the developer, I'm using a Macbook and my IDE is Xcode, as I'm a current iOS developer in duty, and I like to use it for experimental programs with C/C++ rather than using Eclipse-like IDEs. If the developer is coming from Windows, I recommend using Visual Studio since it has great capabilities for developing in C/C++, but the developer will have to create buiild rules for our Flex/Bison files in it's pipeline. For linux, I don't have experience.

### Flex

Flex is the tool that will be used to generate the lexical analyzer, meaning the parser for the tokens that can be inside our language. I won't get into specifics about what Flex is, but rather if the developer is interest on knowing more about it, it can click this [link](https://en.wikipedia.org/wiki/Flex_(lexical_analyser_generator)). For now, lets focus on how to install it. 

To install it in Mac, just run this commands (I'm presumming the developer already has [homebrew](https://brew.sh) inside its computer)

```bash
brew install flex
which flex
flex -V
```

I truly encourage the developer to run the command `flex --help` and see the different values

### Bison

### Why are these tools needed.

If the developer opens the project with Xcode, and goes to the build rules, it will see that there are multiple rules regarding files with extensions `.lex` and `.yacc`. These files will be translated into C++ code ultimately and they will be used in the compiler, but it's always easier to use tools for this kind of projects.


https://www.geeksforgeeks.org/flex-fast-lexical-analyzer-generator/
https://gnuu.org/2009/09/18/writing-your-own-toy-compiler/
https://llvm.org
https://stackoverflow.com/questions/28203937/installing-llvm-libraries-along-with-xcode
https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet
