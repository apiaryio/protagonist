var assert, fs, path, protagonist;

fs = require('fs');
path = require('path');
assert = require('chai').assert;
protagonist = require('./protagonist');

const data = fs.readFileSync(path.join(__dirname, './fixtures/valid.apib'), 'utf8');
const sync_parsed = protagonist.parseSync(data)
const sync_parsed_sm = protagonist.parseSync(data, { exportSourcemap : true })

describe('Async parse', () => {

  it('will parse with promise', function() {
    protagonist.parse(data)
      .then(res => {
        assert.deepEqual(res, sync_parsed);
      })
      .catch(err => {
        assert.fail()
      })

  })

})
