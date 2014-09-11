{
  "includes": [
    "common.gypi"
  ],
  "targets": [
    {
      "target_name": "protagonist",
      "include_dirs": [
        "./snowcrash/src",
        "./snowcrash/ext/markdown-parser/src",
        "./snowcrash/ext/markdown-parser/ext/sundown/src",
        "./snowcrash/ext/markdown-parser/ext/sundown/html",
      ],
      "sources": [
        "src/annotation.cc",
        "src/blueprint.cc",
        "src/sourcemap.cc",
        "src/parse.cc",
        "src/protagonist.cc",
        "src/protagonist.h",
        "src/result.cc"
      ],
      "dependencies": [
        "./snowcrash/snowcrash.gyp:libsnowcrash",
        "./snowcrash/snowcrash.gyp:libmarkdownparser",
        "./snowcrash/snowcrash.gyp:libsundown"
      ]
    }
  ]
}
