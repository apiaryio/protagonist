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
        "./snowcrash/ext/sos/src"
      ],
      "sources": [
        "src/annotation.cc",
        "src/parse.cc",
        "src/protagonist.cc",
        "src/protagonist.h",
        "src/result.cc",
        "src/v8_wrapper.cc"
      ],
      "dependencies": [
        "./snowcrash/snowcrash.gyp:libsnowcrash",
        "./snowcrash/snowcrash.gyp:libmarkdownparser",
        "./snowcrash/snowcrash.gyp:libsundown",
        "./snowcrash/snowcrash.gyp:libsos"
      ]
    }
  ]
}
