# Example of ProjectCore

### Try thoses examples !

##### Build and Run examples one by one (Recommended)
Example with `//Workflow:Workflow` (see below for the list of example)
- On Windows, using msvc:
    ```
    bazelisk run --config=msvc //Workflow:Workflow
    ```
- On Linux: (This `--config=linux` is not mandatory)
    ```
    bazelisk run --config=linux //Workflow:Workflow
    ```

##### Compile all examples, then run binaries
To compile every example:
- On Windows, using msvc:
    ```
    bazelisk build --config=msvc //...
    ```
- On Linux: (This `--config=linux` is not mandatory)
    ```
    bazelisk build --config=linux //...
    ```

Then you can run a program with (example with `//Workflow:Workflow`, see below for the list of example)
```
.\bazel-bin\Workflow\Workflow
```

### List Of Examples

- `//Workflow:Workflow`: Workflow using the Internal Logger
