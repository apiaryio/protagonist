fs = require 'fs'
path = require 'path'
{assert} = require 'chai'
protagonist = require '../build/Release/protagonist'

describe 'Parser async parse', ->

  it 'will finish after the sync', (done) ->

    syncFinished = false
    sync_parsed = null

    data = fs.readFileSync(path.join(__dirname, './fixtures/sample-api.apib'), 'utf8')

    protagonist.parse data, (err, res) ->
      if syncFinished
        assert.deepEqual res, sync_parsed
        done()
      else
        done new Error('Async finished before sync')

    sync_parsed = protagonist.parseSync(data)
    syncFinished = true
