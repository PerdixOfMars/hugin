# Hugin

This context defines the automation vocabulary specific to Hugin as a UI testing framework for Munin interfaces. It exists to separate Hugin's query and execution language from the lower-level UI facts exposed by Munin.

## Current public interface terminology

**Hugin** is a UI automation and testing DSL over Munin windows. Its public vocabulary is built around **Automation Sessions**, **Selectors**, **Nodes**, and **Diagnostic Exceptions**.

An **Automation Session** is the user-facing object that queries one `munin::window`, finds **Nodes**, and routes node **Actions** back to that window.

A **Node** is a lightweight handle to one matched item in an **Automation Snapshot**. It exposes user-facing facts such as role and accessible name, and it can perform supported interactions such as **Node Click**.

Public selector construction is expressed through `hugin::by`, such as role, role-and-name, and automation-id selectors. Concrete selector types are implementation details rather than domain vocabulary that tests should name directly.

**Strict Find** means exactly one match. If no node or multiple nodes match, Hugin throws a **Diagnostic Exception** that explains the failed selector and summarizes relevant visible nodes.

**Diagnostics** are user-facing failure reports for executable UI specifications. They should explain failed automation assertions in selector and visible-node terms, not as raw JSON dumps or generic assertion text.

## Language

**C++ Testing DSL**:
Hugin's public C++ vocabulary for writing readable executable specifications of Munin UI behavior.
_Avoid_: Textual script language, parser DSL, thin wrapper API

**Selector**:
A Hugin expression that locates one or more targets or matching sets by interpreting the Munin **Automation Surface** rather than by inspecting implementation internals.
_Avoid_: Raw tree path, widget pointer lookup

**Strict Find**:
A Hugin DSL lookup that must resolve exactly one **Node** or produce a **Diagnostic Failure**.
_Avoid_: Silent null result, raw optional in test body

**Query**:
A non-failing Hugin lookup that returns zero or more matching **Nodes**.
_Avoid_: Strict find, boolean-only existence check, assertion substitute

**Node Predicate**:
A composable Hugin test expression that decides whether a **Node** matches some user-facing condition.
_Avoid_: Closed selector enum, hard-coded query branch

**Role Predicate**:
A **Node Predicate** that matches a **Node** by its Munin **Semantic Role** while remaining open to application-defined roles.
_Avoid_: Closed role enum, hard-coded role switch

**Node**:
A Hugin abstraction over one automation-visible item from a Munin **Automation Snapshot**.
_Avoid_: Munin Component, widget object, owned control

**Snapshot-Bound Node**:
A **Node** whose facts come from one specific **Automation Snapshot** and are not dynamically updated after actions or later snapshots.
_Avoid_: Live component handle, auto-refreshing node

**Automation Bounds**:
The bounds of a **Node** in the coordinate space Hugin uses to issue actions against the inspected UI root.
_Avoid_: Local component position, raw render offset, screen scrape bounds

**Action**:
A Hugin-issued user-intent interaction that is realized through the Munin **Native Event Path** rather than by direct component mutation.
_Avoid_: Helper call, state poke

**Node Click**:
A stable node-level Hugin **Action** that invokes the user intent of clicking a **Node**, regardless of the specific Munin mouse primitives needed underneath.
_Avoid_: Test-authored mouse-down sequence, direct on_click call

**Structural Assertion**:
A Hugin claim about the shape or semantics of an inspected UI surface, such as the count of matching components or the **Semantic Role** of a specific target.
_Avoid_: Screenshot scrape, implementation-only check

**Diagnostic Failure**:
A Hugin test failure report that explains the broken UI contract in terms of selectors, visible nodes, focus, snapshots, and recent actions.
_Avoid_: Raw optional failure, generic assertion text, unadorned JSON dump

**Diagnostic Exception**:
An exception carrying a **Diagnostic Failure** when strict DSL flow cannot continue, such as a failed **Strict Find**.
_Avoid_: Sentinel node, ignored error code

