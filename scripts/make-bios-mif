#!/usr/bin/env python3

# Copyright Jamie Iles, 2017
#
# This file is part of s80x86.
#
# s80x86 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# s80x86 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with s80x86.  If not, see <http://www.gnu.org/licenses/>.

import argparse
import os
import pystache
import struct

pystache.defaults.DELIMITERS = (u'<%', u'%>')

HERE = os.path.dirname(__file__)
MIF_TEMPLATE = os.path.join(HERE, 'bios.mif.templ')

def write_mif(binary, output):
    with open(binary, 'rb') as b:
        data = bytearray(b.read())
    vals = []
    for i in range(len(data) // 2):
        vals.append(struct.unpack('<H', data[i*2:i*2 + 2])[0])

    data = {
        'depth': len(data) // 2,
        'width': '16',
        'values': [
            {'address': '{0:04x}'.format(a),
             'value': '{0:04x}'.format(v)} for a, v in enumerate(vals)
        ]
    }
    with open(MIF_TEMPLATE) as f:
        template = f.read()
    with open(output, 'w') as output:
        output.write(pystache.render(template, data))

parser = argparse.ArgumentParser()
parser.add_argument('binary')
parser.add_argument('output')
args = parser.parse_args()

write_mif(args.binary, args.output)
