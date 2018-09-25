fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require './protagonist'

valid_fixture = path.join __dirname, './fixtures/valid.apib'

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