**Assertion Helper**:
A Hugin-owned testing helper that evaluates a UI condition and reports a **Diagnostic Failure** when the condition is not met.
_Avoid_: Raw EXPECT_TRUE over query results, test-authored diagnostic string

**Automation Snapshot**:
A point-in-time Hugin view of the Munin **Automation Surface** requested explicitly for inspection or post-action verification.
_Avoid_: Live mutation stream, implicit push feed

**Automation Session**:
A Hugin conversation with one inspected Munin UI surface through which snapshots are requested and actions are issued.
_Avoid_: Global server monitor, multi-client broadcast

**Focus Assertion**:
A Hugin claim about the current Munin focus state, typically over the **Focused Leaf** and sometimes over the full **Focus Path**.
_Avoid_: Cursor guess, visual highlight scrape

**Focused Leaf Assertion**:
A Hugin **Focus Assertion** that verifies a selected **Node** is the current **Focused Leaf**, not merely a focused ancestor container.
_Avoid_: Focused container assertion, first focused node check

**Focus Path Query**:
A Hugin query that returns the current Munin **Focus Path** as ordered, **Snapshot-Bound Nodes** from the inspected content root through focused ancestors to the **Focused Leaf**.
_Avoid_: Leaf-only focus query, unordered focused-node set

**Primitive Action**:
A minimal Hugin-issued input operation such as a keyboard press or mouse interaction from which richer conveniences are composed.
_Avoid_: Compound helper, test-script macro

**Convenience Action**:
A higher-level Hugin helper that resolves targets and sequences one or more **Primitive Actions** without changing the underlying automation contract.
_Avoid_: New transport primitive, direct widget API

**Wait Condition**:
A Hugin polling condition evaluated repeatedly against fresh **Automation Snapshot**s until it succeeds or times out.
_Avoid_: Push subscription, implicit background listener

## Relationships

- A **Selector** is resolved against one inspected Munin UI surface at a time
- The **C++ Testing DSL** is the user-facing product surface that composes **Selectors**, **Actions**, **Structural Assertions**, **Focus Assertions**, and **Wait Conditions**
- A **Selector** is built from one or more **Node Predicates** and resolves to zero or more **Nodes**
- A **Strict Find** expects exactly one **Node** and reports a **Diagnostic Failure** when a selector matches zero or many nodes
- A **Query** returns zero or more **Nodes** and does not produce a **Diagnostic Failure** by itself
- A role-and-name **Selector** matches a **Node** by combining a **Role Predicate** with its Munin **Accessible Name**
- Custom **Node Predicates** operate on Hugin **Nodes**, allowing application-specific UI concepts to participate in selection without depending directly on raw Munin JSON
- A **Node** abstracts a Munin **Component** for Hugin test interaction and assertion, but does not expose ownership of that component
- Every **Node** is a **Snapshot-Bound Node**; after an action or explicit fresh snapshot, tests must inspect newly resolved nodes to observe the new UI state
- A **Node** has **Automation Bounds** when Hugin can target it with coordinate-based actions
- An **Action** targets a **Node** or the current **Automation Session** and must execute through the Munin **Native Event Path**
- A **Node Click** may target any **Node** with **Automation Bounds**; whether the UI reacts is a later observable outcome
- A **Structural Assertion** may be evaluated either over all matches from a **Selector** or over one specifically identified **Node**
- An **Assertion Helper** is the normal way to express UI expectations whose value is in their Hugin diagnostic output
- A **Diagnostic Failure** is part of the **C++ Testing DSL** user experience, not an afterthought layered over raw engine errors
- A **Diagnostic Exception** is used when a strict DSL operation cannot produce a meaningful return value and must stop the current test flow
- An **Automation Snapshot** provides the inspected UI surface against which **Selectors** and **Structural Assertions** are evaluated
- A **Node** present in an **Automation Snapshot** is considered available for Hugin selection; Hugin does not define a separate visibility concept unless Munin exposes one
- An **Automation Session** owns the sequence of **Automation Snapshots** and **Actions** for one inspected UI surface
- A **Focus Assertion** is evaluated against the focus facts present in an **Automation Snapshot**
- A **Focused Leaf Assertion** fails when the selected **Node** is a focused ancestor container rather than the **Focused Leaf**
- A **Focus Path Query** exposes the broader **Focus Path** for diagnostics and container-owned focus behavior without changing the default focused-leaf assertion target
- A **Convenience Action** is composed from one or more **Primitive Actions**
- A **Wait Condition** is re-evaluated across successive **Automation Snapshot**s within one **Automation Session**

