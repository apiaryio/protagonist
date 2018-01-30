const fs = require('fs');
const expect = require('chai').expect;
const glob = require('glob');

const protagonist = require('../build/Release/protagonist');

describe('Parsing Drafter Test Fixtures', () => {
  const fixtures = glob.sync('drafter/test/fixtures/*/*.apib');

  fixtures.forEach((fixture) => {
    it(`parsing ${fixture}`, () => {
      const source = fs.readFileSync(fixture, 'utf-8');
      const parseResult = protagonist.parseSync(source);
    });
  });
});
