# Layout

Layout in ruis is a **two-phase** system: a *measure* phase in which widgets
report how big they want to be, and a *lay-out* phase in which containers
actually resize and position their children. Layout is **lazy**: it is driven
by a per-widget *dirty flag* and is (re)computed automatically **before a
widget is rendered**, so the widget tree is always laid out when it is drawn.

Key source files (all under `src/ruis/`):

- `layout/layout.hpp` — the abstract `layout` interface.
- `layout/measure_mode.hpp` — `measure_mode` and `measure_infinite_quotum`.
- `layout/dimension.hpp` — `dimension` (the `min`/`max`/`fill`/`length` policy).
- `layout/layout_parameters.hpp` — `layout_parameters`, the per-widget parameters
  (`dims`, `weight`, `align`) that define how the widget is laid out within its parent.
- `layout/linear_layout.{hpp,cpp}` — `linear_layout` (`row` / `column`).
- `layout/pile_layout.{hpp,cpp}` — `pile_layout` (stack/overlay).
- `layout/trivial_layout.{hpp,cpp}` — `trivial_layout` (no-op positioning).
- `layout/size_layout.{hpp,cpp}` — `size_layout` (resize only, no move).
- `widget/widget.{hpp,cpp}` — per-widget layout state, `measure()`, `lay_out()`,
  invalidation.
- `widget/container.{hpp,cpp}` — how a container owns a layout and delegates.
- `gui.{hpp,cpp}` — where the layout pass is kicked off before rendering.

---

## Concepts and principles

A **widget** never knows where it sits. It only ever:

1. **measures** — given some space it is *allowed* to use (the *quotum*) and a
   *mode* per axis, it reports the size it wants to have; and
2. **gets laid out** — its parent container calls `resize()` and `move_to()` on
   it to give it a concrete rectangle.

