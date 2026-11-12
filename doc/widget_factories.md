# Widget factories

This document describes the convention for widget factory functions
(`ruis::make::...`) and how applications combine the factories of the
widgets they use into their own `m::` namespace, so that building a view
reads as a simple, uniform expression tree.

The parameter structs that factories take (`all_parameters` etc.) are
described in [Widget parameters](widget_parameters.md).

Key source files (all under `src/ruis/widget/`):

- `button/push_button.{hpp,cpp}` — a factory of an *abstract* widget
  (`ruis::make::push_button`) which constructs a default impl.
- `button/impl/nine_patch_push_button.{hpp,cpp}` — a *final* widget in an
  `impl/` subdirectory with its `ruis::make::` factory and a
  `ruis::make::mapping::` factory (which loads the default nine-patch
  images from the resource loader).
- `button/impl/rectangle_push_button.{hpp,cpp}` — a final widget which
  additionally provides a `ruis::make::mapping::` factory named after the
  abstract widget it implements.
- `label/nine_patch.hpp` — a fundamental final widget whose implementation
  lives outside of `impl/`, with an `inline` `ruis::make::` factory.
- `container.hpp` — `ruis::make::container` plus the `ruis::make::row` /
  `column` / `pile` convenience factories.
- `input/text_field.cpp` — an abstract factory forwarding the parameters
  to a default impl.
- `group/touch/dialog.hpp` — a factory of a widget living in the
  `ruis::touch` sub-namespace (`ruis::touch::make::dialog`).

---

## The convention

A factory is a free function which:

- is named **after the widget class** it constructs (`push_button`,
  `text`, `gap`, …);
- lives in a **`make` namespace inside the namespace of the widget class**
  (`ruis::make` for classes in `ruis`, `ruis::touch::make` for classes in
  `ruis::touch`, `my_app::make` for app-specific widgets);
- takes the ruis context as its first argument:
  `const utki::shared_ref<ruis::context>& context`;
- takes the widget's `all_parameters` aggregate **by value** (named
  `params`) and moves the members out of it;
- may take additional arguments for the widget's *content* (e.g.
  `widget_list children`, `ruis::string text`) — these values could be a
  part of the parameters struct, but initializing them inside the struct
  would be too verbose, so they are passed as plain arguments to keep the
  call sites terse;
- returns a `utki::shared_ref<...>`: the **concrete type** for factories
  of final widgets, or the **abstract base type** for factories of
  abstract widgets (below).

The factory is declared in the same header as the widget class, either
defined `inline` there (most common for final widgets, e.g.
`nine_patch`, `nine_patch_push_button`) or declared in the header and
defined in the `.cpp` (common when the factory has logic, e.g.
`push_button`, `dialog`).

Example, a factory of a final widget (from
`src/ruis/widget/label/nine_patch.hpp`):

```cpp
namespace make {
inline utki::shared_ref<ruis::nine_patch> nine_patch(
    const utki::shared_ref<ruis::context>& context, //
    nine_patch::all_parameters params,
    widget_list children = {}
)
{
    return utki::make_shared<ruis::nine_patch>(
        context, //
        std::move(params),
        std::move(children)
    );
}
} // namespace make
```

---

## Factories of final widgets

Most *final* (concrete, non-abstract) widgets live in an `impl/`
subdirectory of their category (`button/impl/`, `input/impl/`). The
exception are very fundamental widgets which do not need abstraction —
their final implementation lives directly in the category directory
(e.g. `nine_patch`, `text`, `gap`, `rectangle` in `label/`).

The factory of a final widget simply wraps the constructor:

```cpp
// src/ruis/widget/button/impl/nine_patch_push_button.hpp
namespace make {
inline utki::shared_ref<ruis::nine_patch_push_button> nine_patch_push_button(
    const utki::shared_ref<ruis::context>& context, //
    nine_patch_push_button::all_parameters params,
    widget_list children
)
{
    return utki::make_shared<ruis::nine_patch_push_button>(
        context, //
        std::move(params),
        std::move(children)
    );
}
} // namespace make
```

The return type is the concrete class, so the caller gets access to its
specific API.

---

## Factories of abstract widgets

A widget can also be *abstract* in the sense of how it is used in the UI:
a `push_button` is what the GUI needs, but how it is actually implemented
can vary. Therefore the GUI construction phase operates with the
*abstract* widget instead of a concrete impl, and which concrete impl is
constructed is decided by how the application's `m::` namespace is
composed (see below): the `m::` namespace can be tuned to select certain
concrete impls without touching the view code.

Under the hood, the factory of an abstract widget delegates to the
**mapping factory** of the default impl (in the `make::mapping`
sub-namespace, see below), which moves the given parameters into the
impl's `all_parameters` (which has a different, deeper shape), applies
any defaults, and returns the abstract type:

```cpp
// src/ruis/widget/button/push_button.cpp
utki::shared_ref<ruis::push_button> ruis::make::push_button( //
    const utki::shared_ref<ruis::context>& context,
    ruis::push_button::all_parameters params,
    widget_list children
)
{
    return mapping::rectangle_push_button::push_button( //
        context,
        std::move(params),
        std::move(children)
    );
}
```

