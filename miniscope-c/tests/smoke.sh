#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
./miniscope --help | grep -q 'Linux process monitor'
./miniscope --once --top 5 > /tmp/miniscope-smoke-$$.txt
trap 'rm -f /tmp/miniscope-smoke-$$.txt' EXIT
grep -q 'CPU:' /tmp/miniscope-smoke-$$.txt
grep -q 'COMMAND' /tmp/miniscope-smoke-$$.txt
if ./miniscope --top 0 >/dev/null 2>&1; then echo 'Invalid count unexpectedly accepted' >&2; exit 1; fi
echo 'MiniScope smoke tests passed.'
