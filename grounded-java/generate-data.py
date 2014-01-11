#!/usr/bin/env python

"""
Grounded, by Alejandro Santos <ale@ralo.com.ar>

Code generator for the Data.java file.
"""

import os

def as_byte(x):
    if x < 128:
        return x
    return -((x ^ 0xFF) + 1)

def main():
    with open("../common/img/tiles.png", 'rb') as f:
        with open("src/org/alejolp/grounded/Data.java", "w") as outf:
            data = f.read()

            outf.write("""
package org.alejolp.grounded;


/*
 * Grounded project, by Alejandro Santos
 * 
 *   https://github.com/alejolp/grounded
 * 
 * Automatically generated file. Do not edit!
 * 
 */

public class Data {

    public static final byte[] TILES_DATA = new byte[]{""")
            # Graphics
            outf.write(', '.join(map(lambda x: str(as_byte(ord(x))), data)))
            outf.write("""};
""")
            outf.write("""
    public static final String[] MAP_DATA = new String[]{
""")
            # Map data
            files = [int(os.path.splitext(i)[0]) \
                for i in os.listdir('../common/maps/')]
            maxmapnum = max(files)
            for i in range(0, maxmapnum+1):
                FNAME = "../common/maps/" + str(i) + ".txt"
                if os.path.exists(FNAME):
                    with open(FNAME) as mf:
                        outf.write('        "' + repr(mf.read().rstrip('\n'))[1:-1] + '",\n')
                else:
                    outf.write('        "",\n')
            outf.write("""
    };
""")
            outf.write("""
}""")

if __name__ == '__main__':
    main()

