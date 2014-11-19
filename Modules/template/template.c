/******************************************************************************
 *
 * Khepera IV Toolbox
 * (c) 2014 EPFL, Lausanne, Switzerland
 * Authors: Jorge Soares et al.
 *
 * Code forked from Khepera III Toolbox
 * (c) 2006 EPFL, Lausanne, Switzerland
 * Authors: Thomas Lochmatter et al.
 *
 * Additional DISAL libraries
 * (c) 2006 EPFL, Lausanne, Switzerland
 * Authors: Thomas Lochmatter, Jim Pugh, Sven Gowal, Pascal Gilbert, and others
 *
 ******************************************************************************/


#include <stdio.h>
#include "template.h"

void template_init() {
}

void template_function1(struct sTemplate *template) {
    template->variable1 = 3;
    template->variable2 = 10.5;
}

float template_function2(struct sTemplate *template) {
    float sum;

    sum = (float) template->variable1 + template->variable2;
    return sum;
}
