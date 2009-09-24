/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// Frequency modulator/demodulator
//

#include <stdlib.h>
#include <stdio.h>

#include "liquid.internal.h"

struct freqmodem_s {
    nco oscillator;
    pll sync;
    float m;        // modulation index

    // phase difference
    float complex q;
};

freqmodem freqmodem_create()
{
    freqmodem fm = (freqmodem) malloc(sizeof(struct freqmodem_s));

    fm->oscillator = nco_create(LIQUID_VCO);
    fm->sync = pll_create();
    fm->m = 0.1f;

    return fm;
}

void freqmodem_destroy(freqmodem _fm)
{
    nco_destroy(_fm->oscillator);
    pll_destroy(_fm->sync);
    free(_fm);
}

void freqmodem_print(freqmodem _fm)
{
    printf("freqmodem:\n");
    printf("    mod. index  :   %8.4f\n", _fm->m);
}

void freqmodem_reset(freqmodem _fm)
{
    // reset oscillator, phase-locked loop
    nco_reset(_fm->oscillator);
    pll_reset(_fm->sync);

    // clear complex phase term
    _fm->q = 0.0f;
}

void freqmodem_modulate(freqmodem _fm,
                        float _x,
                        float complex *_y)
{
    nco_set_frequency(_fm->oscillator,
                      (_fm->m)*_x);

    nco_cexpf(_fm->oscillator, _y);
    nco_step(_fm->oscillator);
}

void freqmodem_demodulate(freqmodem _fm,
                          float complex _y,
                          float *_x)
{
    // compute phase difference and normalize by modulation index
    // TODO : push through loop filter
    *_x = cargf(conjf(_fm->q)*(_y)) / (_fm->m);

    _fm->q = _y;
}


