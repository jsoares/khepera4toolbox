/*!
 * (c) 2006 - 2008 EPFL, Lausanne, Switzerland
 * Thomas Lochmatter
 */

#ifndef TEMPLATE
#define TEMPLATE

struct sTemplate {
    int variable1;
    float variable2;
};

//! Initializes this module.
void template_init();

//! A sample function.
void template_function1(struct sTemplate * template);
//! A sample function.
float template_function2(struct sTemplate * template);

#endif
