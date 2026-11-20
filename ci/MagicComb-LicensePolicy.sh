#!/bin/sh
# SPDX-License-Identifier: 0BSD
# MagicComb LICENSE gate: unapproved components must NOT be released.
# Explicit operator rule 2026-11-20: LGPL is permitted (with obligations).
# GPL/AGPL, EUPL, EPL and unapproved GNU-licensed components are not.
# Do not infer that every copyleft component is forbidden; classify each license.
# Unknown licensing is a review hold, not silent permission.
set -eu
export LC_ALL=C
fail() { printf 'IRON|MAGICCOMB|LICENSE|HOLD|%s\n' "$*" >&2; exit 1; }
test "${1:-}" = image || fail 'usage: /bin/sh ci/MagicComb-LicensePolicy.sh image'
command -v dpkg-query >/dev/null 2>&1 || fail 'package database not available for full license audit'
command -v grep >/dev/null 2>&1 || fail 'license scanner not available'
command -v awk >/dev/null 2>&1 || fail 'package parser not available'
command -v sort >/dev/null 2>&1 || fail 'package sorter not available'
command -v sha256sum >/dev/null 2>&1 || fail 'SHA256 audit not available'

allowed=0
forbidden=0
unknown=0
packages=0
printf 'IRON|MAGICCOMB|LICENSE|HEAD|policy=LGPL_ALLOWED_GPL_AGPL_EUPL_EPL_REJECTED|other_GNU_and_unknown_review_hold\n'
# Container package inventory comes from its OWN dpkg database. The
# Linux kernel, files not owned by packages, JAR metadata, and bundled
# PowerShell/.NET/Oracle assets require SEPARATE attribution.
dpkg-query -W -f='${binary:Package}\t${db:Status-Abbrev}\n' |
  awk '$2 == "ii" {print $1}' |
  sort | while IFS= read -r name; do
    test -n "$name" || continue
    packages=$((packages + 1))
    base_name=${name%%:*}
    copyright="/usr/share/doc/$base_name/copyright"
    # Debian multiarch names carry :amd64 while /usr/share/doc usually does not.
    if test ! -r "$copyright"; then copyright="/usr/share/doc/$name/copyright"; fi
    if test ! -r "$copyright"; then
      printf 'IRON|MAGICCOMB|LICENSE|UNKNOWN|%s|missing_copyright_file\n' "$name"
      unknown=$((unknown + 1))
    elif grep -iEq '(^|[^A-Za-z])(AGPL|GPL|EUPL|EPL)([-0-9.+]*([^A-Za-z]|$)|[^A-Za-z]|$)|GNU[[:space:]]+(General|Affero[[:space:]]+General)[[:space:]]+Public[[:space:]]+License|Eclipse[[:space:]]+Public[[:space:]]+License|European[[:space:]]+Union[[:space:]]+Public[[:space:]]+License' "$copyright"; then
      # GPL + LGPL together is mixed REVIEW, not automatic LGPL admission.
      # A documented SPDX OR alternative can only be chosen in a scoped review.
      printf 'IRON|MAGICCOMB|LICENSE|FORBIDDEN_OR_MIXED|%s|operator_review_required\n' "$name"
      forbidden=$((forbidden + 1))
    elif grep -iEq '(^|[^A-Za-z])LGPL([-0-9.+]*([^A-Za-z]|$)|[^A-Za-z]|$)|GNU[[:space:]]+Lesser[[:space:]]+General[[:space:]]+Public[[:space:]]+License' "$copyright"; then
      # User approved LGPL; obligations, file scope, and other bundled licenses
      # still require independently verified attribution before distribution.
      printf 'IRON|MAGICCOMB|LICENSE|LGPL_PERMITTED|%s|obligations_and_file_scope_review\n' "$name"
      allowed=$((allowed + 1))
    elif grep -iqE '^License:[[:space:]]*[^[:space:]]+' "$copyright"; then
      printf 'IRON|MAGICCOMB|LICENSE|REVIEW|%s|non_GNU_not_equivalent_to_approved\n' "$name"
      unknown=$((unknown + 1))
    else
      printf 'IRON|MAGICCOMB|LICENSE|UNKNOWN|%s|non_machine_readable_metadata\n' "$name"
      unknown=$((unknown + 1))
    fi
    # NOTE: counts inside a pipe subshell are advisory only. Emit each
    # result line and let the HOST verify the audit's full source record.
  done
printf 'IRON|MAGICCOMB|LICENSE|TAIL|metadata_probed|fail_closed_requires_independent_host_evidence\n'
# This is a proof-of-negative only when external component attribution
# is complete. Debian package metadata by itself cannot prove it.
fail 'a complete SPDX SBOM, Oracle NFTC review and GNU/EPL exception ledger are required'
