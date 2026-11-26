# INCEPTION-GATE in PR Checks

Checks reads the exact GitHub merge revision and the PR file list. The first
job uses one inline PowerShell Core step on Linux. It reads the root policy and scans added or changed PowerShell
sources for UTF-8, parse errors, and the JSON rules in [AGENTS.md](../AGENTS.md).
It reads source through GitHub’s Contents API using the exact merge SHA. Unchanged historical scripts and workflow
inline step bodies are outside this scan. Prior evidence stays in its original
form.

The issue job runs separately and has only issue write and PR read access.
It opens one issue per triggered rule and revision. The issue digest covers
the repository, PR number, head and merge revisions, and sorted finding
locations. A rerun reuses the same issue. Changes to the workflow that defines
the scan require the same maintainer review as other CI policy changes.

## Acknowledge a trigger

1. Inspect the files, lines and revision in each INCEPTION-GATE issue.
2. As `pcheek1991` (GitHub ID 53888432), copy the issue’s exact
   `/ack-inception <head-sha> <digest>` command into a comment on that issue.
3. Use **Re-run failed jobs** on the **Checks** run. Until every issue for
   that revision is acknowledged, the existing PR validations remain skipped.

An acknowledgment authorizes the existing checks for that revision. Each
downstream check still has its existing pass criteria. A changed head,
merge revision or finding set requires fresh review. A clean updated revision
can pass without an exception. Old issues and runs retain their evidence.

GitHub restricts the issue token for fork PRs, so findings on fork PRs hold
without opening issues. A scan error or incomplete receipt also holds. Clean
PRs proceed to source guard, license custody, TRIFORCE and REELNATIVE.
The OpenBSD preflight retains its path condition. Existing push and manual
triggers are unchanged. The draft, merge, branch protection, and release
settings are separate from this workflow.
