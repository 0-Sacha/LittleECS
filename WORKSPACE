""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_utilities",
    strip_prefix = "bazel_utilities-d748d078d0bdd4e5fe9020e8011a46edb484525e",
    urls = [ "https://github.com/0-Sacha/bazel_utilities/archive/d748d078d0bdd4e5fe9020e8011a46edb484525e.zip" ],
    integrity = "sha256-qakyhHoLPrGK/3zD+dWIL7wGSjLwMX69TiSGA05Wu2c=",
)

http_archive(
    name = "bazel_buildbuddy",
    strip_prefix = "bazel_buildbuddy-89937f1942a9271348c0963ef89fb67c240c3afd",
    urls = [ "https://github.com/0-Sacha/bazel_buildbuddy/archive/89937f1942a9271348c0963ef89fb67c240c3afd.zip" ],
    integrity = "sha256-M8sEkfNBjSivQhIRYRzCXIq7ix3w23UckP0z9T/L3II=",
)

load("@bazel_buildbuddy//:rules.bzl", "buildbuddy_toolchain")

buildbuddy_toolchain(
    name = "buildbuddy_test",
    auto_register_toolchain = False
)
