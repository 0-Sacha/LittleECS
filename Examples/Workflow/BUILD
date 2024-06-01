""

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "Workflow",
    srcs = glob([ "*.h", "*.cpp" ]),
    defines = [ "LECS_USE_PROJECTCORE" ],
    copts = select({
        "@rules_cc//cc/compiler:msvc-cl": ["/std:c++20"],
        "//conditions:default": ["-std=c++20"],
    }),
    deps = [ "@ProjectCore//:ProjectCore", "@littleecs//:LittleECS" ],
    visibility = ["//visibility:public"],
)
