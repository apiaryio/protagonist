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
        "src/protagonist.h",
        "src/protagonist.cc",
        "src/parse.cc",
        "src/blueprint.cc"
      ],
      "dependencies": [
        "./snowcrash/snowcrash.gyp:libsnowcrash",
        "./snowcrash/snowcrash.gyp:sundown"
      ]
    }
  ]
}