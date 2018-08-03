const fs = require('fs');
const expect = require('chai').expect;
const glob = require('glob');

const protagonist = require('./protagonist');

describe('Parsing Drafter Test Fixtures', () => {
  const fixtures = glob.sync('drafter/test/fixtures/*/*.apib');

  fixtures.forEach((fixture) => {
    if (fixture.match(/\/mson\//) !== null) {
      return;
    }

    // TODO: Remove these exceptions after they are fixed in drafter
    if (fixture === 'drafter/test/fixtures/render/nullable.apib' || fixture === 'drafter/test/fixtures/schema/enum-nullable.apib') {
      return;
    }

    it(`parsing ${fixture}`, () => {
      // Find the parse result fixture(s).
      // Some test fixtures are with source maps and others are not.
      const results = glob.sync(fixture.replace('.apib', '{.sourcemap,}.json'));
      const sourceMapResult = results.find((results) => results.includes('.sourcemap.json'))
      const result = results.find((results) => !results.includes('.sourcemap.json'))

      const source = fs.readFileSync(fixture, 'utf-8');

      if (result) {
        const parseResult = protagonist.parseSync(source);
        const expected = JSON.parse(fs.readFileSync(result, 'utf-8'));
        expect(parseResult).to.deep.equal(expected);
      }

      if (sourceMapResult) {
        const sourceMapParseResult = protagonist.parseSync(source, {generateSourceMap: true});
        const sourceMapExpected = JSON.parse(fs.readFileSync(sourceMapResult, 'utf-8'));
        expect(sourceMapParseResult).to.deep.equal(sourceMapExpected);
      }

      if (!result && !sourceMapResult) {
        throw Error('No parse result fixture found');
      }
    });
  });
});
