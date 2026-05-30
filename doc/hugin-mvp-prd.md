# Hugin MVP PRD

## Problem Statement

Khoravel needs a reliable way to test and automate Munin terminal user interfaces without scraping rendered terminal output. Existing terminal-level testing is too brittle to express user intent, reason about focus and interactivity, or produce clear diagnostic failures when UI behavior changes.

## Solution

Hugin will provide a high-level C++ testing DSL for automated inspection, manipulation, investigation, and assertion of Munin UIs. That DSL is the MVP product surface: tests should read as executable specifications of user-visible UI behavior rather than as Munin wrapper calls or raw snapshot inspection.

Because Hugin is a testing framework, its primary user experience is the expressiveness and clarity of its failures. Diagnostic failure output is therefore a first-class MVP behavior, not polish to add after happy-path automation works.

Under the DSL, Hugin will consume Munin's existing `to_json()`-based introspection surface, enriched for automation use, and drive the UI through the same native keyboard and mouse event paths used by live application input. The MVP will be in-process, snapshot-based, and test-first, with the underlying engine kept separate from the public DSL vocabulary.

## User Stories

1. [x] As a UI test author, I want to fetch a point-in-time automation snapshot of a Munin UI, so that I can inspect the current component tree without scraping terminal text.
2. [x] As a UI test author, I want to locate a component by stable Automation ID, so that tests survive label and layout changes.
3. [x] As a UI test author, I want to ask whether a screen contains a certain number of components of a given type, so that I can assert structural expectations about the UI.
4. [x] As a UI test author, I want to assert that a specific component exists, so that I can verify screen composition directly.
5. [x] As a UI test author, I want to assert that a component's current type matches the expected semantic role, so that I can verify user-facing meaning rather than raw terminal output.
6. [x] As a UI test author, I want to locate a control by semantic role plus accessible name, so that tests can target user-visible controls without scraping terminal rendering.
7. [ ] As a UI test author, I want to inspect the raw introspection JSON when helpers are insufficient, so that I can debug failing tests without waiting for new helper APIs.
8. [ ] As a UI test author, I want to send keyboard input through the native Munin event path, so that tests exercise the same behavior as real users.
9. [x] As a UI test author, I want to send mouse input through the native Munin event path, so that pointer interactions are tested honestly.
10. [ ] As a UI test author, I want to move focus through normal keyboard navigation, so that I can verify focus behavior and input routing.
11. [ ] As a UI test author, I want to assert the currently focused leaf component, so that I can verify navigational behavior precisely.
12. [ ] As a UI test author, I want access to the broader focus path when needed, so that container-owned focus behavior remains visible in diagnostics.
13. [ ] As a UI test author, I want to wait for a condition by polling fresh snapshots until timeout, so that asynchronous or multi-step UI changes can be tested deterministically.
14. [ ] As a UI test author, I want a small explicit C++ DSL of sessions, selectors, nodes, actions, assertions, and waits, so that tests express user-visible UI contracts clearly.
15. [ ] As a UI test author, I want DSL failures to explain the selector, matched or missing nodes, current focus, and recent actions, so that broken UI contracts are quick to diagnose.
16. [x] As a UI test author, I want convenience helpers such as target resolution plus click-like behavior built on primitives, so that common tests stay readable without weakening the core contract.
17. [ ] As a framework developer, I want Hugin's public C++ DSL separated from its underlying engine, so that readable test vocabulary does not collapse into Munin wrapper mechanics.
18. [ ] As a framework developer, I want Hugin's selector and assertion logic separated from Munin wiring, so that the core can be tested in isolation.
19. [x] As a framework developer, I want the Munin automation adapter to be thin, so that later transports such as a telnet side-channel can be added without rewriting the Hugin core.
20. [x] As a framework developer, I want Munin to own automation facts and Hugin to own automation behavior, so that each library evolves at the right abstraction level.
21. [x] As a Munin UI author, I want to assign Automation IDs without changing every constructor signature, so that automation support can be adopted incrementally across components.
22. [x] As a Munin UI author, I want Automation IDs to be unique only within one component tree, so that multiple similar client UIs can coexist in the same process.
23. [x] As a Munin UI author, I want components with visible identifying text to expose an accessible name, so that Hugin can resolve user-facing selectors without terminal scraping.
24. [x] As a debugger, I want introspection JSON to remain useful outside formal tests, so that the same surface helps with both automation and troubleshooting.
25. [ ] As a product team, I want a small proving scenario with a form, focus movement, text entry, button activation, and diagnostic assertion failure, so that the MVP proves the full vertical slice before wider rollout.
26. [x] As a future Hugin maintainer, I want tests to drive development of the core modules, so that the framework grows from verified behavior instead of ad hoc helpers.

