/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

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
