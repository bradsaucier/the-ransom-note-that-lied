# The Ransom Note That Lied

```yaml
STATUS  : ACTIVE
AUTHOR  : Bradley D. Saucier
COURSE  : SNHU CS-405 - Secure Coding
```

> [!IMPORTANT]
> **BOTTOM LINE UP FRONT**
>
> A C++20 command-line tool that checks whether a note could have been cut from the text it claims as its source.
>

[![C++ CI](https://github.com/bradsaucier/random-note-that-lied/actions/workflows/ci.yml/badge.svg)](https://github.com/bradsaucier/random-note-that-lied/actions/workflows/ci.yml)

## Case file

The claimed source says:

```text
Meet me at the old clock at midnight. Bring the blue folder.
```

The note says:

```text
Meet at dawn. Bring the folder.
```

Most of the story checks out. One word does not. The source never says `dawn`.

```text
Note words examined: 6
Source words examined: 12
VERDICT: THE NOTE LIED
The claimed source cannot supply the following inventory:
1. dawn required=1 available=0
```

Change `dawn` to `midnight`, and the same source clears the check:

```text
Note words examined: 6
Source words examined: 12
VERDICT: POSSIBLE
The claimed source contains enough copies of every note word.
```

Underneath the detective framing, this is a word-frequency comparison wearing a trench coat. The interesting part is not just finding `dawn`. It is deciding what the program should refuse before it trusts either file.

## Try it

You'll need CMake 3.24 or later, a C++20 compiler, and network access the first time you configure the tests. CMake retrieves GoogleTest v1.18.0 and checks the archive against the SHA-256 value in `CMakeLists.txt`.

### Linux or macOS

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DRNTL_WARNINGS_AS_ERRORS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure

./build/ransom-note-checker samples/note_lied.txt samples/claimed_source.txt
./build/ransom-note-checker samples/note_plausible.txt samples/claimed_source.txt
```

### Windows with Visual Studio

```powershell
cmake -S . -B build -DRNTL_WARNINGS_AS_ERRORS=ON
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure

.\build\Release\ransom-note-checker.exe samples\note_lied.txt samples\claimed_source.txt
.\build\Release\ransom-note-checker.exe samples\note_plausible.txt samples\claimed_source.txt
```

The lying sample exits with code 2. That's the program working, not falling over.

CTest discovers 31 cases: 16 analyzer and file-input tests plus 15 portable regression tests based on the original vector assignment. They cover the word inventory, input limits, both sample verdicts, and the boundary cases you would expect.

## Exit codes

| Code | Result | Meaning |
| --- | --- | --- |
| 0 | `VERDICT: POSSIBLE` | The source has enough copies of every note word |
| 1 | Analysis error | An input could not be safely analyzed |
| 2 | `VERDICT: THE NOTE LIED` | At least one required word is unavailable |
| 64 | Usage error | The command did not receive exactly two file paths |

## How it decides

The checker answers one narrow question: does the claimed source contain enough copies of every normalized word in the note?

1. Read both files, refusing anything that is not a regular file or exceeds the size limit.
2. Split the text into ASCII letter-and-digit sequences and convert letters to lowercase.
3. Build frequency maps for the note and source.
4. Compare the required count for each note word with the count available in the source.

It's a frequency problem, not a yes-or-no word lookup. If the note uses `meet` twice, the source needs two copies. Punctuation and whitespace act as separators, and `std::map` keeps shortage reports in a stable sorted order.

A `POSSIBLE` verdict only means the words were available. It says nothing about who wrote the note.

## What it refuses to accept

| Concern | Control | Evidence |
| --- | --- | --- |
| Unexpected paths | Each input must be a readable regular file | File reader and missing-file test |
| Oversized files | Default maximum is 1,048,576 bytes per file | Checks before and during the read |
| Runaway tokenization | Default maximum is 100,000 words per input | Token limit and negative test |
| Unsupported encoding | Bytes outside ASCII are rejected | Tokenizer validation and negative test |
| Empty evidence | Empty notes and empty sources are rejected | Analysis preconditions and tests |
| Repeated-word mistakes | Required and available counts are compared directly | Frequency-map tests |
| Uncontrolled errors | Exceptions become concise messages and exit code 1 | Command-line boundary in `main.cpp` |

This project changed how I think about small input checks. File type, size, encoding, and empty data all look harmless until the code has to state what it will accept.

## Repository map

| Path | Purpose |
| --- | --- |
| `include/ransom_note/` | Public analyzer interface and default limits |
| `src/` | File reader, tokenizer, inventory comparison, and command-line entry point |
| `tests/` | Analyzer tests and vector regression tests |
| `samples/` | One source, one possible note, and one lying note |
| `portfolio/` | CS 405 policies, presentation, coding archives, and reflection |
| `packaging/` | Staging notes and the preserved original GoogleTest submission |
| `scripts/` | Portfolio validation and repeatable archive packaging |
| `.github/workflows/` | Focused CI and manual extended checks |

I kept the original GoogleTest submission as a course artifact, then added portable versions of those behaviors to the CMake test target. That leaves the submitted work intact without making reviewers reconstruct its old Visual Studio project.

## CI

The required `C++ CI` workflow has one Ubuntu GCC job. It configures, builds, and runs CTest for pushes and pull requests. One job, one platform, so a red X points to a useful failure instead of a maze of duplicate jobs.

`Extended Validation` keeps MSVC, Clang sanitizers, and portfolio checks available by manual dispatch. `CodeQL` is manual as well. Workflow permissions are explicit, checkout credentials are not persisted, and external actions are pinned to verified commit SHAs.

A green workflow is evidence, not proof.

## CS 405 portfolio

| Artifact | Location |
| --- | --- |
| Green Pace Secure Development Policy | `portfolio/project-one/Green_Pace_Secure_Development_Policy.docx` |
| Security policy presentation | `portfolio/project-two/Green_Pace_Security_Policy_Presentation.pptx` |
| Presentation script | `portfolio/project-two/Green_Pace_Security_Policy_Presentation_Script.docx` |
| Original GoogleTest submission | `portfolio/coding/CS405_Original_GoogleTest_Suite.zip` |
| Packaged application source | `portfolio/coding/Ransom_Note_That_Lied_Source.zip` |
| Final reflection | `portfolio/reflection/Security_Before_the_Finish_Line.pdf` |
| Accessible reflection text | `portfolio/reflection/Portfolio_Reflection.txt` |

The reflection connects the code and testing decisions to the NIST Secure Software Development Framework, risk assessment, zero-trust concepts, and the SEI CERT C++ Coding Standard. Its title, `Security Before the Finish Line`, captures the main lesson: limits and tests belong in the design, not in a cleanup pass before submission.

## Scope

1. The parser accepts ASCII text only. UTF-8 and other multibyte encodings are rejected.
2. Punctuation is discarded. As far as this checker is concerned, `the note` and `the note!!!` are the same evidence.
3. Word order, grammar, typography, and physical cutout constraints are ignored.
4. The project is an academic demonstration, not a forensic tool.

It can tell you the source never says `dawn`. It can't tell you who wrote the note.

## Primary references

| Source | Portfolio connection |
| --- | --- |
| [NIST SP 800-218, Secure Software Development Framework](https://doi.org/10.6028/NIST.SP.800-218) | Security practices across the development life cycle |
| [NIST SP 800-30 Rev. 1, Guide for Conducting Risk Assessments](https://doi.org/10.6028/NIST.SP.800-30r1) | Risk-informed control selection |
| [NIST SP 800-207, Zero Trust Architecture](https://doi.org/10.6028/NIST.SP.800-207) | Removing implicit trust from access decisions |
| [SEI CERT C++ Coding Standard](https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=88046682) | Secure C++ rules referenced by the course policy |
| [GoogleTest Primer](https://google.github.io/googletest/primer.html) | Test structure and assertions |

## License

The original application source, tests, CMake files, and scripts are available under the MIT License. Course documents and submitted portfolio artifacts remain the academic work of Bradley D. Saucier and are not relicensed for reuse.
