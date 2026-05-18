# Hugin MVP PRD

## Problem Statement

Khoravel needs a reliable way to test and automate Munin terminal user interfaces without scraping rendered terminal output. Existing terminal-level testing is too brittle to express user intent, reason about focus and interactivity, or produce good diagnostics when UI behavior changes.

## Solution

Hugin will provide a C++ library for automated inspection and interaction with Munin UIs. It will consume Munin's existing `to_json()`-based introspection surface, enriched for automation use, and drive the UI through the same native keyboard and mouse event paths used by live application input. The MVP will be in-process, snapshot-based, and test-first, with a small primitive action set and a few ergonomic helpers layered on top.

## User Stories

1. As a UI test author, I want to fetch a point-in-time automation snapshot of a Munin UI, so that I can inspect the current component tree without scraping terminal text.
2. As a UI test author, I want to locate a component by stable Automation ID, so that tests survive label and layout changes.
3. As a UI test author, I want to ask whether a screen contains a certain number of components of a given type, so that I can assert structural expectations about the UI.
4. As a UI test author, I want to assert that a specific component exists, so that I can verify screen composition directly.
5. As a UI test author, I want to assert that a component's current type matches the expected semantic role, so that I can verify user-facing meaning rather than raw terminal output.
6. As a UI test author, I want to inspect the raw introspection JSON when helpers are insufficient, so that I can debug failing tests without waiting for new helper APIs.
7. As a UI test author, I want to send keyboard input through the native Munin event path, so that tests exercise the same behavior as real users.
8. As a UI test author, I want to send mouse input through the native Munin event path, so that pointer interactions are tested honestly.
9. As a UI test author, I want to move focus through normal keyboard navigation, so that I can verify focus behavior and input routing.
10. As a UI test author, I want to assert the currently focused leaf component, so that I can verify navigational behavior precisely.
11. As a UI test author, I want access to the broader focus path when needed, so that container-owned focus behavior remains visible in diagnostics.
12. As a UI test author, I want to wait for a condition by polling fresh snapshots until timeout, so that asynchronous or multi-step UI changes can be tested deterministically.
13. As a UI test author, I want convenience helpers such as target resolution plus click-like behavior built on primitives, so that common tests stay readable without weakening the core contract.
14. As a framework developer, I want Hugin's selector and assertion logic separated from Munin wiring, so that the core can be tested in isolation.
15. As a framework developer, I want the Munin automation adapter to be thin, so that later transports such as a telnet side-channel can be added without rewriting the Hugin core.
16. As a framework developer, I want Munin to own automation facts and Hugin to own automation behavior, so that each library evolves at the right abstraction level.
17. As a Munin UI author, I want to assign Automation IDs without changing every constructor signature, so that automation support can be adopted incrementally across components.
18. As a Munin UI author, I want Automation IDs to be unique only within one component tree, so that multiple similar client UIs can coexist in the same process.
19. As a debugger, I want introspection JSON to remain useful outside formal tests, so that the same surface helps with both automation and troubleshooting.
20. As a product team, I want a small proving scenario with a form, focus movement, text entry, and button activation, so that the MVP proves the full vertical slice before wider rollout.
21. As a future transport implementer, I want the automation session model to be scoped to one UI surface, so that remote transports can mirror the same contract cleanly.
22. As a future Hugin maintainer, I want tests to drive development of the core modules, so that the framework grows from verified behavior instead of ad hoc helpers.

## Implementation Decisions

- Hugin will be a dedicated library in the `hugin` repository and will not be implemented in the metaproject root.
- Munin will expose the automation facts Hugin needs; Hugin will do the selector, assertion, waiting, and runner work.
- Munin automation support will build on the existing `to_json()` mechanism, treated as the canonical introspection JSON surface for automation.
- Munin components will gain a stable Automation ID capability at the core component level.
- Automation IDs will be unique within a single rendered component tree, not globally across a process.
- The first Hugin protocol will be snapshot-based: Hugin explicitly requests fresh automation snapshots when it needs to inspect state.
- The first waiting model will be polling-based, with explicit timeout behavior owned by Hugin.
- Hugin actions must drive the UI through the normal Munin native event path rather than direct state mutation or widget-specific shortcuts.
- The MVP primitive action set will be keyboard input, mouse input, and snapshot/inspection operations.
- Convenience actions will be layered on top of primitives and will not redefine the core transport contract.
- The first public API will be a programmatic C++ library API; no textual DSL or external script format is part of the MVP.
- The MVP will be in-process only, but the architecture should preserve a transport-neutral core so a remote side-channel can be added later.
- Hugin should expose both structured helper APIs and direct raw snapshot access.
- Munin focus is hierarchical; automation snapshots should preserve enough information for Hugin to expose both focused-leaf assertions and fuller focus diagnostics.
- For the MVP component set, component `type` is treated as the effective semantic role. A separate explicit role field is deferred unless the component set proves that `type` and role diverge.
- The initial deep module split should include a Munin automation adapter responsible for snapshot fetch and native input injection.
- The initial deep module split should include a snapshot model and query core responsible for selectors, counts, role checks, and focus reasoning.
- The initial deep module split should include a runner core responsible for sequencing primitive actions, convenience actions, and wait conditions over an automation session.
- The initial deep module split should include a thin test-facing API responsible for ergonomic access from C++ tests.
- The first proving scenario should be a small focusable form flow covering snapshot inspection, Automation ID lookup, type/role assertion, focus movement, text entry, button activation, and post-action verification.

## Testing Decisions

- Tests will drive development of Hugin.
- A good test should verify externally observable automation behavior rather than implementation details of internal helper classes.
- Most coverage should target the deep modules in isolation, especially the snapshot model and query core.
- Most coverage should target the deep modules in isolation, especially the runner core.
- Unit tests should use synthetic or fixture snapshots heavily to validate selector resolution, structural assertions, focus assertions, and wait-condition behavior quickly and deterministically.
- A smaller set of integration tests should target the Munin automation adapter and verify that real native keyboard and mouse inputs produce the expected next snapshots.
- The thin public API should receive lighter smoke coverage to ensure ergonomics stay aligned with actual capabilities.
- The first end-to-end test fixture should be the small form scenario agreed during design grilling.
- Prior art exists in Munin's current JSON-reporting tests, which already validate parts of the introspection surface and can inform how automation-facing snapshot assertions are written.

## Out of Scope

- A telnet side-channel in the MVP
- A live push-stream or subscription-based snapshot update model
- A separate normalized introspection schema distinct from Munin `to_json()`
- A textual Hugin DSL or standalone script runner
- Broad semantic-role normalization beyond the current `type`-to-role equivalence
- Large-scale adoption across real Khoravel UIs before the proving scenario is complete
- Direct manipulation shortcuts that bypass the native Munin event path

## Further Notes

- The design assumes Munin may need small core changes to fully support automation, and that is acceptable.
- The glossary for automation facts now lives in Munin, while the glossary for automation behavior lives in Hugin; future work should continue respecting that split.
- The MVP should stay intentionally small. The goal is to prove a trustworthy automation seam, then expand from that working base into broader Khoravel use.
