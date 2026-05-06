# Project-scoped guidance — Friendly LwM2M Client

This file augments the root `CLAUDE.md`. It tells Claude how to **work** in this repo.

> Working branch: `claude/consolidate-branches-xCb7h`

## How to approach changes

1. **This is C/C++ for embedded targets.** Memory is tight, control flow matters, and undefined behaviour bites. Before introducing a new dependency or allocation pattern, check whether it fits the device-class footprints described in `README.md`.
2. **Don't fork Wakaama in place.** `2305-Wakaama/` is the vendored base. Override behaviour through the wrapper layer, not by editing inside the vendored tree.
3. **DTLS provider abstraction is the contract.** New crypto code goes behind the factory. No `#ifdef OPENSSL` sprinkled through the core client.
4. **OMA-TS is the spec.** Cite the section when changing wire-format or interface semantics.

## Skills to invoke

- `oma-lwm2m-expert` — for any protocol-level question (object semantics, registration/observe flows, version differences, MQTT binding rules).
- `iot-protocols-expert` — for transport- or security-binding work that touches non-LwM2M layers.
- `cpp-coding-standards`, `cpp-testing` — for C/C++ changes; this code lands in production.
- `security-review` — for any DTLS/TLS/OSCORE/credential change.
- `docker-patterns` — only if/when adding container-based dev or test environments.

## Verification before claiming "done"

```bash
# Configure + build
mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -j

# Smoke
./friendly_example_client --list-dtls-providers
./friendly_example_client --help
```

For DTLS / FOTA / MQTT changes specifically: smoke against a known-good server (Leshan or Friendly Server) before claiming done. Build success is necessary, not sufficient.

If the build complains, read `QUICK_REFERENCE_BUILD_FIXES.md` first — most issues are already documented.

## Branch hygiene — `claude/consolidate-branches-xCb7h`

Branch consolidation work. Keep the diff focused. Don't introduce new features under this branch — surface them with `mcp__ccd_session__spawn_task` and land them later on a feature branch.

## Sub-agent patterns

- "Where is this Wakaama symbol used in our wrapper" — Grep across the non-vendored source.
- For multi-file refactors (renaming an object, changing a content-format default) — TodoWrite the file list before editing.

## Sensitive files / never touch casually

- `2305-Wakaama/` — vendored, treat as read-only unless explicitly patching.
- `Friendly_LwM2M_Client_v1.2.2_Technical_Pitch_Deck.html` — customer-facing.
- Any `build*/`, `_build/`, `build.log`, `backup_object_id_update_*/` — generated/transient.
- `Issues/`, `Prompts/` — internal authoring notes.

## Commit style

Conventional commits with OMA/RFC citations where relevant:
- `fix(observe): correct token reuse per RFC 7641 §3.1`
- `feat(object/24): add MQTT broker reconnect backoff`
- `chore(build): silence mbedTLS deprecation warnings`
