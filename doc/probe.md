# Widget probe (tree inspection)

## Purpose

`widget::probe()` is a query-style API for inspecting the state of the widget
tree **at runtime**, without attaching a debugger or instrumenting the
application. It is intended for debugging and for automated testing:

- **Debugging** — a developer (or a debug tool) can ask a widget "what is your
  current state?", "how many children do you have?", "run this command on the
  widget over there" and get a structured answer.
- **Testing** — because the response is a deterministic, structured value (not
  text), tests can assert on it precisely. The unit tests in
  `tests/unit/src/probe/` rely on this.

The whole mechanism is deliberately small: one virtual method, one command
type, one response type.

## The two halves

```cpp
// defined in ruis/widget/widget.hpp
virtual probe_response_set probe(const tml::tree& command);
```

- **The command** is a `tml::tree`. Its `value` is the **command name**; its
  `children` are the **parameters** (if the command takes any).
- **The response** is a `probe_response_set` — a *set of sections*.

A **section** is itself a `tml::tree` whose `value` (its name) is the demangled
C++ type name of the class that produced it, e.g. `ruis::widget`,
`ruis::container`. The section's children are the data being reported.

So a probe call is: *give me a named command with parameters, and I'll give
back a collection of named data sections*.

## The response type

```cpp
// ruis/widget/widget.hpp, nested in class widget
struct probe_section_less {
	bool operator()(const tml::tree& a, const tml::tree& b) const
	{
		return a.value.string < b.value.string; // by name only, content ignored
	}
};

using probe_response_set = std::set<tml::tree, probe_section_less>;
```

Two consequences of storing sections in a `std::set` keyed **by name only**
(the section content is ignored when comparing):

1. **Sections are identified by name.** Inserting a section whose name is
   already present is a no-op; the first inserted section wins.
2. **Multiple inheritance is handled for free.** In a (virtual) diamond
   hierarchy a shared base class can contribute the same section more than
   once (once per inheritance path). The set keeps a single copy, so the
   response never contains duplicate sections.

There are **no** comparison operators on `tml::tree` / `tml::leaf` for this —
the ordering is provided by the dedicated `probe_section_less` comparator.

## Fold and non-fold commands

Every command is dispatched through the **same single mechanism**: a class that
participates in a command overrides `probe()`, **calls the base implementation
first**, and then adds its own section to the result:

```cpp
probe_response_set color_widget::probe(const tml::tree& command)
{
    probe_response_set ret = widget::probe(command); // base sections
    if (command.value == "get_state") {
        ret.insert(/* this class's section */);
    }
    return ret;
}
```

So a response is always the **accumulation of every section that any class in
the inheritance chain contributes** for that command. A class that does not
handle the command simply contributes nothing.

There is **no principal difference** between the commands — *fold* and
*non-fold* are only a **convention** that describes how many classes in the
hierarchy handle a given command:

- A **fold** command (e.g. `get_state`) is handled by **many** classes in the
  chain, so the response accumulates one section per class. Running it on a
  `color_widget` returns *both* the `ruis::widget` section and the
  `ruis::color_widget` section.
- A **non-fold** command (e.g. `get_children`) is handled by **exactly one**
  class (here `container`); every other class in the chain ignores it and
  contributes nothing, so the response contains that single section only.

**Unknown commands** are ignored and result in an **empty** response (an empty
set). This is the base behaviour; a derived class that does not recognize the
command simply delegates upward and the chain ends empty.

## Fundamental commands

`widget` and `container` are the two fundamental classes in the hierarchy, and
the commands they handle are the important ones — everything else in the
library is built on top of these:

- `get_state`, handled by `widget`, is the command every widget implements (or
  inherits) to report its own state.
- `get_children` and `for`, handled by `container`, are for inspecting the
  widget tree: how many children a widget has, and reaching a specific widget
  in order to run a command on it.

### `get_state` (handled by `widget`)

No parameters. Adds a section named after the widget's class reporting the
widget's state:

| field          | meaning                                                    |
| -------------- | ---------------------------------------------------------- |
| `id`           | the widget's id                                            |
| `layout_params`| layout parameters (see value formatting below)             |
| `rect`         | the widget's rectangle (position `p` and size `d`)         |
| `layout_dirty` | whether a layout pass is pending                           |
| `clip`         | whether clipping is enabled                                |
| `visible`      | whether the widget is visible                              |
| `enabled`      | whether the widget is enabled                              |
| `depth`        | whether depth testing is enabled                           |
| `focused`      | whether the widget has keyboard focus                      |
| `cache`        | render cache state: `enabled`, `dirty`, `present`          |

`cache.present` means a render-cache frame buffer currently exists for the
widget. `cache.dirty` means the cached content is stale.

### `get_children` (handled by `container`)

No parameters. Returns a single `ruis::container` section reporting the number
of child widgets:

```
ruis::container {
	children_count { 3 }
}
```

### `for` (handled by `container`)

Run a probe command on **another** widget in the tree, addressed from the
container the command is run on. The `for` node has exactly two children: an
**addressing node** and the **command** to run. The addressing node is either
an `index` node or an `id` node.

#### Addressing by child index

```
for {
	index { <i0> <i1> ... }   <command>
}
```

