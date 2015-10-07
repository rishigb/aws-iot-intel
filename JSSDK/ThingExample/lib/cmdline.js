/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

//node.js deps
const fs        = require('fs');

//npm deps
const minimist  = require('minimist');

//app deps
const isUndefined = require('../../common/lib/is-undefined');

//begin module
const clientIdDefault = process.env.USER.concat(Math.floor((Math.random() * 100000) + 1));
module.exports = function( description, args, processFunction, argumentHelp ) {
    doHelp = function() {
        var progName=process.argv[1];
        var lastSlash=progName.lastIndexOf('/');
        if (lastSlash !=-1)
        {
           progName=progName.substring(lastSlash+1,progName.length);
        }
        if (isUndefined(argumentHelp)) {
            console.log('Usage: ' + progName + ' [OPTION...]');
        }
        else
        {
            console.log('Usage: ' + progName + ' [OPTION...] ARGUMENTS...');
        }
        console.log('\n' + progName + ': ' + description + '\n\n' +  
                    ' Options\n\n' +
                    '  -g, --aws-region=REGION          AWS IoT region\n' +
                    '  -i, --client-id=ID               use ID as client ID\n' +
                    '  -k, --private-key=FILE           use FILE as private key\n' +
                    '  -c, --client-certificate=FILE    use FILE as client certificate\n' +
                    '  -a, --ca-certificate=FILE        use FILE as client certificate\n' +
                    '  -f, --certificate-dir=DIR        look in DIR for certificates\n' +
                    '  -r, --reconnect-period-ms=VALUE  use VALUE as the reconnect period (ms)\n' +
                    '  -t, --test-mode=[1-n]            set test mode for multi-process tests\n' +
                    '  -d, --delay-ms=VALUE             delay in milliseconds before publishing\n\n' +
                    ' Default values\n\n' + 
                    '  aws-region                       us-east-1\n' +
                    '  client-id                        $USER<random-integer>\n' +
                    '  private-key                      ./privkey.pem\n' +
                    '  client-certificate               ./cert.pem\n' +
                    '  ca-certificate                   ./aws-iot-rootCA.crt\n' +
                    '  reconnect-period-ms              3000ms\n' +
                    '  delay-ms                         4000ms\n' +
                    '  test-mode                        1\n');
        if (!isUndefined(argumentHelp)) {
            console.log(argumentHelp);
        }
    };
    args = minimist(args, {
    string: ['certificate-dir', 'aws-region', 'private-key', 'client-certificate', 'ca-certificate', 'client-id' ],
    integer: [ 'reconnect-period-ms', 'test-mode', 'delay-ms' ],
    boolean: ['help'],
    alias: {
      region: ['g', 'aws-region'],
      clientId: ['i', 'client-id'],
      privateKey: ['k', 'private-key'],
      clientCert: ['c', 'client-certificate'],
      caCert: ['a', 'ca-certificate'],
      certDir: ['f', 'certificate-dir'],
      reconnectPeriod: ['r', 'reconnect-period-ms'],
      testMode: ['t', 'test-mode'],
      delay: ['d', 'delay-ms'],
      debug: 'D',
      help: 'h'
    },
    default: {
      region: 'us-east-1',
      clientId: clientIdDefault,
      privateKey: 'privkey.pem',
      clientCert: 'cert.pem',
      caCert: 'aws-iot-rootCA.crt',
      testMode: 1,
      reconnectPeriod: 3*1000,     /* milliseconds */
      delay: 4*1000,               /* milliseconds */
    }, 
    unknown: function() {console.error('***unrecognized options***'); doHelp(); process.exit(1); }
  });
  if (args.help) {
      doHelp();
      return;
  }
  if (!isUndefined( args.certDir ))
  {
     args.privateKey=args.certDir+'/'+args.privateKey;
     args.clientCert=args.certDir+'/'+args.clientCert;
     args.caCert=args.certDir+'/'+args.caCert;
  }
//
// Client certificate, private key, and CA certificate must all exist.
//
  if (!fs.existsSync( args.privateKey ))
  {
     console.error( '\n' + args.privateKey + ' doesn\'t exist (--help for usage)\n');
     return;
  }
  if (!fs.existsSync( args.clientCert ))
  {
     console.error( '\n' + args.clientCert + ' doesn\'t exist (--help for usage)\n');
     return;
  }
  if (!fs.existsSync( args.caCert ))
  {
     console.error( '\n' + args.caCert + ' doesn\'t exist (--help for usage)\n');
     return;
  }

  processFunction( args );
}
