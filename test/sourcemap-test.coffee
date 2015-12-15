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
      return done err if err

      protagonist.parse data, { type: 'ast', generateSourceMap: true }, (err, result) ->
        return done err if err

        sourcemap_parsed = result.sourcemap
        done()

  # Parser Sourcemap should conform to recent source map serialization JSON media type
  it '`sourcemap` field conforms to `vnd.apiblueprint.sourcemap+json; version=4.0`', ->
    assert.deepEqual sourcemap_fixture, sourcemap_parsed

describe "Parser sourcemap with old option name", ->

  sourcemap_fixture = require './fixtures/sample-api-sourcemap.json'
  sourcemap_parsed = null

  # Read & parse blueprint fixture
  before (done) ->

    fixture_path = path.join __dirname, './fixtures/sample-api.apib'

    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      protagonist.parse data, { type: 'ast', exportSourcemap: true }, (err, result) ->
        return done err if err

        sourcemap_parsed = result.sourcemap
        done()

  # Parser Sourcemap should conform to recent source map serialization JSON media type
  it '`sourcemap` field conforms to `vnd.apiblueprint.sourcemap+json; version=4.0`', ->
    assert.deepEqual sourcemap_fixture, sourcemap_parsed
