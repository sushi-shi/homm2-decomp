// G01: tested complete two-arm structural matrix in build/g01-{axes,manifest}.json.
// build/g01-batch/results.json:2/2 complete, no TU-state probes. Original goto:
//1240bytes,83blocks; break candidate:1234bytes,81blocks,99.212600% fuzzy.
// Retain goto: the lost tail stub is actually present in retail, not a
// register-allocation residual. No internal respelling/state search is warranted.
// Independent check-function.py BASE/BUTTON 0xd3890 proves original1240bytes,
// complete DIR32 sites, and all22 ordered targets against genuine retail.
// The exact source-span alternatives are below (not a standalone TU).

// Retained:
            goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);

// Rejected:
            break;
    }


    return widget::Main(msg);
