#!/bin/sh
set -eu
LC_ALL=C
export LC_ALL
fail() { printf 'IRON|STD-INVENTORY|FAIL|%s\n' "$*" >&2; exit 1; }
command -v git >/dev/null 2>&1 || fail 'local Git required'
command -v grep >/dev/null 2>&1 || fail 'local grep required'
command -v wc >/dev/null 2>&1 || fail 'local wc required'
command -v tr >/dev/null 2>&1 || fail 'local tr required'
root=$(CDPATH='' cd "$(dirname "$0")/.." && pwd -P) || fail 'repository root'
cd "$root" || fail 'repository root unavailable'
git diff --quiet --exit-code HEAD -- || fail 'tracked source has local edits'
head=$(git rev-parse --verify HEAD) || fail 'local HEAD missing'
test "${#head}" -eq 40 || fail 'malformed commit identifier'
names=$(git -c core.quotepath=false ls-files -- '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hpp') ||
  fail 'Git C/C++ file inventory failed'
test -n "$names" || fail 'tracked C/C++ source inventory is empty'
printf "schema: SWE-STD-FREE-INVENTORY-v1\n"
printf "authority: local-git-and-current-tracked-files\n"
printf "source_commit: '%s'\n" "$head"
printf "status: 'SOURCE_TOKEN_AUDIT_ONLY'\n"
printf "files:\n"
total=0
blocked=0
review=0
candidate=0
namespace_total=0
standard_header_total=0
oldIFS=$IFS
IFS='
'
for path in $names; do
  case "$path" in
    *[!a-zA-Z0-9_./-]*|'') fail 'unsafe path encoding in Git inventory; manual review required' ;;
  esac
  test -f "$path" || fail "missing or unexpected tracked source path: $path"
  count_std=$(grep -o 'std::' "$path" | wc -l | tr -d ' ')
  count_headers=$(grep -cE '^[[:space:]]*#[[:space:]]*include[[:space:]]*<(array|vector|string|iostream|ostream|iomanip|sstream|fstream|cmath|math[.]h|cstdio|stdio[.]h|cstdlib|stdlib[.]h|cstring|string[.]h|cstdint|stdint[.]h|cstddef|algorithm|bit|format|span|ranges|memory|new|exception|stdexcept|limits|type_traits|optional|variant|map|set|unordered_map|unordered_set|utility|iterator|filesystem|chrono|complex|numbers|ratio|numeric|functional|concepts|string_view)(>|/)' "$path" || :)
  count_angle=$(grep -cE '^[[:space:]]*#[[:space:]]*include[[:space:]]*<' "$path" || :)
  if test "$count_std" -gt 0 || test "$count_headers" -gt 0; then
    state='BLOCKED_STD_REFERENCES'
    blocked=$((blocked + 1))
  elif test "$count_angle" -gt 0; then
    state='REVIEW_NONSTANDARD_HEADERS'
    review=$((review + 1))
  else
    state='SOURCE_CANDIDATE_NOT_LINK_CERTIFIED'
    candidate=$((candidate + 1))
  fi
  total=$((total + 1))
  namespace_total=$((namespace_total + count_std))
  standard_header_total=$((standard_header_total + count_headers))
  printf "  - path: '%s'\n" "$path"
  printf "    std_namespace_tokens: %s\n" "$count_std"
  printf "    standard_header_lines: %s\n" "$count_headers"
  printf "    angle_include_lines: %s\n" "$count_angle"
  printf "    state: '%s'\n" "$state"
done
IFS=$oldIFS
printf "summary:\n"
printf "  tracked_source_files: %s\n" "$total"
printf "  std_namespace_tokens: %s\n" "$namespace_total"
printf "  standard_header_lines: %s\n" "$standard_header_total"
printf "  blocked_files: %s\n" "$blocked"
printf "  review_files: %s\n" "$review"
printf "  source_candidate_files: %s\n" "$candidate"
printf "  linked_std_runtime_status: 'NOT_TESTED'\n"
printf "  complete_repository_std_free: '%s'\n" \
  "$(if test "$blocked" -eq 0 && test "$review" -eq 0; then echo 'SOURCE_CANDIDATE_ONLY'; else echo 'NOT_CERTIFIED'; fi)"
