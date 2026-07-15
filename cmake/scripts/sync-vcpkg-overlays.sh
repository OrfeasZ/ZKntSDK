#!/usr/bin/env bash
#
# Regenerate the vcpkg overlay ports needed to cross-compile the SDK on Linux.

set -euo pipefail
cd "$(dirname "$0")/../.."

UPSTREAM=External/vcpkg/ports
OVERLAY=cmake/vcpkg-ports-cross
PATCHES=cmake/scripts/overlay-patches

echo
echo "Done!"
