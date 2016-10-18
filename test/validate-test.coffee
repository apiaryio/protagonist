fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe "Validate invalid Blueprint - Sync", ->

  fixture = require './fixtures/invalid-api.json'
  parsed = null

  # Read & parse blueprint fixture
  before (done) ->

    fixture_path = path.join __dirname, './fixtures/invalid-api.apib'

    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      parsed = protagonist.validateSync(data, { requireBlueprintName: true })
      done()

  # Result should contain annotations only
  it 'Result contains annotations only', ->
    assert.deepEqual parsed, fixture


describe "Validate valid Blueprint - Sync", ->

  parsed = 1

  before (done) ->

    fixture_path = path.join __dirname, './fixtures/sample-api.apib'

    fs.readFile fixture_path, 'utf8', (err, data) ->
      return done err if err

      parsed = protagonist.validateSync(data)
      done()

  it 'will return null', ->
    assert.isNull parsed


describe "Validate Blueprint with error - Async", ->

  it 'will return error', (done) ->

    parsed = require './fixtures/invalid-api-error.json'
    data = fs.readFileSync(path.join(__dirname, './fixtures/invalid-api-error.apib'), 'utf8')

    protagonist.validate data, { requireBlueprintName: true }, (err, res) ->
      assert.deepEqual res, parsed
      assert.isNull err
      done()


describe "Validate valid Blueprint - Async", ->

  it 'will return null', (done) ->

    data = fs.readFileSync(path.join(__dirname, './fixtures/sample-api.apib'), 'utf8')

    protagonist.validate data, (err, res) ->
      assert.isNull err
      assert.isNull res
      done()


describe "Validate invalid Blueprint - Async", ->

  it 'will finish after the sync', (done) ->

    syncFinished = false
    sync_parsed = null

    data = fs.readFileSync(path.join(__dirname, './fixtures/invalid-api.apib'), 'utf8')

    protagonist.validate data, (err, res) ->
      if syncFinished
        assert.deepEqual res, sync_parsed
        assert.isNull err
        done()
      else
        done new Error('Async finished before sync')

    sync_parsed = protagonist.validateSync(data)
    syncFinished = true
