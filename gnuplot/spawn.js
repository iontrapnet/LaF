var ps = require('child_process').spawn(process.argv[2],process.argv.slice(3,process.argv.length));
process.stdin.pipe(ps.stdin);
ps.stdout.pipe(process.stdout);
ps.stderr.pipe(process.stderr);
ps.on('close',process.exit);