## Example dialogue

> **Dev:** "Should this test grab the widget instance directly?"
> **Domain expert:** "No — write a **Selector** and let Hugin resolve it to a **Node**."

> **Dev:** "Is `by::role_name(role::button, \"OK\")` a special hard-coded query?"
> **Domain expert:** "Conceptually no — it is a readable **Selector** built from **Node Predicates**."

> **Dev:** "Should roles be an enum?"
> **Domain expert:** "No — a role is expressed as a **Role Predicate** so custom UI roles can extend Hugin."

> **Dev:** "Should custom predicates inspect raw JSON?"
> **Domain expert:** "No — they should operate on Hugin **Nodes**. Raw JSON is an escape hatch, not the predicate contract."

> **Dev:** "When we say Hugin has a DSL, do we mean a separate script language?"
> **Domain expert:** "No — the MVP DSL is the **C++ Testing DSL** used directly in tests."

> **Dev:** "Can I ask whether this screen has two buttons and whether `@ok_button` is a button?"
> **Domain expert:** "Yes — those are both **Structural Assertions** evaluated through Hugin over the Munin automation data."

> **Dev:** "Can missing nodes just fail with `std::nullopt`?"
> **Domain expert:** "No — Hugin should produce a **Diagnostic Failure** that explains what selector failed and what nodes were visible."

> **Dev:** "Can tests just use `EXPECT_TRUE(ui.query(selector).size() == 1)`?"
> **Domain expert:** "No — use a Hugin **Assertion Helper** so failures explain the UI contract."

> **Dev:** "Should `node.visible()` exist?"
> **Domain expert:** "No — Munin does not expose a separate visibility concept, so Hugin should not invent one."

> **Dev:** "Should `ui.find(...)` return an optional?"
> **Domain expert:** "No — public **Strict Find** expects exactly one node; use a **Query** when zero or many matches are valid data."

> **Dev:** "What happens when strict find fails?"
> **Domain expert:** "It throws a **Diagnostic Exception** carrying the same report style used by assertion helpers."

> **Dev:** "Where does `by::role_name(role::button, \"OK\")` get `OK` from?"
> **Domain expert:** "From the Munin **Accessible Name** exposed on the button's automation data."

> **Dev:** "Is a Hugin Node the same thing as a Munin Component?"
> **Domain expert:** "No — a **Node** is Hugin's test-facing abstraction over a Munin automation item."

> **Dev:** "If I click a node, does that node update with the new UI state?"
> **Domain expert:** "No — it is a **Snapshot-Bound Node**. Take or query a fresh snapshot to inspect what changed."

> **Dev:** "Should tests spell out mouse down and mouse up to click a button?"
> **Domain expert:** "No — use **Node Click**. Hugin can change the underlying mouse primitive sequence without changing test code."

> **Dev:** "Can Hugin click only nodes that advertise a click action?"
> **Domain expert:** "No — any bounded **Node** can be clicked. The test can assert whether the UI reacted."

> **Dev:** "Does `node.bounds()` mean the component's local position?"
> **Domain expert:** "No — Hugin exposes **Automation Bounds**, the coordinate space used for automation actions against the inspected root."

> **Dev:** "After Hugin presses Enter, how does it know what changed?"
> **Domain expert:** "It requests a fresh **Automation Snapshot** and evaluates the next step against that point-in-time view."

> **Dev:** "Does Hugin start by talking to the whole terminal server?"
> **Domain expert:** "No — an **Automation Session** is scoped to one inspected UI surface, even if a remote transport is added later."

