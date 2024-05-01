[![Build status](https://badge.buildkite.com/0cf82389a7a3436a623ce9a61e15bdd701d29c73ffbb6ecaf0.svg)](https://buildkite.com/sacha/littleecs)
[![LittleECS](https://github.com/0-Sacha/LittleECS/actions/workflows/LittleECS.yml/badge.svg)](https://github.com/0-Sacha/LittleECS/actions/workflows/LittleECS.yml)

# LittleECS

LittleECS is a C++ Entity Component System.
The API is mostly inspired from [entt](https://github.com/skypjack/entt)

I highly encourage you to check out the [documentation](Docs/Workflow.md) and [example](Examples/Workflow/main.cpp) to see what it can do and how to use it.

## Warnings
This project is mostly an educational project of mine that I have started to learn about C++. Some features are not finished to be implemented/tested. I highly not recomend using this for a serious project.
**Do not hesitate to open an issue if you have any suggestions or review to make.**

## Docs / Comments
Like said above, I have begun this project when learning C++, and at the time my projects where absolutely not documented. I am trying to fix this error whenever I got the time. A doc and comments will come one day...

For now, at least, you have the [Examples](Examples/README.md) (and maybe the Tests/) folders to understand what you can do with it. More examples are coming, but again I am trying to do this whenever I have the time to do it.

## Download
```
git clone git@github.com:0-Sacha/LittleECS.git
```

## Using the Lib
It can be used using [Bazel](https://bazel.build/).
A `cc_library` rule has been created: `@LittleECS//:LittleECS`.
You need to add the module `littleecs` to your dependencies.

You will need at least to give an c++ standard to use (at least c++20) since no one have been forced. You can check the [bazelrc](.bazelrc) to see how you can add one.

Not Recommended: There is also a [Premake](https://premake.github.io/docs/using-premake) configuration, thought it is deprecated (and run on a wrapper of mine: [PremakeUtilities](https://github.com/0-Sacha/PremakeUtilities)). I keep it for my Game Engine [Blackbird](https://github.com/0-Sacha/Blackbird) which is using `Premake` as Build system.
