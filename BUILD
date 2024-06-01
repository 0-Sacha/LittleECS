""

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "LittleECS",
    srcs = glob([ "src/**/*.h" ]),
    hdrs = glob([ "src/**/*.h" ]),
    includes = [ "src/" ],
    copts = select({
        "@rules_cc//cc/compiler:msvc-cl": ["/std:c++20"],
        "//conditions:default": ["-std=c++20"],
    }),
    strip_include_prefix = "src",
    include_prefix = "LittleECS",
    linkstatic = True,
    visibility = ["//visibility:public"],
)

cc_test(
    name = "LittleECSTests",
    srcs = glob([ "Tests/**/*.h", "Tests/**/*.cpp" ], exclude=["Tests/Perf/**"]),
    includes = [ "src/" ],
    defines = [ "LECS_USE_PROJECTCORE" ],
    copts = select({
        "@rules_cc//cc/compiler:msvc-cl": ["/std:c++20"],
        "//conditions:default": ["-std=c++20"],
    }),
    deps = [ "@ProjectCore//:ProjectCore", ":LittleECS" ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "LittleECSTestsPerf",
    srcs = glob([ "Tests/**/*.h", "Tests/**/*.cpp" ]),
    includes = [ "src/" ],
    defines = [ "LECS_USE_PROJECTCORE" ],
    copts = select({
        "@rules_cc//cc/compiler:msvc-cl": ["/std:c++20"],
        "//conditions:default": ["-std=c++20"],
    }),
    deps = [ "@ProjectCore//:ProjectCore", ":LittleECS" ],
    visibility = ["//visibility:public"],
)
