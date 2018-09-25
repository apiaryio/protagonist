
const fs = require('fs');
const path = require('path');
const assert = require('chai').assert;
const protagonist = require('./protagonist');

const data = fs.readFileSync(path.join(__dirname, 'fixtures', 'sample-api.apib'), 'utf8');
const sync_parsed = JSON.parse(fs.readFileSync(path.join(__dirname, 'fixtures', 'sample-api-refract.json'), 'utf8'));

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