```cpp
// src/ruis/widget/button/impl/rectangle_push_button.cpp
utki::shared_ref<ruis::push_button>
ruis::make::mapping::rectangle_push_button::push_button( //
    const utki::shared_ref<ruis::context>& context,
    ruis::push_button::all_parameters params,
    ruis::widget_list contents
)
{
    if (!params.params.layout) {
        // pile layout by default
        params.params.layout = ruis::layout::pile;
    }

    return make::rectangle_push_button( //
        context,
        {
            .layout_params = std::move(params.layout_params),
            .widget = std::move(params.widget),
            .params{
                .rectangle_button{
                    .rectangle{
                        .padding{
                            .container = std::move(params.params)
                        }
                    }
                }
            }
        },
        std::move(contents)
    );
}
```

(`ruis::make::text_field` -> `rectangle_text_field` and
`ruis::make::labeled_text_field` -> `labeled_rectangle_text_field` do
not go through a mapping factory yet and construct the default impl
directly.)

Notes:

- The default impl may rely on resources and defaults of the
  implementation (nine-patch images loaded from the resource loader, a
  default layout, …); the caller of the abstract factory does not see or
  configure the impl's specific parameters — that is the point: code
  written against the abstract type keeps working if the default impl is
  swapped.
- If the caller wants a *particular* impl, it calls that impl's factory
  directly (`ruis::make::rectangle_push_button`, …); the impl's
  `all_parameters` is then fully visible and settable.
- The short container factories `ruis::make::row` / `column` / `pile` are
  convenience factories which substitute some default values to the
  params — the layout in this case: they take a plain
  `widget::all_parameters` plus `children`, preset the `container`'s
  layout and delegate to `ruis::make::container`, returning
  `utki::shared_ref<ruis::container>`.

---

## Combining factories in an application: the `m::` namespace

Applications do not call `ruis::make::text(...)` etc. all the time.
Instead they define their own small **`m`** namespace which *brings in*
the factories of the widgets the app actually uses, and the view code
then reads uniformly as `m::...` (as an abstract application does in a
small app header such as `gui/style.hpp`):

```cpp
namespace my_app::m {
    // this imports all the factories from ruis::make
    using namespace ruis::make;

    // these import and obscure the previous import one by one:
    // the touch variants of these widgets are what this app uses
    using ruis::touch::make::tabbed_book;
    using ruis::touch::make::tab_button;
    using ruis::touch::make::list;
    using ruis::touch::make::selection_box;
    using ruis::touch::make::progress_bar;
} // namespace my_app::m
```

The conventions:

- One `m` namespace per app (usually defined in a small app header such
  as `gui/style.hpp`, or in the main `gui.cpp`), inside the app's
  namespace (`my_app::m`, `another_my_app::m`, …).
- `using namespace ruis::make;` imports the whole plain make namespace at
  once.
- For sub-namespace factories (`ruis::touch::make`), bring in **selected
  names with individual `using` declarations**, which obscure the
  corresponding imports from `ruis::make` one by one: several names exist
  in both `ruis::make` and `ruis::touch::make` (e.g. `list`,
  `scroll_bar`), so a blanket `using namespace` of both would make those
  names ambiguous — while the app wants exactly the touch variant of
  those widgets.
- App-specific widgets get app-level `make` factories
  (`my_app::make::...`), and the app's `m` brings those in as well:
  `using namespace ruis::make; using namespace my_app::make;`. The same
  mechanism works for overriding an abstract factory: an app can provide
  its own `my_app::make::push_button` constructing a particular impl, and
  the view code's `m::push_button(...)` then constructs the app's chosen
  impl. Impls ease this by providing a factory named after the abstract
  class, taking the abstract widget's `all_parameters` (just like the
  default abstract factory, so call sites keep the same shape) and
  returning the abstract type, in a `make::mapping` sub-namespace (e.g.
  `ruis::make::mapping::rectangle_push_button::push_button`); the app
  selects it with a single using declaration in `m`
  (`using ruis::make::mapping::rectangle_push_button::push_button;`), which
  shadows the `ruis::make::push_button` imported via `using namespace`.

With the `m` namespace in place, a view is a plain, uniform expression
tree:

```cpp
// in the view code, 'c' is the ruis context
auto dialog = ruis::touch::make::dialog(c,
    {
        .layout_params{
            .dims = { ruis::dim::fill, ruis::dim::fill }
        }
    },
    {
        ruis::touch::make::scroll_area(c,
            {},
            m::column(c,
                {
                    .layout_params{
                        .dims = { ruis::dim::fill, ruis::dim::min }
                    }
                },
                {
                    m::text(c, {}, U"title"),
                    m::row(c,
                        {
                            .layout_params{
                                .dims = { ruis::dim::fill, ruis::dim::min }
                            }
                        },
                        {
                            m::text(c, {}, U"left"),
                            m::gap(c, {}),
                            m::text(c, {}, U"right")
                        }
                    )
                }
            )
        )
    }
);
```