These two steps map to two virtual methods on `widget`: `widget::measure()`
(overridden per widget to report its natural size) and `widget::lay_out()` /
`widget::on_lay_out()` (invoked to arrange the widget's own contents). `measure()`
is called by the parent's layout during its measure pass; `lay_out()` is invoked
lazily — from `gui::render()` when the widget is layout-dirty — and eagerly from
`resize()`/`on_resize()`.

A **container** owns exactly **one** `layout` object. The layout is the policy
that turns "a list of children, each with a set of layout parameters, and a
rectangle we are allowed to fill" into concrete sizes and positions for those
children. The container does not position children itself; it just forwards its
own rectangle and its children to `layout::measure` / `layout::lay_out`.

The unit of negotiation is the **`dimension`** policy, applied per axis. Layout
parameters are **per widget**: every widget holds a `layout_parameters` value
(with a `dimension` for the x axis, one for the y axis, a `weight`, and an
`align` per axis) that defines how *that* widget is laid out within its parent.

The whole thing is **lazy and idempotent**: nothing is re-computed until a
widget (or one of its ancestors) is marked layout-dirty, and the re-layout
happens on the next `gui::render()` (or eagerly on a resize). See
[the dirty flag](#layout-dirty-flag--invalidation) and
[layout runs before rendering](#layout-runs-before-rendering).

---

## The core types

### `layout` (abstract)

```cpp
class layout {
    // How big does the layout want to be, given a space quotum and a mode per axis?
    virtual vec2 measure(
        const vec2& quotum,                    // space available to the layout
        const r4::vector2<measure_mode>& mode, // per-axis measurement mode
        const_widget_list& widgets             // children to measure
    ) const = 0;

    // Arrange the children inside the given rectangle.
    virtual void lay_out(
        const vec2& container_dims, // the rectangle the layout may fill
        semiconst_widget_list& widgets
    ) const = 0;
};
```

- `measure()` is **read-only**: it must not mutate the children. It returns the
  layout's *desired* size for the given quotum/mode.
- `lay_out()` is where the children are resized and moved.

There are standard layout objects pre-created as shared singletons that you can
use directly (assign them to a container, or use them to build your own):

```cpp
static const utki::shared_ref<layout> trivial;  // no-op positioning
static const utki::shared_ref<layout> size;     // resize only (size_layout)
static const utki::shared_ref<layout> pile;     // pile_layout
static const utki::shared_ref<layout> row;      // linear_layout, horizontal
static const utki::shared_ref<layout> column;   // linear_layout, vertical
```

### `measure_mode`

The measure mode is, together with the quotum, an argument of the `measure()`
method — one mode per axis. It tells the widget how to interpret the quotum for
that axis:

```cpp
enum class measure_mode {
    exactly, // the widget will be resized to this exact size; report it as-is
    at_most  // the widget may be at most this size; report <= the limit
};
```

- **`exactly`** — the quotum for that axis is the final, concrete size. The
  widget must not "grow"; it reports the quotum back.
- **`at_most`** — the quotum is an upper bound. The widget reports the smallest
  size it needs to draw its content properly, clamped to the quotum.

`measure_infinite_quotum` (the maximum representable `real`) is used as the
quotum in `at_most` mode when a layout wants to ask a widget for its *natural,
unclamped* minimal size (i.e. "how big do you really want to be?").

### `dimension`

A `dimension` is a per-axis sizing policy. It is either a *policy*
(`undefined`, `min`, `max`, `fill`) or an *exact length* (`length`). It can
also hold a `styled<length>` (so the value may come from a style).

| type        | meaning                                                                 |
|-------------|-------------------------------------------------------------------------|
| `undefined` | Unset. **Treated as `min`** by all layouts.                            |
| `min`       | Give the widget its natural (measured) size, never less.               |
| `max`       | *Measured* like `min`, but *laid out* like `fill` (stretched).         |
| `fill`      | Take the whole space of the parent. Assumed minimal size is `0`.       |
| `length`    | Take exactly the given (styled) length.                                |

The subtlety worth remembering is the split between **how a dimension is
measured** and **how it is laid out**:

- **`min` / `undefined`** — measured as *natural size*; laid out at that natural
  size (clamped to what is actually available).
- **`max`** — *measured* as `min` (so its natural size is known and contributes
  to the container's reported size), but *laid out* as `fill` (so it is
  stretched to whatever room the layout ends up giving it). `max` is the way to
  say "report my natural size, but also happily take any extra space."
- **`fill`** — contributes **zero** natural size in the measurement phase (it
  is assumed to need nothing), and is stretched to the parent in the lay-out
  phase.
- **`length`** — exact, in both phases.

These two roles (contribute-to-measurement vs. stretch-at-layout) are the core
idea that `min`/`max`/`fill` are built on; see
[how min/max/fill work in general](#how-minmaxfill-work-in-general).

### `layout_parameters`

Every widget holds a `layout_parameters` value; it is the per-widget data that
defines how the widget is laid out within its parent.

```cpp
struct layout_parameters {
    r4::vector2<dimension> dims;      // per-axis sizing policy
    styled<real> weight = -1;         // share of the leftover space
    r4::vector2<styled<align>> align; // per-axis alignment within the parent
};
```

Accessors: `widget::get_layout_params()` (non-const, **invalidates the
layout**) and `widget::get_layout_params_const()`.

---

## How layout is assigned to a container

A `container` holds a single `utki::shared_ref<ruis::layout> layout`. It is
chosen through the container's own `parameters`:

```cpp
struct parameters {
    std::shared_ptr<ruis::layout> layout;
};
```

- If `params.params.layout` is **null**, the container falls back to
  `layout::trivial` (see `container::container` — `if (!params.params.layout)
  return ruis::layout::trivial;`).
- The convenience factories in `namespace ruis::make` just build a `container`
  with a specific layout pre-selected:

  | factory          | layout used              |
  |------------------|--------------------------|
  | `make::container`| whatever you pass (or trivial) |
  | `make::row`      | `layout::row` (horizontal `linear_layout`) |
  | `make::column`   | `layout::column` (vertical `linear_layout`) |
  | `make::pile`     | `layout::pile` |

So "how layout is assigned to a container" is simply: *the container is
constructed with a `layout` in its parameters; if none is given it defaults to
`trivial`*. The container then delegates both phases to it:

```cpp
vec2 container::measure(const vec2& quotum, const r4::vector2<measure_mode>& mode) const
{
    return this->get_layout().measure(quotum, mode, this->children());
}

void container::on_lay_out()
{
    this->get_layout().lay_out(this->rect().d, this->children());
}
```

Because the layout is a plain shared object, you can also write and assign your
own `layout` subclass to a container.

---

## The measure / lay-out double pass

The name "double pass" refers to the fact that sizing a widget tree is done in
two distinct phases that flow in opposite directions:

1. **Measure (bottom-up).** A widget is asked "how big do you want to be given
   this much space?" A leaf widget computes its natural size directly. A
   **container** answers by delegating to `layout::measure`, which recursively
   measures its children. The result is the container's *desired* size.
2. **Lay-out (top-down).** Once a widget knows its own rectangle (e.g. because
   its parent gave it one, or because it is the root sized to the viewport), it
   lays out its **contents**. A container does this via
   `container::on_lay_out()` → `layout::lay_out(rect().d, children)`, which
   **resizes and moves each child**.

The two are stitched together by `widget::resize()`: resizing a child triggers
`on_resize()`, whose default implementation calls `lay_out()` on that child,
which in turn lays out *its* children — so the top-down lay-out recurses down
the whole tree automatically.

```
        root.lay_out()                         (top-down)
          └─ layout.lay_out(rect, children)
               ├─ child.measure(...)          (measure, per child, bottom-up inside)
               ├─ child.resize(d)  ──► child.lay_out() ──► layout.lay_out(...)  (recurse)
               ├─ child.move_to(pos)
               ├─ ...
```

Important detail: inside a concrete layout, **both** `measure` and `lay_out`
are themselves often implemented as two internal passes (linear layout does a
"measure children / compute rigid size" pass and then a "distribute leftover /
place" pass). That is an implementation detail on top of the global
measure→lay-out protocol.

`widget::measure_within_parent()` is a helper used by some layouts (and by
widgets that need to resolve their own policy) — it resolves the widget's
`layout_parameters` against a parent's dimensions and mode and reports the
resulting rectangle, clamping `min`/`undefined`/`max` to the parent and using
`fill`/`length` as exact sizes.

---

## How `min` / `max` / `fill` work in general

Both phases behave as follows for each policy (this is the general rule that
every layout follows):

| policy     | during **measure**                              | during **lay-out**                              |
|------------|-------------------------------------------------|--------------------------------------------------|
| `min` / `undefined` | natural size (`at_most` against available space) | given its natural size, **clamped** to the space actually available |
| `max`      | measured as `min` (natural size)                | stretched to the space the layout gives it (like `fill`) |
| `fill`     | contributes `0` natural size                    | stretched to the whole space of the parent       |
| `length`   | the exact length                                | the exact length                                 |

So:

- **`min`** = "I need exactly my natural size, no more, no less" (but never
  more than what exists).
- **`max`** = "tell my parent my natural size, but at layout time give me
  however much room you can" — the flexible citizen whose *baseline* is its
  natural size.
- **`fill`** = "I have no opinion on my natural size (0), just fill the space".
- **`length`** = "exactly this much, full stop".

The reason `max` and `fill` differ at all is precisely this: `max` still
*measures* as `min` (so it reserves its natural space in the parent's total),
whereas `fill` measures as `0`. Everything else about how a layout distributes
space keys off these two roles.

---

## `weight`

`weight` controls how the **leftover space** (the space that is left over after
every child's non-flexible demand is satisfied) is shared.

- `weight <= 0` (the default is `-1`, which means "undefined") → the widget
  takes **none** of the leftover space. Negative values are clamped to `0`.
- `weight > 0` → the widget receives a **share of the leftover space
  proportional to its weight** relative to the sum of all positive weights in
  the line.

In a linear layout, "leftover space" is `container_size - rigid`, where
`rigid` is the sum of the measured longitudinal sizes of the `min`/`length`
(and `max`, which has no rigid baseline — it is laid out as `fill`) children.
The shares are computed with `flexible * weight / net_weight`, floored per
child, with the fractional remainder carried forward and (in `lay_out`)
handed to the last widget so the line exactly fills the container.

Only the linear layout uses `weight`. The pile and size layouts ignore it: they
size and position each child directly against the whole rectangle, so there is
no "leftover space" to distribute.

---

## `align`

`align` is a per-axis value (`front` / `center` / `back`, with `undefined`
meaning `center`). It positions a widget **within the space assigned to it**.

- In **linear layouts** the alignment along the **longitudinal** (line) axis is
  ignored, because the widget is always stretched to fill its whole slot in
  that direction. Only the **transverse** axis alignment is used.
- In **pile / size layouts** the alignment is applied in **both** axes.

---

## Linear layout (`row` / `column`) in depth

`linear_layout` arranges the children in a single line. It is `oriented`, so it
knows its **longitudinal** axis (the direction of the line) and its
**transverse** axis (the other one):

- `column` → longitudinal = y, transverse = x
- `row`    → longitudinal = x, transverse = y

(`oriented::get_long_index()` / `get_trans_index()` give the component indices.)

### Space assigned to a child (longitudinal)

For the direction *along* the line, the room a child gets is:

- its **measured size** for `min`, `length`, `undefined`;
- **zero** for `fill` and `max` (they have no rigid baseline);
- **plus** its weighted share of the leftover space, if `weight > 0`;
- **clamped** to the space actually remaining in the container, so a child can
  never push the line past the container's edge.

### Space assigned to a child (transverse)

For the direction *across* the line:

- `fill` / `max` → stretched to the full container size in that axis;
- `min` / `undefined` → its natural size, **clamped** to the container size;
- `length` → the specified length.

### The measured size of the container

The container's natural size **along the line** is the sum of the measured
longitudinal sizes of the `min`, `length` and `undefined` children — `fill` and
`max` children contribute **zero** to that sum. **Across the line**, the
container's natural size is the largest measured transverse size among the
children (a `fill` transverse child does not determine the height; it is
measured in a second pass with the resolved transverse size).

### `lay_out` (two internal passes)

**Pass 1 — measure and accumulate.** For each child it resolves the child's
transverse dimension and longitudinal dimension, measures the child to get its
natural sizes, and records them. Along the way it accumulates:

- `rigid` — the sum of the longitudinal sizes of the children that *do* have a
  rigid baseline (`min`, `length`, `undefined`; not `fill`, not `max`);
- `net_weight` — the sum of the clamped weights.

**Pass 2 — distribute and place.** It computes `flexible = container - rigid`,
then walks the children assigning each one a longitudinal slot:

```
long_room = measured longitudinal size        // 0 if the child is 'max'
if (weight > 0 && flexible > 0)
    long_room += floor(flexible * weight / net_weight)   // + fractional carry
long_slot = min(long_room, container - pos)   // never overflow the container
```

The child is `resize()`d to `(long_slot, transverse size)` and `move_to()`d
with its transverse alignment applied. `pos` advances by `long_slot`. Any
fractional leftover from the weight division is handed to the **last** widget
so the whole line is filled exactly.

### `measure` (two internal passes)

**Pass 1** resolves the transverse (e.g. height) by measuring every child whose
transverse dimension is *not* `fill` (a `fill` child does not determine the
height). For the longitudinal dimension it measures `min`/`max`/`undefined` as
`at_most` (natural size) and `fill` as `exactly 0`.

**Pass 2** re-measures the `fill`/`max` transverse children using the transverse
size found in pass 1, so their longitudinal contribution is correct.

Finally it sums the measured longitudinal sizes into `rigid_length`, and, when
measured as `exactly`, distributes `quotum - rigid_length` among the weighted
children (again with floor + remainder to the last) to produce the container's
reported size.

---

## Pile layout

`pile_layout` stacks all children on top of each other (an overlay). There is no
line: every child is laid out against the *whole* rectangle.

- **`lay_out`** — each child is resized to
  `measure_within_parent(container_dims, exactly)` (so `fill`/`max` stretch to
  the container, `min`/`undefined` get their natural size clamped to the
  container, `length` uses its length) and then positioned per its `align` in
  **both** axes.
- **`measure`** — the pile's desired size per axis is the **maximum**, over the
  children, of their resolved sizes (a child that is `fill` in *both* axes does
  not constrain the size).

`make::pile` builds a container with this layout.

---

## Trivial layout and size layout

- **`trivial_layout`** — `lay_out` only calls `lay_out()` on each dirty child
  (no positioning, no resizing of children by the layout itself). `measure`
  reports the bounding extent of the children, clamped to the quotum. This is
  the **default** for a container that has no explicit layout, and it is the
  base class of `size_layout`.

- **`size_layout`** (extends `trivial_layout`) — only **resizes** children
  according to their layout parameters; it **never moves** them. `lay_out`
  resizes each dirty child to `measure_within_parent(dims - child_pos,
  exactly)`, clamping content so it does not exceed the layout bounds. It is
  used where children are positioned absolutely and only their sizing is
  governed (e.g. `ruis::overlay`).

---

## Layout dirty flag & invalidation

Every widget carries a `bool layout_dirty` (initially `true`). It means "the
contents of this widget need to be (re)laid out." The two invalidation
directions are:

- **`invalidate_layout()`** — marks **this widget** dirty and **bubbles up** to
  the parent (so the whole chain up to the root becomes dirty). It also clears
  the render cache. Called when a widget's own layout-relevant state changes
  (e.g. you call the non-const `get_layout_params()`, or `reload()`), and by
  `container::on_children_change()` (so inserting / erasing / reordering
  children invalidates the container's layout).
- **`invalidate_subtree_layout()`** — marks **this widget and all its
  descendants** dirty (top-down), but does **not** touch the ancestors. This is
  for changes that affect a parameter the descendants depend on but which is not
  itself a widget (e.g. live window insets referenced by a padding). The default
  implementation marks only the widget; `container` overrides it to also mark
  its children.

The distinction matters because layouts **skip clean children** when laying out
a parent: if you only invalidate a parent, the descendants are assumed up-to-
date and are not re-measured. Hence `invalidate_subtree_layout()` exists to push
the dirtiness *down*.

The re-layout itself is:

```cpp
void widget::lay_out()
{
    this->clear_cache();
    this->layout_dirty = false;
    this->on_lay_out();          // container: calls layout.lay_out(rect, children)
}
```

And resizing is wired to it: `widget::resize()` sets the new rectangle and, if
it changed, calls `on_resize()` whose default implementation calls `lay_out()`.
So any resize immediately lays the widget out (and, for containers, recurses).

---

## Layout runs before rendering

The guarantee that a widget tree is always laid out before it is drawn comes
from `gui::render()`:

```cpp
void gui::render(const mat4& matrix) const
{
    if (this->get_root().is_layout_dirty()) {
        this->root_widget.get().lay_out();   // one top-down re-layout pass
    }
    // ... build the viewport matrix ...
    this->get_root().render_internal(m);
}
```

So the flow each frame is: if the root (or anything reachable from it) is
dirty, a single `lay_out()` runs from the root down, recursively laying out the
entire tree (via the resize → `on_resize` → `lay_out` chain and each container's
`layout.lay_out`), and only *then* is the tree rendered. Individual widgets also
refuse to render if their rectangle is not positive (`render_internal` returns
early on `!rect().d.is_positive()`), which is what a not-yet-laid-out widget
looks like.

Two things feed the dirty state that `render()` then resolves:

- **`set_root()` / `set_viewport()`** resize the root widget, which (via
  `on_resize`) lays it out.
- **Any structural or parameter change** that calls `invalidate_layout()` (or
  `invalidate_subtree_layout()`) marks the path dirty, so the *next* render
  re-lays it out.

(The current `gui::render` is `const` but calls the non-const `lay_out()`; there
is a TODO in the source about fixing that.)