## Implementation Decisions

- Hugin will be a dedicated library in the `hugin` repository and will not be implemented in the metaproject root.
- Munin will expose the automation facts Hugin needs; Hugin will do the selector, assertion, waiting, and runner work.
- Munin automation support will build on the existing `to_json()` mechanism, treated as the canonical introspection JSON surface for automation.
- Munin components will gain a stable Automation ID capability at the core component level.
- Munin components with visible identifying text, beginning with `munin::button`, will expose that text as an accessible name in Introspection JSON.
- For the MVP, `by::role_name(role::button, "OK")` matches a node whose effective semantic role is `button` and whose accessible name is `OK`.
- Automation IDs will be unique within a single rendered component tree, not globally across a process.
- The first Hugin protocol will be snapshot-based: Hugin explicitly requests fresh automation snapshots when it needs to inspect state.
- The first waiting model will be polling-based, with explicit timeout behavior owned by Hugin.
- Hugin actions must drive the UI through the normal Munin native event path rather than direct state mutation or widget-specific shortcuts.
- The first public API will be a C++ Testing DSL, including sessions, selectors, nodes, actions, assertions, waits, and diagnostic failure output.
- Diagnostic failure quality is part of the MVP contract and should be specified from the first single-button scenario.
- Public `find()`-style DSL lookup will be strict: it expects exactly one node and reports a diagnostic failure when a selector matches zero or many nodes.
- Public `query()`-style DSL lookup will return zero or more nodes without producing a diagnostic failure by itself, for waits, predicates, conditional assertions, and engine-level tests.
- Hugin will provide minimal assertion helpers early, such as `expect_that(ui, has_node(selector))`, so UI expectations fail with Hugin-owned diagnostics instead of raw GoogleTest boolean output.
- Hugin's assertion machinery should split diagnostic evaluation from test-framework reporting: an engine-level condition should produce a structured diagnostic result, and a thin GoogleTest adapter should turn that result into a test failure.
- Strict `find()` failures and assertion helper failures should share the same diagnostic engine rather than developing separate reporting paths.
- Strict `find()` should return a node on exactly one match or throw a Hugin diagnostic exception carrying the diagnostic report when the selector matches zero or many nodes.
- Hugin selectors will be predicate-first: conceptually, a selector is composed from one or more predicates over Hugin nodes.
- Built-in selectors such as `by::role_name(role::button, "OK")` will be readable factories over predicate composition rather than a closed selector enum.
- Hugin roles will be represented by extensible role predicates or function objects, not by a closed enum.
- Custom predicates will operate on the public `hugin::node` abstraction rather than directly on raw Munin JSON.
- The exact C++ representation of predicates and function objects will be driven by executable specifications rather than fixed in advance.
- The underlying engine will remain separate from the public DSL and will own selector evaluation, node resolution, action sequencing, snapshot polling, and diagnostic data collection.
- The public DSL noun for a resolved automation item will be `hugin::node`, distinct from `munin::component`; a node is a test-facing abstraction over an automation-visible item, not ownership of the underlying component.
- Hugin nodes are snapshot-bound wrappers around facts from a specific automation snapshot; they are not dynamically updated after actions or later snapshot requests.
- The minimum public node fact surface for the first scenario is role, accessible name, automation ID, automation bounds, focus state, and raw JSON as a debugging escape hatch.
- `node.raw_json()` may exist for debugging and temporary escape hatches, but predicates, selectors, actions, and matchers should prefer named Hugin node facts.
- Hugin will not expose a separate `visible()` node fact in the MVP; presence in the automation snapshot is the selectable availability concept unless Munin later exposes visibility.
- Node facts such as enabled state, value, and advertised actions are deferred until executable specifications require them.
- Tests must request or query fresh snapshot data to inspect UI state after a click, key press, or other action.
- `node.click()` is the public DSL for clicking a node and should remain stable even if Munin's underlying mouse event primitives evolve.
- `node.click()` will target the node using bounds derived from the snapshot that produced the node, rather than by re-resolving the selector at click time.
- Any node with automation bounds can be clicked in the MVP; whether the UI reacts is an observable outcome for the test to assert, not a precondition for issuing the click.
- Hugin's node-resolution engine must preserve automation bounds for each resolved node. Munin component JSON may report component-local positions, so Hugin traversal must compute the non-offset root coordinate position needed to send native mouse events through the inspected UI root.
- Lower-level session actions such as explicit mouse and key events may still exist for tests whose behavior is specifically about raw input.
- The MVP primitive action set will be keyboard input, mouse input, and snapshot/inspection operations.
- Convenience actions will be layered on top of primitives and will not redefine the core transport contract.
- No textual DSL, parser, external script format, or standalone runner is part of the MVP.
- The MVP will be in-process only.
- Hugin should expose both structured helper APIs and direct raw snapshot access.
- Munin focus is hierarchical; automation snapshots should preserve enough information for Hugin to expose both focused-leaf assertions and fuller focus diagnostics.
- For the MVP component set, component `type` is treated as the effective semantic role. A separate explicit role field is deferred unless the component set proves that `type` and role diverge.
- The initial layered architecture should include a public C++ DSL layer responsible for readable test vocabulary and high-quality failure messages.
- The initial layered architecture should include an underlying engine responsible for selectors, node resolution, actions, waits, snapshots, and diagnostic assembly.
- The initial layered architecture should include a Munin adapter responsible for snapshot fetch and native input injection.
- The first proving scenario should be a single accessible button in a `munin::window`, proving `hugin::session`, `hugin::node`, role/name lookup, node click, post-action observation through a fresh snapshot, and at least one intentional missing-node diagnostic failure case.
- The first button click outcome should be verified by an observable change in the UI automation surface, not by listening directly to a Munin component callback.
- The smallest post-click UI change for the first proving scenario should be a sibling `image` node changing its accessible name from `Before` to `After`.
- In Munin's terminal UI vocabulary, `image` remains the appropriate MVP role for static label-like content; a generic `text` role should not be introduced merely because the rendered medium is textual.
- After the single-button scenario is green, the next proving scenario should be a small focusable form flow covering label lookup, focus movement, text entry, button activation, and post-action verification.

