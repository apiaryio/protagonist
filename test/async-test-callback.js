var assert, fs, path, protagonist;

fs = require('fs');
path = require('path');
assert = require('chai').assert;
protagonist = require('./protagonist');

const data = fs.readFileSync(path.join(__dirname, './fixtures/sample-api.apib'), 'utf8');
const sync_parsed = protagonist.parseSync(data)
const sync_parsed_sm = protagonist.parseSync(data, { exportSourcemap : true })

describe('Async parse', () => {

  it('will parse with callback', function(done) {
    protagonist.parse(data, function (err, res) {
      assert.isNull(err)
      assert.deepEqual(res, sync_parsed);
      done()
    })
  })

})
