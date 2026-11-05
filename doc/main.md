# ruis documentation

Hand-written documentation for *ruis*. Generated API documentation (Doxygen)
lives in the separate `doxygen/` directory.

## Topics

- [Layout](layout.md) — layouting concepts and principles, the
  measure / lay-out double pass, how layout is assigned to a container, the
  `min` / `max` / `fill` / `length` dimension policies and `weight`, how the
  linear (row/column) and pile layouts work, the layout dirty flag, and how
  layouting is driven before rendering the widget tree.
- [Widget probe (tree inspection)](probe.md) — the `widget::probe()` query-style
  API for inspecting the widget tree at runtime for debugging and testing.
