# Hugin

This context defines the automation vocabulary specific to Hugin as a UI testing framework for Munin interfaces. It exists to separate Hugin's query and execution language from the lower-level UI facts exposed by Munin.

## Language

**Selector**:
A Hugin expression that locates one or more targets or matching sets by interpreting the Munin **Automation Surface** rather than by inspecting implementation internals.
_Avoid_: Raw tree path, widget pointer lookup

**Action**:
A Hugin-issued user-intent interaction that is realized through the Munin **Native Event Path** rather than by direct component mutation.
_Avoid_: Helper call, state poke

**Structural Assertion**:
A Hugin claim about the shape or semantics of an inspected UI surface, such as the count of matching components or the **Semantic Role** of a specific target.
_Avoid_: Screenshot scrape, implementation-only check

**Automation Snapshot**:
A point-in-time Hugin view of the Munin **Automation Surface** requested explicitly for inspection or post-action verification.
_Avoid_: Live mutation stream, implicit push feed

**Automation Session**:
A Hugin conversation with one inspected Munin UI surface through which snapshots are requested and actions are issued.
_Avoid_: Global server monitor, multi-client broadcast

**Focus Assertion**:
A Hugin claim about the current Munin focus state, typically over the **Focused Leaf** and sometimes over the full **Focus Path**.
_Avoid_: Cursor guess, visual highlight scrape

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
- An **Action** targets the result of a **Selector** and must execute through the Munin **Native Event Path**
- A **Structural Assertion** may be evaluated either over all matches from a **Selector** or over one specifically identified target
- An **Automation Snapshot** provides the inspected UI surface against which **Selectors** and **Structural Assertions** are evaluated
- An **Automation Session** owns the sequence of **Automation Snapshots** and **Actions** for one inspected UI surface
- A **Focus Assertion** is evaluated against the focus facts present in an **Automation Snapshot**
- A **Convenience Action** is composed from one or more **Primitive Actions**
- A **Wait Condition** is re-evaluated across successive **Automation Snapshot**s within one **Automation Session**

## Example dialogue

> **Dev:** "Should this test grab the widget instance directly?"
> **Domain expert:** "No — write a **Selector** against the Munin automation data and let Hugin resolve it."

> **Dev:** "Can I ask whether this screen has two buttons and whether `@ok_button` is a button?"
> **Domain expert:** "Yes — those are both **Structural Assertions** evaluated through Hugin over the Munin automation data."

> **Dev:** "After Hugin presses Enter, how does it know what changed?"
> **Domain expert:** "It requests a fresh **Automation Snapshot** and evaluates the next step against that point-in-time view."

> **Dev:** "Does Hugin start by talking to the whole terminal server?"
> **Domain expert:** "No — an **Automation Session** is scoped to one inspected UI surface, even if a remote transport is added later."

> **Dev:** "When I assert focus, am I talking about one node or the whole nesting chain?"
> **Domain expert:** "Usually the **Focused Leaf**, but Hugin may also expose **Focus Assertions** over the full Munin focus path."

> **Dev:** "Is `click(@ok_button)` part of the transport contract?"
> **Domain expert:** "No — that is a **Convenience Action** built from **Primitive Actions** such as target resolution and mouse input."

> **Dev:** "How does Hugin wait for the UI to settle?"
> **Domain expert:** "It reevaluates a **Wait Condition** against fresh **Automation Snapshot**s until success or timeout."

## Flagged ambiguities

- "find" was used loosely to mean either tree traversal or user-facing query resolution; resolved: **Selector** means the Hugin-side query language over Munin's exposed automation data
- "assertion" was drifting between target lookup and UI-shape inspection; resolved: **Structural Assertion** means a Hugin claim about the inspected UI surface or a selected target
- For the MVP, role-oriented **Structural Assertions** read the Munin component `type` as the effective semantic role
- "current UI state" risked implying a live subscription model; resolved: **Automation Snapshot** means an explicit point-in-time read of the automation data
- "connection" risked meaning either a whole-server control channel or a per-UI test interaction; resolved: **Automation Session** means the Hugin interaction scope for one inspected UI surface
- "focus assertion" risked assuming Munin had only a single focused node; resolved: Hugin's default target is usually the Munin **Focused Leaf**, while the broader focus chain remains available
- "action API" risked conflating low-level input with ergonomic helpers; resolved: **Primitive Actions** form the contract and **Convenience Actions** are layered on top
- "wait" risked implying event subscriptions inside Munin; resolved: a **Wait Condition** is a Hugin-side polling construct over repeated snapshots
