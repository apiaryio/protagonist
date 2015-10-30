{
  "includes": [
    "common.gypi"
  ],
  "targets": [
    {
      "target_name": "protagonist",
      "include_dirs": [
        "drafter/src",
        "drafter/ext/snowcrash/src",
        "drafter/ext/snowcrash/ext/markdown-parser/src",
        "drafter/ext/snowcrash/ext/markdown-parser/ext/sundown/src",
        "drafter/ext/snowcrash/ext/markdown-parser/ext/sundown/html",
        "drafter/ext/sos/src",
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [
        "src/annotation.cc",
        "src/options_parser.cc",
        "src/parse_async.cc",
        "src/parse_sync.cc",
        "src/protagonist.cc",
        "src/protagonist.h",
        "src/result.cc",
        "src/v8_wrapper.cc"
      ],
      "dependencies": [
        "drafter/drafter.gyp:libdrafter",
      ]
    }
  ]
}
