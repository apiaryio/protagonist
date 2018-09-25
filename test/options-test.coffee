fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require './protagonist'

fixture_path = path.join __dirname, './fixtures/sample-api.apib'
expected_err = require './fixtures/sample-api-error.json'

describe "Requiring Blueprint name with sourcemaps", ->
  refract_err = null

  before (done) ->
    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      protagonist.parse data, { requireBlueprintName: true, generateSourceMap: true }, (err, result) ->
        return done err if err

        refract_err = result
        done()

  it 'conforms to the refract spec', ->
    assert.deepEqual refract_err, expected_err

describe "Requiring Blueprint name without sourcemaps", ->
  refract_err = null

  before (done) ->
    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      protagonist.parse data, { requireBlueprintName: true, generateSourceMap: false }, (err, result) ->
        return done err if err

        refract_err = result
        done()

  it 'conforms to the refract spec', ->
    assert.deepEqual refract_err, expected_err

describe "Requiring Blueprint name with sourcemaps using sync", ->
  refract_err = null

  before (done) ->
    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      refract_err = protagonist.parseSync data, { requireBlueprintName: true, generateSourceMap: true }
      done()

  it 'conforms to the refract spec', ->
    assert.deepEqual refract_err, expected_err

describe "Requiring Blueprint name without sourcemaps using sync", ->
  refract_err = null

  before (done) ->
    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      refract_err = protagonist.parseSync data, { requireBlueprintName: true, generateSourceMap: false }
      done()

  it 'conforms to the refract spec', ->
    assert.deepEqual refract_err, expected_err

describe "Requiring Blueprint name with validate", ->
  refract_err = null

  before (done) ->
    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      protagonist.validate data, { requireBlueprintName: true }, (err, result) ->
        return done err if err

        refract_err = result
        done()

  it 'conforms to the refract spec', ->
    assert.deepEqual refract_err, expected_err

describe "Requiring Blueprint name with validate sync", ->
  refract_err = null

  before (done) ->
    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      refract_err = protagonist.validateSync data, { requireBlueprintName: true }
      done()

  it 'conforms to the refract spec', ->
    assert.deepEqual refract_err, expected_err
