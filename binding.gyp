{
  "includes": [
    "common.gypi"
  ],
  "targets": [
    {
      "target_name": "protagonist",
      "include_dirs": [
        "../snowcrash/src"
      ],
      "sources": [
        "src/protagonist.h",
        "src/protagonist.cc",
        "src/parse.cc",
        "src/blueprint.cc"
      ],
      "cflags_cc": [
        "-Wc++11-extensions"
      ],
      "xcode_settings": {
        "OTHER_CPLUSPLUSFLAGS": [
          "-Wc++11-extensions"
        ]
      },
      "dependencies": [
        "../snowcrash/snowcrash.gyp:libsnowcrash",
        "../snowcrash/snowcrash.gyp:sundown"
      ]
    }
  ]
}