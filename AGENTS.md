# AGENTS.md

## Purpose

This repository contains Hugin, the C++ testing DSL for inspecting and interacting with Munin user interfaces.

## Working Rules

1. Hugin tests must use the library as a user would: through public API only. Direct test use of an entity marks that entity as part of the public API from that point forward.
2. Tests may expand the public API, but only by directly expressing a user-facing need. When a test first names or calls a new entity, treat that as an intentional API proposal and make the implementation match that public shape.
3. Entities not directly used by user-facing tests are not public API, even if they appear in headers for template or compilation reasons. Keep them in `hugin::detail`, private members, or `.cpp` files where possible.
4. Put non-template function definitions in `src/*.cpp`; keep templates and concepts that callers must instantiate in headers.
5. When adding a new `.cpp` file under `src/`, register it in `target_sources(hugin PRIVATE ...)` in `CMakeLists.txt`.
6. Public API documentation follows the Munin Doxygen banner style:

   ```cpp
   //* =========================================================================
   /// \brief ...
   //* =========================================================================
   ```

7. Document public top-level classes and functions with a brief description and a short usage guide where useful.
8. Document public member functions briefly. Usage guides are not required for member functions.
9. Do not document `hugin::detail` entities as public API.