## Testing Decisions

- Tests will drive development of Hugin.
- The outermost TDD slice for Hugin behavior should begin with a `hugin::session` over a `munin::window` containing a real component composition, so each behavior is pulled by a user-facing UI scenario.
- Outer session/window slices should decompose into unit-level engine tests only when the scenario reveals a needed concept such as nodes, roles, selectors, actions, waits, or diagnostics.
- A good test should verify externally observable automation behavior rather than implementation details of internal helper classes.
- Most coverage should target the underlying engine in isolation, especially selector resolution, node modeling, actions, waits, and diagnostics.
- Public C++ DSL tests should prove that the high-level vocabulary remains readable and maps correctly onto engine behavior.
- Diagnostic failure tests should be introduced alongside happy-path DSL tests, beginning with the first missing-node case.
- Because Hugin itself will be tested with GoogleTest, diagnostic generation should be specified separately from the mechanics of failing the outer GoogleTest test where practical.
- Tests for assertion helpers should verify both the evaluated condition and the diagnostic content without relying only on intentionally failing the surrounding test process.
- Diagnostic engine tests should assert structured condition results and diagnostic text directly; GoogleTest adapter tests should remain small and focused on integration.
- Strict find diagnostics should be specified through the same engine-level diagnostic result model used by assertion helpers.
- Tests for strict find should verify both successful node return and thrown diagnostic exception content.
- Strict public `find()` and non-failing `query()` should be specified separately so tests do not blur exactly-one lookup with zero-or-more search mechanics.
- Unit tests should use synthetic or fixture snapshots heavily to validate selector resolution, structural assertions, focus assertions, and wait-condition behavior quickly and deterministically.
- A smaller set of integration tests should target the Munin automation adapter and verify that real native keyboard and mouse inputs produce the expected next snapshots.
- The thin public API should receive lighter smoke coverage to ensure ergonomics stay aligned with actual capabilities.
- The first end-to-end test fixture should be the single-button window scenario agreed during design grilling.
- The small form scenario should follow only after the single-button DSL path has proven the basic public vocabulary and diagnostic style.
- Prior art exists in Munin's current JSON-reporting tests, which already validate parts of the introspection surface and can inform how automation-facing snapshot assertions are written.

## Out of Scope

- A telnet side-channel in the MVP
- Remote control or remote session behavior in the MVP
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
