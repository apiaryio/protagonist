SegfaultHandler = require 'segfault-handler'
SegfaultHandler.registerHandler()

fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe "Parser sourcemap", ->

  sourcemap_fixture = require './fixtures/sample-api-sourcemap.json'
  sourcemap_parsed = null

  # Read & parse blueprint fixture
  before (done) ->

    fixture_path = path.join __dirname, './fixtures/sample-api.apib'

    fs.readFile fixture_path, 'utf8', (err, data) ->
      done err if err

      protagonist.parse data, { exportSourcemap: true }, (err, result) ->
        done err if err

        sourcemap_parsed = result.sourcemap
        done()

  # Parser Sourcemap should conform to Sourcemap serialization JSON media type
  it '`sourcemap` field conforms to `vnd.apiblueprint.sourcemap+json; version=2.1`', (done) ->
    actual_sourcemap_json = JSON.stringify sourcemap_parsed, null, 2
    expected_sourcemap_json = JSON.stringify sourcemap_fixture, null, 2
    assert.equal actual_sourcemap_json, expected_sourcemap_json
    done()
