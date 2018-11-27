const protagonist = require('./helpers/protagonist');

const path = require('path');
const fs = require('fs');
const valid_fixture = fs.readFileSync(path.join(__dirname, 'fixtures', 'valid.apib'), 'utf8');
const expect = require('chai').expect;
const assert = require('chai').assert;

describe('Protagonist option vulerability', () => {
  it('should not segfault protagonist', () => {
      assert.throws(() => {
        protagonist.parse(valid_fixture, { get requireBlueprintName() { throw 'nope' }});
      }, 'nope')
  })
})
