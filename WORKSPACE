""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_utilities",
    strip_prefix = "bazel_utilities-efe15a48c3b063436d6deecaa14b811083035993",
    urls = [ "https://github.com/0-Sacha/bazel_utilities/archive/efe15a48c3b063436d6deecaa14b811083035993.zip" ],
    integrity = "sha256-g1BUGaQFSLywkzTqixQdQTdrJ25TlcqG6PZXoCK7Jrw=",
)

http_archive(
    name = "bazel_buildbuddy",
    strip_prefix = "bazel_buildbuddy-b0840cf378c4aef5750b5e8bc87aa2ad1aeb874f",
    urls = [ "https://github.com/0-Sacha/bazel_buildbuddy/archive/b0840cf378c4aef5750b5e8bc87aa2ad1aeb874f.zip" ],
    integrity = "sha256-Wvbu4LB7F3d42Xj/dQyaGjVEntf4U3DO+aku3zEZD4I=",
)

load("@bazel_buildbuddy//:rules.bzl", "buildbuddy_toolchain")

buildbuddy_toolchain(
    name = "buildbuddy_test",
    auto_register_toolchain = False
)
