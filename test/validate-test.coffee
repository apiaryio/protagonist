fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require './protagonist'

valid_fixture = path.join __dirname, './fixtures/valid.apib'
warning_fixture = path.join __dirname, './fixtures/warning.apib'
error_fixture = path.join __dirname, './fixtures/error.apib'

warning_refract = require './fixtures/warning.validate.json'
error_refract = require './fixtures/error.json'

describe "Validate Blueprint with error - Sync", ->

  parsed = null

  before (done) ->
    fs.readFile error_fixture, 'utf8', (err, data) ->
      return done err if err

      parsed = protagonist.validateSync(data)
      done()

  it 'Result contains annotations only', ->
    assert.deepEqual parsed, error_refract


describe "Validate Blueprint with warning - Sync", ->

  parsed = null

  before (done) ->
    fs.readFile warning_fixture, 'utf8', (err, data) ->
      return done err if err

      parsed = protagonist.validateSync(data)
      done()

  it 'Result contains annotations only', ->
    assert.deepEqual parsed, warning_refract


describe "Validate valid Blueprint - Sync", ->

  parsed = 1

  before (done) ->
    fs.readFile valid_fixture, 'utf8', (err, data) ->
      return done err if err

      parsed = protagonist.validateSync(data)
      done()

  it 'will return null', ->
    assert.isNull parsed


describe "Validate Blueprint - Async", ->

  it 'will finish after the sync', (done) ->

    syncFinished = false
    sync_parsed = null

    data = fs.readFileSync(valid_fixture, 'utf8')

    protagonist.validate data, (err, res) ->
      if syncFinished
        assert.deepEqual res, sync_parsed
        assert.isNull err
        done()
      else
        done new Error('Async finished before sync')

    sync_parsed = protagonist.validateSync(data)
    syncFinished = true
