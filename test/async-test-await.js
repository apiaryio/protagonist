var assert, fs, path, protagonist;

fs = require('fs');
path = require('path');
assert = require('chai').assert;
protagonist = require('./protagonist');

const data = fs.readFileSync(path.join(__dirname, './fixtures/sample-api.apib'), 'utf8');
const sync_parsed = protagonist.parseSync(data)
const sync_parsed_sm = protagonist.parseSync(data, { exportSourcemap : true })

describe('Async parse', () => {

  it('will parse with await/async', async function() {
    let res

    try {
      res = await protagonist.parse(data)
    } catch (err) {
      assert.fail()
    }
    assert.deepEqual(res, sync_parsed);

  })

})
