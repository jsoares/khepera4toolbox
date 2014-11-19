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
