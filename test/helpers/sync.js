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
  describe('Parse sync', () => {
    it('valid fixture without options should return parse result', () => {
      const refract = parser.parseSync(valid_fixture);
      assert.deepEqual(refract, valid_refract);
    });

    it('valid fixture with options should return parse result', () => {
      const refract = parser.parseSync(valid_fixture, { generateSourceMap: true });
      assert.deepEqual(refract, valid_sourcemap_refract);
    });

    it('error fixture without options should return parse result', () => {
      const refract = parser.parseSync(error_fixture);
      assert.deepEqual(refract, error_refract);
    });

    it('warning fixture without options should return parse result', () => {
      const refract = parser.parseSync(warning_fixture);
      assert.deepEqual(refract, warning_parse_refract);
    });
  });

  describe('Validate sync', () => {
    it('valid fixture without options should return null', () => {
      const refract = parser.validateSync(valid_fixture);
      assert.isNull(refract);
    });

    it('valid fixture with options should return null', () => {
      const refract = parser.validateSync(valid_fixture, {});
      assert.isNull(refract);
    });

    it('error fixture without options should return annotations', () => {
      const refract = parser.validateSync(error_fixture);
      assert.deepEqual(refract, error_refract);
    });

    it('warning fixture without options should return annotations', () => {
      const refract = parser.validateSync(warning_fixture);
      assert.deepEqual(refract, warning_validate_refract);
    });
  });
};
