const assert = require('chai').assert;
const path = require('path');
const fs = require('fs');
const protagonist = require('./protagonist');

const valid_fixture = fs.readFileSync(path.join(__dirname, 'fixtures', 'valid.apib'), 'utf8');

describe('Async Parse', () => {
  it('should finish after Sync Parse', (done) => {
    var syncFinished = false;
    var syncParsed = null;

    protagonist.parse(valid_fixture, (err, res) => {
      if (syncFinished) {
        assert.isNull(err);
        assert.deepEqual(res, syncParsed);
        done();
      } else {
        done(new Error('Async finished before sync'));
      }
    });

    syncParsed = protagonist.parseSync(valid_fixture);
    syncFinished = true;
  });
});

describe('Async Validate', () => {
  it('should finish after Sync Validate', (done) => {
    var syncFinished = false;
    var syncParsed = null;

    protagonist.validate(valid_fixture, (err, res) => {
      if (syncFinished) {
        assert.isNull(err);
        assert.deepEqual(res, syncParsed);
        done();
      } else {
        done(new Error('Async finished before sync'));
      }
    });

    syncParsed = protagonist.validateSync(valid_fixture);
    syncFinished = true;
  });
});
