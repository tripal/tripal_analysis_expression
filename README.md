# tripal_analysis_expression

This is an extension module for the Tripal project. 

# Tripal Analysis: Expression

1. [Introduction](#Introduction)
2. [Module Features](#Module Features)
3. [Loading Biomaterials](#Loading Biomaterials)
4. [Loading Expression Data](#Loading Expression Data)
5. [Administrative Pages](#Administrative Pages)
6. [Viewing Data](#Viewing Data)

# Introduction 
Tripal Analysis: Expression is a [Drupal](https://www.drupal.org/) module built to extend the functionality of the [Tripal](http://tripal.info/) toolset.
The purpose of the module is to visually represent differential gene expression for Tripal features. This module requires the following Tripal modules:

1. Tripal Core
2. Tripal Views
3. Tripal DB
4. Tripal CV
5. Tripal Analysis
6. Tripal Feature
7. Tripal Organism
8. Tripal Contact

The is module provides four content types - Analysis: Expression, Biomaterial, Array Design, and Protocol. 

1. Analysis: Expression - The analysis: expression content type it built by hooking into the Tripal 2 Analysis module. This content type was modeled after the content types provided by the [Tripal InterPro Analysis](https://www.drupal.org/project/tripal_analysis_interpro) module and the [Tripal Blast Analysis](https://www.drupal.org/project/tripal_analysis_blast) module. 
