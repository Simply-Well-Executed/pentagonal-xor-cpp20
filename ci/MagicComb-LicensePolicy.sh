#!/bin/sh
# SPDX-License-Identifier: 0BSD
# ISSUE-29: retired legacy entrypoint; original exact bytes archived in Git.
# Archive: Archive/Retired/ci/MagicComb-LicensePolicy.sh.txt
# FRESH GammaGamma fixed control transfer; no original release operation.
# REQUIRES repository root as working directory; experimental only.
echo 'FRESH|ENTRY|ci/MagicComb-LicensePolicy.sh|RETIRED|HOLD'
exec sh ./ci/FRESH-GammaGamma.sh
exit 42
