fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe "Parser AST - Sync", ->

  ast_fixture = require './fixtures/sample-api-ast.json'
  ast_parsed = null

  # Read & parse blueprint fixture
  before (done) ->

    fixture_path = path.join __dirname, './fixtures/sample-api.apib'

    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      ast_parsed = protagonist.parseSync(data, type: 'ast').ast
      done()

  # Parser AST should conform to AST serialization JSON media type
  it '`ast` field conforms to `vnd.apiblueprint.ast+json; version=4.0`', ->
    assert.deepEqual ast_parsed, ast_fixture
