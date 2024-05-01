""

load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "Workflow",
    srcs = glob([ "*.h", "*.cpp" ]),
    defines = [ "LECS_USE_PROJECTCORE" ],
    deps = [ "@ProjectCore//:ProjectCore", "@littleecs//:LittleECS" ],
    visibility = ["//visibility:public"],
)
