# Widget parameters

This document describes the convention for passing parameters to widgets:
how the parameter structs are named and nested, and how *undefined*
parameter values are substituted with defaults in the widget constructor.

Key source files (all under `src/ruis/widget/`):

- `widget.hpp` — the base `widget` class with `widget::parameters` and
  `widget::all_parameters`.
- `label/nine_patch.hpp` — a concrete widget demonstrating all three struct
  tiers (`all_parameters` / `parameters` / `specific_parameters`).
- `input/text_field.hpp` — an intermediate widget with `parameters` next to
  the virtual-base parameters in `all_parameters`.
- `input/impl/labeled_rectangle_text_field.cpp` — a concrete widget whose
  constructor substitutes undefined values with style defaults.

---

## The three parameter structs

Every widget defines up to three nested parameter structs, all
plain-aggregate C++ structs (no constructors, no virtual functions):

- **`all_parameters`** — the *top-level* parameter aggregate of the widget.
  This is the only struct that is passed to the widget constructor and to
  the `ruis::make::widget_name(...)` factory function. It always contains:
  - `layout_params` — a `ruis::layout_parameters` (the widget's layout
    params within its parent),
  - `widget` — a `ruis::widget::parameters` (id, rectangle, clip, cache,
    visibility, enabled, depth),
  - `params` — the widget's own `parameters` struct (below).
  In other words, **the parameters of the virtual bases** (`widget`) are
  lifted out to the top level of `all_parameters`, one member per virtual
  base. As an exception, `layout_params` is kept as a separate top-level
  member: layout parameters are a part of the widget parameters, but they
  are used very often and should be at the top level.
- **`parameters`** — aggregates the parameters of the widget's
  **non-virtual** bases, one named member per base (e.g.
  `ruis::padding::parameters padding;`, `ruis::blending_widget::parameters
  blending;`), plus a `specific_parameters specific;` member.
- **`specific_parameters`** — holds the parameters that are **specific only
  for this (final) class**: the knobs that no base class knows about (e.g.
  `source` nine-patch image, `pressed_color` of a button, `hint` text of a
  text input).

An intermediate (abstract) class that has no parameters of its own yet may
leave the tier out — e.g. `text_field` keeps its single `clear_button`
member directly in `parameters` next to the virtual-base members of
`all_parameters`. Concrete classes should, however, use the full
three-tier layout.

Example, from `nine_patch` (which inherits `virtual widget`,
`blending_widget` and `padding`):

```cpp
class nine_patch :
    public virtual widget, //
    public blending_widget,
    public padding
{
public:
    struct specific_parameters {
        std::shared_ptr<const res::nine_patch> source;
    };

    struct parameters {
        ruis::padding::parameters padding;   // non-virtual base
        ruis::blending_widget::parameters blending; // non-virtual base
        specific_parameters specific;        // this class only
    };

    struct all_parameters {
        ruis::layout_parameters layout_params; // virtual base: layout
        ruis::widget::parameters widget;       // virtual base: widget
        parameters params;
    };

    nine_patch(
        const utki::shared_ref<ruis::context>& context, //
        all_parameters params,
        widget_list children
    );
};
```

Note the rule of thumb: *parameters of virtual bases live at the top level
of `all_parameters`, parameters of non-virtual bases are grouped in
`parameters`, and parameters specific to the final class live in
`specific_parameters`.*

---

## Naming conventions

- The structs are named exactly `parameters`, `specific_parameters` and
  `all_parameters`, nested `public:` inside the widget class.
- Each nested member is named after the base class or widget it carries:
  `widget`, `blending`, `padding`, `label`, `text`, `rectangle`, … For the
  widget's own parameters the member is named `specific`. The layout
  member is the fixed exception: `layout_params`.
- Parameter fields are named after the property they configure:
  `clear_button`, `stroke_width`, `fill_color`, `hint`, `source`.
- Constructors take the `all_parameters` aggregate **by value** and move
  members out of it.
- Each concrete widget is created through a `ruis::make::` factory
  function named after the widget, which also takes
  `all_parameters` by value, e.g. `ruis::make::nine_patch(context, params, children)`.
- Callers initialize `all_parameters` with **designated initializers**
  (`.layout_params`, `.widget`, `.params`, …) so that only the fields of
  interest need to be spelled out.

Usage example:

```cpp
auto field = ruis::make::rectangle_text_field(
    context, //
    {
        .layout_params{
            .dims = { ruis::dim::fill, ruis::dim::min }
        },
        .widget{
            .id = "name_field"
        },
        .params{
            // the text_input params, aggregated in this widget
            .text_input{
                // only this knob set
                .specific = {
                    .hint = "Enter your name"
                }
            }
        }
    },
    {}
);
```
Besides `all_parameters`, the constructor (and the factory) takes
additional arguments for the widget's *content*, e.g. a `widget_list
children` for containers (like `nine_patch`), or a `ruis::string text`
for text widgets (like `text_input` and its decorated variants). These
values could be a part of the parameters struct, but initializing them
inside the struct would be too verbose, so they are passed as plain
arguments to keep the call sites terse.

---

## Undefined values and default substitution

Parameter members are *not* generally initialized with hard-coded default
values in the struct. Instead, a parameter is usually left at its
**undefined** value, and the widget **constructor** substitutes the
undefined values with actual defaults. This keeps a single place where the
defaults are decided (the constructor), and lets the defaults come from the
`ruis::context` — most often from the active style sheet
(`context.get().style()`), so the widget automatically follows the theme.
It also allows the widget to detect whether a value was explicitly set by
the user or not, and substitute the default only for the values that were
not set.

### How "undefined" is expressed

- `styled<T>` values (`styled<ruis::color>`, `styled<ruis::length>`,
  `styled<ruis::align>`, …) hold the default-constructed value, which is
  undefined; check with `value.get().is_undefined()`.
- `std::optional<T>` members are simply empty; check with `!value`.
- Other value types (enums, `dimension`, …) expose an
  `is_undefined()`/`type::undefined` sentinel in the same spirit.

### Where substitution happens

In the **concrete widget's constructor**, before the parameters are
delegated to the base-class constructors or forwarded to child widgets.
Typical form:

```cpp
if (auto& c = params.params.specific.fill_color; c.get().is_undefined()) {
    c = context.get().style().get_color_background();
}
if (auto& l = params.params.specific.stroke_width; l.get().is_undefined()) {
    l = context.get().style().get_len_border();
}
```

(see `widget/input/impl/rectangle_text_field.cpp`).

Rules of thumb:

- Only substitute values that are still undefined; an explicitly provided
  value (or a style reference) always wins.
- Default to style values when a sensible one exists (`get_color_*`,
  `get_len_*`, …); otherwise to a hard-coded constant.
- If a base class's own parameters need defaults, the base substitutes them
  in its own constructor (e.g. `widget` applies defaults to `clip`,
  `cache`, `visible`, `enabled` and `depth`). The final class is
  responsible for the parameters of the non-virtual bases it aggregates and
  for its own `specific_parameters`.
- Document the applied default in the comment of the field, e.g.
  *"Defaults to the 'color_background' style value if undefined."*
