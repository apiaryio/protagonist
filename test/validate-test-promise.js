
const fs = require('fs');
const path = require('path');
const assert = require('chai').assert;
const protagonist = require('./protagonist');

const valid_fixture = path.join(__dirname, 'fixtures', 'sample-api.apib');
const warning_fixture = path.join(__dirname, 'fixtures', 'invalid-api-warning.apib');
const error_fixture = path.join(__dirname, 'fixtures', 'invalid-api-error.apib');

const warning_refract = require('./fixtures/invalid-api-warning.json');
const error_refract = require('./fixtures/invalid-api-error.json');

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
