<a name="logo">
<div align="center">
<img src="repo-assets/logo.svg" alt="Logo" height = "140px" width="200px"></img>
<br><br>

<img src="https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge" alt="license"></img>

<img src="https://img.shields.io/github/v/release/sid200/protonium?include_prereleases&style=for-the-badge" alt="version"></img>

<a href="https://sid200.github.io/Protonium/" target="_blank">
<img src="https://img.shields.io/website?down_message=Offline&label=DOCS&style=for-the-badge&up_message=Online&url=https%3A%2F%2Fsid200.github.io%2FProtonium%2F" alt="docs"></img>
</a>
</div>
</a>

# Protonium

*A simple language for no reason.*

### 👋 Hello World!

It's as simple as it can get:

```ts
print("Hello World!");
println("Hello World!");    //with a new line at the end
```

## The source

> The current design makes uses a tree walk interpreter inspired by jlox from [Crafting Interpreters](https://github.com/munificent/craftinginterpreters). The tree walk interpreter is a temporary design and will be replaced in the future.

### 🛠️ Building

The current system utilizes **Premake 5** to help make the building process a breeze. Premake is a command line tool which can generate project files and build configurations. You can get it [here](https://premake.github.io/download).

> Premake 5 uses the `premake5.lua` file in the project's root. So make sure you don't rename it or mess with it.

Depending on the target system go ahead and provide an action:

```sh
premake5 <action>
```

For example, on Windows it's recommended that you use Visual Studio 2019. To generate the solution, use `vs2019` as an action. Then you can simply open the solution and build the project.

On Linux, you could go for GNU Make which uses `gmake2` as an action to generate a corresponding `Makefile`. To use this `Makefile` and build the project, simply call `make`.

## ℹ️ The Language

> This is just a simple reference, and a proper documentation is currently in the works.
>
> Many features are in their primitive state and the language might end up drastically different in the future.
> Expect major changes to come about until a stable release comes out. Things marked with `***` are more prone to change. Some are probably being worked upon while you're reading this.

### A note about statements and expressions `***`

All statements must end with a semi-colon. Implicit semi-colons or something similar might be in the language soon.

In the REPL, however, if a statement is an expression statement and if it can be printed, the semi-colon can be omitted to print what the expression evaluates to.

### Type System `***`

Protonium currently supports six *types*:

- The numeric type referred to as `num_t`
- Boolean types (`true` and `false`) referred to as `bool_t`.
- The string type referred to as `string_t`. This covers both strings and characters. Strings are enclosed in double quotes.
- The null type referred to as `nix_t`. Only `nix` has a type of `nix_t`.
- The list type referred to as `list_t`. A list is a homogenous collection of elements.
- Callable types referred to as `callable_t`.

> More types are planned to come in the future.

> These reference names aren't used anywhere yet, but these are the *official* names.

### Variables

Variable (and function) names follow the C-style naming rules: names can contain any number alpha-numeric characters and underscores, but cannot start with a numeric character. `***`

Assignment supports implicit declaration. Redefinition is allowed. `***`

```ts
a = 3;
b = a^3;
b += 1;

name = "Protonium"; //name is a string
name = 2; //name is now a number

list = [1, 2, 3]; //this is a list
list2 = [[1, 2, 3], [4, 5, 6]]; //this is also a list
list3 = [[1, 2, 3, 4, 5, 6], [3, 4]]; //lists can be non-uniform
```

### Functions

Functions are first-class citizens. They follow the same naming rules as variables. Protonium is a Lisp-1 language which means that variables and functions are stored in the same namespace. Use the `fn` keyword to make a function and use `return` to well, return some value from the function:

```ts
fn myFunc(){
    //code
}
```

Anonymous functions are also allowed:

```ts
fn callWith2(g){
    return g(2);
}

callWith2(fn (a){
    return a^2;
}); //4
```

If a function doesn't return anything, it returns `nix` implicitly:

```ts
fn b(){}

print(b()); //nix
```

### Control Flow

A non-boolean is implicitly converted to a boolean where a `bool_t` is expected based on the following rules:

- Non-zero numbers always evaluate to true
- `nix` evaluates to false.

#### Conditional Control Flow

Use the keywords `if` and `else` for conditional control flow:

```ts
bool = true;
if(bool == true) {
    print("bool was true!");
}
else if(bool == false) {
    print("Hmmm, I wonder how bool became false.");
}
else {
    print("How in the world did a boolean have a value other than true and false?");
}
```

The conditions can be shortened as the value is evaluated implicitly and checked for *trueness*:

```ts
bool = true;
if(bool) {
    print("bool was true!");
}
else if(bool) {
    print("Hmmm, I wonder how bool became false.");
}
else {
    print("How in the world did a boolean have a value other than true and false?");
}
```

#### Loops

Protonium supports `while` and `for` loops. `break` and `continue` statements are also supported.

```ts
num = 2;
while(num < 100){
    num+=1;
    println(num);
}
```

```ts
for(num=2;num<100;num+=1){
    println(num);
}
```

Range based for-loops are also supported:

```ts
for (i in 1..10) {
    println(i^2);
    //1
    //4
    //9
    //16
    //25
    //36
    //49
    //64
    //81
    //100
}
```

### Scopes `***`

Any *block* introduces a new scope:

```ts
a = "I am a global variable.";
{
    a = "I am a local variable.";
    println(a); //the local variable
}
```

A variable created with the same name as the one in an outer scope *shadows* the outer variable. You might wonder how we would assign to a variable in an outer scope instead of making a new one in an inner scope. To do so, use the **strict assignment** (`` `= ``) operator:

```ts
a = 3;
b = 3;
{
    a = 4; //creates a new variable; no way to access the outer variable now.
    b `= 4; //Okay, find the nearest variable named 'b' and assign 4 to it.
}
```

The normal assignment (the **lazy assignment** operator) can implicitly make variables if they don't exist in the current scope. The **strict assignment** operator `***`, however, doesn't have that power. It searches for the *nearest* (in terms of scopes) variable to assign to, and if it doesn't find one, we have an error.

> The strict assignment operator is a temporary workaround and won't be part of the final, stable language.

In case of `for` loops, the variable that's created in the initializer is in the same scope as the block or statement for the loop. So in this loop:

```ts
for(i=0;i<10;i+=1){
    if(i==1) i = 3;
    print(i);
}
//03456789
```
