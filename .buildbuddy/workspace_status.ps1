# This script will be run by Bazel when the building process starts to
# generate key-value information that represents the status of the
# workspace. The output should be like
#
# KEY1 VALUE1
# KEY2 VALUE2
#
# If the script exits with a non-zero code, it's considered as a failure
# and the output will be discarded.

$ErrorActionPreference = "Stop"

# Get the repository URL without credentials
$repo_url = (git config --get remote.origin.url) -replace '//.*?:.*?@', '//'
$repo_url = $repo_url -replace '^git@github.com:', 'https://github.com/'
$urrepo_urll = $repo_url -replace '\.git$'
Write-Output "REPO_URL $repo_url"

# Get the commit SHA
$commit_sha = git rev-parse HEAD
Write-Output "COMMIT_SHA $commit_sha"

# Get the current Git branch
$git_branch = git rev-parse --abbrev-ref HEAD
Write-Output "GIT_BRANCH $git_branch"

# Check if there are any modified files in the working directory
if (git diff-index --quiet HEAD) {
    $git_tree_status = 'Clean'
} else {
    $git_tree_status = 'Modified'
}
Write-Output "GIT_TREE_STATUS $git_tree_status"
