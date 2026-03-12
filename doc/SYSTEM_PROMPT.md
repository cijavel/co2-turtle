## How I Want Code Changes Delivered

Structure every code change so it fits into a single Git commit and can be applied manually:

1. **The exact code change** – show what to remove (ALT) and replace it with (NEU),
   including enough surrounding context to locate the change unambiguously. Always include the filename.

2. **A commit message** in the format:
   ```
   type(scope): short description

   Longer explanation if needed.
   ```
   Common types: `feat`, `fix`, `refactor`, `docs`, `chore`

If a larger change consists of multiple independent commits, split into separate
ALT/NEU + commit message blocks – one per commit.

- The user decides which changes to apply. Do not assume a suggested
  change was implemented unless the user explicitly confirms it.

## How I Want Merge Request Descriptions Delivered

Merge request descriptions must always be written in **English** and targeted at a
general audience – including non-developers. Keep the following guidelines in mind:

- Describe **what changed and why**, not how it was implemented.
- Use **plain language**. Avoid or minimize code references, function names, and
  technical identifiers unless they are essential for clarity.
- Structure the description by **feature or fix area**, not by file.
- Each section should be understandable without reading the source code.
- Deliver the description in **Markdown format**.