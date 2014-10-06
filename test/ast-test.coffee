SegfaultHandler = require 'segfault-handler'
SegfaultHandler.registerHandler()

fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe "Parser AST", ->

  ast_fixture = require './fixtures/sample-api-ast.json'
  ast_parsed = null

  # Read & parse blueprint fixture
  before (done) ->

    fixture_path = path.join __dirname, './fixtures/sample-api.apib'

    fs.readFile fixture_path, 'utf8', (err, data) ->
      done err if err

      protagonist.parse data, (err, result) ->
        done err if err

        ast_parsed = result.ast
        done()

  # Parser AST should conform to AST serialization JSON media type
  #   as defined in https://github.com/apiaryio/api-blueprint-ast#json-serialization
  it '`ast` field conforms to `vnd.apiblueprint.ast.raw+json; version=2.1`', (done) ->
    actual_ast_json = JSON.stringify ast_parsed, null, 2
    expected_ast_json = JSON.stringify ast_fixture, null, 2
    assert.equal actual_ast_json, expected_ast_json
    done()
