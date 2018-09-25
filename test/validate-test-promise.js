const fs = require('fs');
const path = require('path');
const assert = require('chai').assert;
const protagonist = require('./protagonist');

const valid_fixture = path.join(__dirname, 'fixtures', 'valid.apib');
const warning_fixture = path.join(__dirname, 'fixtures', 'warning.apib');
const error_fixture = path.join(__dirname, 'fixtures', 'error.apib');

const warning_refract = require('./fixtures/warning.validate.json');
const error_refract = require('./fixtures/error.json');

describe("Validate Blueprint async with promise", () => {

  it("Validate with error", (done) => {
    fs.readFile(error_fixture, 'utf8', function(err, data) {
      if (err) {
        return done(err);
      }

      protagonist.validate(data)
        .then(res => {
          assert.deepEqual(res, error_refract);
          done()
        })
        .catch(err => {
          done(err)
        })
    });
  });

  it("Validate with warning", (done) => {
    fs.readFile(warning_fixture, 'utf8', function(err, data) {
      if (err) {
        return done(err);
      }

      protagonist.validate(data)
        .then(res => {
          assert.deepEqual(res, warning_refract);
          done()
        })
        .catch(err => {
          done(err)
        })
    });
  });

  it("Validate valid", (done) => {
    fs.readFile(valid_fixture, 'utf8', function(err, data) {
      if (err) {
        return done(err);
      }

      protagonist.validate(data)
        .then(res => {
          assert.isNull(res);
          done()
        })
        .catch(err => {
          done(err)
        })
    });
  });

})
