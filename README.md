[![Build status](https://badge.buildkite.com/0cf82389a7a3436a623ce9a61e15bdd701d29c73ffbb6ecaf0.svg)](https://buildkite.com/sacha/littleecs)
[![LittleECS](https://github.com/0-Sacha/LittleECS/actions/workflows/LittleECS.yml/badge.svg)](https://github.com/0-Sacha/LittleECS/actions/workflows/LittleECS.yml)

# LittleECS

LittleECS is a C++ Entity Component System.
The API is mostly inspired from [entt](https://github.com/skypjack/entt)

I highly encourage you to check out the Workflow [documentation](Docs/Workflow.md) and [example](examples/Workflow/main.cpp) to see what it can do and how to use it.

## Warnings
This project is mostly an educational project of mine that I have started to learn about C++. Some features are not finished to be implemented/tested. I highly not recomend using this for a serious project.
**Do not hesitate to open an issue if you have any suggestions or review to make.**

## Download
```
git clone git@github.com:0-Sacha/LittleECS.git --recurse-submodules -c core.symlinks=true
```

## Using the Lib
It can be used using [Bazel](https://bazel.build/).
A `cc_library` rule has been created: `@littleecs//:littleecs`.
You need to add the module `littleecs` to your dependencies.


## Docs / Comments
Like said above, I have begun this project when learning C++, and at the time my projects where absolutely not documented. I am trying to fix this error whenever I got the time. A doc and comments will come one day...

For now, at least, you have the [examples](examples/README.md) (and maybe the tests/) folders to understand what you can do with it. More examples are coming, but again I am trying to do this whenever I have the time to do it.
