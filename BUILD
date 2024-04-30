""

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "LittleECS",
    srcs = glob([ "src/**/*.h" ]),
    hdrs = glob([ "src/**/*.h" ]),
    includes = [ "src/" ],
    strip_include_prefix = "src",
    include_prefix = "LittleECS",
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_test(
    name = "LittleECSTests",
    includes = [ "src/" ],
    srcs = glob([ "Tests/**/*.h", "Tests/**/*.cpp" ], exclude=["Tests/Perf/**"]),
    defines = [ "LECS_USE_PROJECTCORE" ],
    deps = [ "@ProjectCore//:ProjectCore", ":LittleECS" ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "LittleECSTestsPerf",
    includes = [ "src/" ],
    srcs = glob([ "Tests/**/*.h", "Tests/**/*.cpp" ]),
    defines = [ "LECS_USE_PROJECTCORE" ],
    deps = [ "@ProjectCore//:ProjectCore", ":LittleECS" ],
    visibility = ["//visibility:public"],
)
