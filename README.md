[![Buildkite](https://badge.buildkite.com/c74bfd00e5543dcb5645acecbfd9d1fc3a0487bc5de13db0b9.svg)](https://buildkite.com/sacha/littleecs)

[![](https://github.com/0-Sacha/littleecs/actions/workflows/linter.yml/badge.svg)](https://github.com/0-Sacha/littleecs/actions/workflows/linter.yml)
[![](https://github.com/0-Sacha/littleecs/actions/workflows/tests_linux.yml/badge.svg)](https://github.com/0-Sacha/littleecs/actions/workflows/tests_linux.yml)
[![](https://github.com/0-Sacha/littleecs/actions/workflows/tests_windows.yml/badge.svg)](https://github.com/0-Sacha/littleecs/actions/workflows/tests_windows.yml)

# LittleECS

LittleECS is a C++20 Entity Component System.
The API is mostly inspired from [entt](https://github.com/skypjack/entt)

I highly encourage you to check out the Workflow [documentation](Docs/Workflow.md) and [example](examples/Workflow/main.cpp) to see what it can do and how to use it.

## Warnings
This project is mostly an educational project of mine that I have started to learn about C++. Some features are not finished to be implemented/tested. I highly not recomend using this for a serious project.
**Do not hesitate to open an issue if you have any suggestions or review to make.**

## Download
```
git clone git@github.com:0-Sacha/littleecs.git
```

## Integration
### [Bazel](https://bazel.build/)
`MODULE.bazel`
```python
git_override(module_name="littleecs", remote="https://github.com/0-Sacha/StreamFormat.git")
bazel_dep(name = "littleecs")
```

`BUILD.bazel`: In your `cc_binary` / `cc_library`
```python
deps = [ "@littleecs//:lecs" ],
```


## Docs / Comments
Like said above, I have begun this project when learning C++, and at the time my projects where absolutely not documented. I am trying to fix this error whenever I got the time. A doc and comments will come one day...

For now, at least, you have the [examples](examples/README.md) (and maybe the tests/) folders to understand what you can do with it. More examples are coming, but again I am trying to do this whenever I have the time to do it.
