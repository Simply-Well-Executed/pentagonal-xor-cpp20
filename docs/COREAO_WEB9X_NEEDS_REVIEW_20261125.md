# COREAO Web9X branch difference review

Status: NEEDS REVIEW
Witness base main tree 6d4081bbd7e22d7a519d49423047d9d267d8f55b
Witness integration head after compost index 4c892c9702b52c8d45a7ddd30c867d36f2549b7b
AIRGAP: evidence gathering is separate from changing source or claiming document retirement

## Files present on main but absent from inspected PR 34 tree

| Source | Main Git blob SHA1 | Decision |
| --- | --- | --- |
| docs/WEB9X_NITEREX_CURRENT.md | 92bae96b6bc63c7a78620cc1e658d856bfdfb1c0 | NEEDS REVIEW |
| docs/WEB9X_OSI7_NONLOSS_CONTRACT.md | acd17d420d2bbed3bfc4c8c19755e97ddaf7b92c | NEEDS REVIEW |

Main design contract identifies an EXPLORATORY current Web9X specification and reserves slots 7 through 9
The OSI7 document separates seven review concerns and preserves earlier slot experiments as historic data
Neither document should be relabeled RETIRED simply because it is absent from this branch snapshot

## COREAO review path

HEAD: verify latest source tree and exact blob OIDs before proposing an update
AIRGAP: prohibit automatic copying into another branch or removal from main
TAIL: reconcile via a separately reviewed change only if omission is established as unintended
A future merge comparison must preserve both source OIDs and report any content conflict
No automatic repair was attempted
No complete CI claim was made

Punctuation note: avoid decorative full stops and em dashes in new prose while retaining syntax and exact source identities
RELEASE HOLD
