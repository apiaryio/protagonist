{
  "includes": [
    "common.gypi"
  ],
  "targets": [
    {
      "target_name": "protagonist",
      "include_dirs": [
        "./snowcrash/src"
      ],
      "sources": [
        "src/annotation.cc",
        "src/blueprint.cc",
        "src/parse.cc",
        "src/protagonist.cc",
        "src/protagonist.h",
        "src/result.cc"
      ],
      "dependencies": [
        "./snowcrash/snowcrash.gyp:libsnowcrash",
        "./snowcrash/snowcrash.gyp:sundown"
      ]
    }
  ]
}