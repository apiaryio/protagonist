const assert = require('chai').assert;
const path = require('path');
const fs = require('fs');

const valid_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'valid.apib'), 'utf8');
const warning_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'warning.apib'), 'utf8');
const error_fixture = fs.readFileSync(path.join(__dirname, '..', 'fixtures', 'error.apib'), 'utf8');

const valid_refract = require('../fixtures/valid.parse.json');
const valid_sourcemap_refract = require('../fixtures/valid.parse.sourcemap.json');
const warning_parse_refract = require('../fixtures/warning.parse.json');
const warning_validate_refract = require('../fixtures/warning.validate.json');
const error_refract = require('../fixtures/error.json');

module.exports = (parser) => {
  describe('Parse with async/await', () => {
    it('valid fixture without options should return parse result', async () => {
      const refract = await parser.parse(valid_fixture);
      assert.deepEqual(refract, valid_refract);
    });

    it('valid fixture with options should return parse result', async () => {
      const refract = await parser.parse(valid_fixture, { generateSourceMap: true });
      assert.deepEqual(refract, valid_sourcemap_refract);
    });

    it('error fixture without options should return parse result', async () => {
      const refract = await parser.parse(error_fixture);
      assert.deepEqual(refract, error_refract);
    });

    it('warning fixture without options should return parse result', async () => {
      const refract = await parser.parse(warning_fixture);
      assert.deepEqual(refract, warning_parse_refract);
    });
  });

  describe('Validate with async/await', () => {
    it('valid fixture without options should return null', async () => {
      const refract = await parser.validate(valid_fixture);
      assert.isNull(refract);
    });

    it('valid fixture with options should return null', async () => {
      const refract = await parser.validate(valid_fixture, {});
      assert.isNull(refract);
    });

    it('error fixture without options should return annotations', async () => {
      const refract = await parser.validate(error_fixture);
      assert.deepEqual(refract, error_refract);
    });

    it('warning fixture without options should return annotations', async () => {
      const refract = await parser.validate(warning_fixture);
      assert.deepEqual(refract, warning_validate_refract);
    });
  });
};