`i0` is an index into the children of this container, `i1` is an index into
the children of the widget reached by `i0`, and so on. Each intermediate widget
must be a container.

#### Addressing by widget id

```
for {
	id { <id0> <id1> ... }   <command>
}
```

`id0` is looked up with `try_get_widget()` on this container, `id1` is looked
up on the widget reached for `id0`, and so on. Because `try_get_widget()`
searches the whole sub-hierarchy, each step can reach any descendant, but the
search is **restricted to the widget reached by the previous step**.

An **empty** addressing node (`index { }` or `id { }`) means *this container
itself*.

On success the response is **exactly the probe response of the target widget**,
returned as is (not wrapped in any extra section).

#### Error rules

A malformed command format results in a single `ruis::container` section with
an `error { malformed_command }` node. Malformed means: no addressing node,
**both** an `index` and an `id` node, more than one of either, more than one
command, no command, or (index variant) an index value that cannot be parsed as
a number.

If a path step fails, the response is a single `ruis::container` section with
an `error` node carrying the addressed-path prefix (including the failing
step), a reason node, and — for `out_of_bound` — the actual number of children:

```
ruis::container {
	error { <index | id> { <path prefix> } <reason> [ <size> ] }
}
```

The reason is:

| reason            | variant | meaning                                                       |
| ----------------- | ------- | ------------------------------------------------------------- |
| `not_found`       | id      | no widget with that id under the widget reached so far        |
| `not_a_container` | index   | the widget at the step is not a container                     |
| `out_of_bound`    | index   | the index exceeds the number of children (size is reported)   |

## Value formatting

Leaf values in the response are produced by the `ruis::to_tml(...)` free
functions (`ruis/util/to_tml.hpp` and friends). The conventions:

| value type      | tml form                                            |
| --------------- | --------------------------------------------------- |
| number          | a decimal leaf, e.g. `42`, `1.5`                    |
| bool            | `true` / `false`                                    |
| 2/3/4-vector    | positional value nodes in order, no names, e.g. `1 2` |
| rectangle       | `rect { p { .. .. } d { .. .. } }` (p, d are positional value nodes) |
| `dimension`     | `min`, `max`, `fill`, `undefined`, or `<len><unit>` (e.g. `10px`) |
| `align`         | `front`, `center`, or `back`                        |
| `color`         | `rgba { r g b a }` (channel values 0–255)           |
| styled value sourced from a style | wrapped in `style_ref { .. }`            |

The `style_ref` wrapper is meaningful: it records *where* a value came from. A
colored widget reports the **styled** value, so a colour inherited from the
style sheet shows up as `style_ref { rgba { .. } }` rather than as a bare
`rgba { .. }`.

## Examples

All values below are from a freshly created widget (defaults).

### `get_state` on a plain container

`container` is a plain widget (no colour state), so a fold command returns the
single `ruis::widget` section:

```
ruis::widget {
	id { test-container }
	layout_params {
		dims {
			min
			10px
		}
		weight { 1.5 }
		align {
			center
			front
		}
	}
	rect {
		p {
			0
			0
		}
		d {
			-1
			-1
		}
	}
	layout_dirty { true }
	clip { false }
	visible { true }
	enabled { true }
	depth { false }
	focused { false }
	cache {
		enabled { false }
		dirty { true }
		present { false }
	}
}
```

### `get_state` on a colored widget

A `color_widget` (e.g. a `gap`) returns **two** sections, sorted by name: the
base `ruis::widget` section *and* the `ruis::color_widget` section. If the
colour is sourced from the style sheet it is wrapped in `style_ref`:

```
ruis::color_widget {
	color {
		style_ref {
			rgba { 1 2 3 255 }
		}
	}
}
ruis::widget {
	id { ... }
	...
}
```

### `for` by id, targeting a nested widget

Given a container `c` whose child `c2` contains a gap with id `g21`:

```
for {
	id { c2 g21 }
	get_state
}
```

`c2` is looked up on `c`, then `g21` on `c2`; the response is the `get_state`
response of `g21` as is (a gap is a colored widget, so two sections).

### `for` error: id not under the reached widget

Given `c` with two children `L` and `R`, where `b` is under `R` (not under
`L`), the path `id { L b }` fails at the second step:

```
ruis::container {
	error {
		id { L b }
		not_found
	}
}
```

## Using it from C++

`probe()` is a **non-const** member: some commands can modify the widget state,
so the method cannot be const-qualified. A widget is usually obtained through a
`gui` (e.g. `gui.get_root()`) or held as a shared reference:

```cpp
#include <ruis/widget/widget.hpp>
#include <tml/tree.hpp>

// build a command: value = command name, children = parameters
tml::tree cmd(tml::leaf("get_state"));

auto& root = gui.get_root();
auto resp = root.probe(cmd);           // widget::probe_response_set

// resp is a std::set<tml::tree> ordered by section name;
// each element is one section (a tml::tree).
for (const auto& section : resp) {
	// section.value  -> section name, e.g. "ruis::widget"
	// section.children -> the reported data
}

// a section can be rendered to a string with tml::to_string():
// tml::to_string(tml::forest(resp.begin(), resp.end()));
```