> **Dev:** "When I assert focus, am I talking about one node or the whole nesting chain?"
> **Domain expert:** "Usually the **Focused Leaf**, but Hugin may also expose **Focus Assertions** over the full Munin focus path."

> **Dev:** "Can `assert_focused(@panel)` pass when the panel contains the focused edit?"
> **Domain expert:** "No — a focused ancestor container is part of the **Focus Path**, but `assert_focused` targets the **Focused Leaf**."

> **Dev:** "How do I inspect the focused containers too?"
> **Domain expert:** "Use a **Focus Path Query**; it returns the ordered path as snapshot-bound **Nodes**."

> **Dev:** "Is `click(@ok_button)` part of the transport contract?"
> **Domain expert:** "No — that is a **Convenience Action** built from **Primitive Actions** such as target resolution and mouse input."

> **Dev:** "How does Hugin wait for the UI to settle?"
> **Domain expert:** "It reevaluates a **Wait Condition** against fresh **Automation Snapshot**s until success or timeout."

## Flagged ambiguities

- "find" was used loosely to mean either tree traversal or user-facing query resolution; resolved: **Selector** means the Hugin-side query language over Munin's exposed automation data
- "selector" risked becoming a closed list of hard-coded query kinds; resolved: **Selector** is predicate-first and can be composed from built-in or custom **Node Predicates**
- "role" risked becoming a closed enum; resolved: Hugin roles are expressed as extensible **Role Predicates**
- "custom predicate" risked exposing Munin JSON as the extension contract; resolved: custom **Node Predicates** operate on Hugin **Nodes**
- "find" risked meaning both one-match and many-match lookup; resolved: **Strict Find** means exactly one match with diagnostics, while **Query** means zero-or-more matches without diagnostics
- "node" risked meaning a Munin implementation object; resolved: **Node** means Hugin's test-facing abstraction over an automation-visible item, distinct from Munin **Component**
- "node handle" risked implying live updates; resolved: every **Node** is snapshot-bound and does not dynamically reflect later UI changes
- "click" risked exposing Munin's current low-level mouse mechanics; resolved: **Node Click** is stable DSL intent, with mouse primitive details hidden underneath
- "bounds" risked meaning local component position or rendered terminal scrape; resolved: **Automation Bounds** are the Hugin action-targeting bounds for a **Node**
- "assertion" was drifting between target lookup and UI-shape inspection; resolved: **Structural Assertion** means a Hugin claim about the inspected UI surface or a selected target
- "failure" risked meaning ordinary test-framework output; resolved: **Diagnostic Failure** is a core Hugin product behavior focused on clear executable-specification feedback
- "visible" risked becoming a Hugin-only state; resolved: Hugin treats snapshot presence as selectable availability unless Munin later exposes visibility
- For the MVP, role-oriented **Structural Assertions** read the Munin component `type` as the effective semantic role
- "current UI state" risked implying a live subscription model; resolved: **Automation Snapshot** means an explicit point-in-time read of the automation data
- "connection" risked meaning either a whole-server control channel or a per-UI test interaction; resolved: **Automation Session** means the Hugin interaction scope for one inspected UI surface
- "focus assertion" risked assuming Munin had only a single focused node; resolved: Hugin's default target is usually the Munin **Focused Leaf**, while the broader focus chain remains available
- "focused" risked accepting any focused ancestor; resolved: Hugin's focused-leaf assertion requires the selected node to be the innermost focused target
- "focus path" risked meaning an unordered list of focused nodes; resolved: a Hugin **Focus Path Query** returns ordered **Snapshot-Bound Nodes** from root to leaf
- "action API" risked conflating low-level input with ergonomic helpers; resolved: **Primitive Actions** form the contract and **Convenience Actions** are layered on top
- "wait" risked implying event subscriptions inside Munin; resolved: a **Wait Condition** is a Hugin-side polling construct over repeated snapshots
- "DSL" risked meaning a separate textual language; resolved: the MVP **C++ Testing DSL** is the public C++ executable-specification vocabulary, while textual scripts remain out of scope
