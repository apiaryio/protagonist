{
  "includes": [
    "drafter/common.gypi"
  ],
  "targets": [
    {
      "target_name": "protagonist",
      "include_dirs": [
        "drafter/src",
        "<!(node -e \"require('nan')\")"
      ],
      "sources": [
        "src/options.cc",
        "src/options.h",
        "src/options_parser.cc",
        "src/parse_async.cc",
        "src/parse_sync.cc",
        "src/protagonist.cc",
        "src/protagonist.h",
        "src/refractToV8.cc",
        "src/validate_async.cc",
        "src/validate_sync.cc"
      ],
      "conditions" : [
        [ 'libdrafter_type=="shared_library"', { 'defines' : [ 'DRAFTER_BUILD_SHARED' ] }, { 'defines' : [ 'DRAFTER_BUILD_STATIC' ] }],
      ],
      "dependencies": [
        "drafter/drafter.gyp:libdrafter",
      ]
    }
  ]
}
