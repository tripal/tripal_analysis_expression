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

# Module Features

### Content Types
The is module provides four content types - Analysis: Expression, Biomaterial, Array Design, and Protocol. 

1. Analysis: Expression - The analysis: expression content type it built by hooking into the Tripal 2 Analysis module. This content type was modeled after the content types provided by the [Tripal InterPro Analysis](https://www.drupal.org/project/tripal_analysis_interpro) module and the [Tripal Blast Analysis](https://www.drupal.org/project/tripal_analysis_blast) module. This content type provides an interface to describe the experiment from which differential expression data was gathered. This content type provides the ability to describe either microarray differential expression data or next generation sequencing expression data (such as data obtained from RNASeq). This content type also provides a form to load expression data associated with the analysis.

2. Biomaterial - The biomaterial content type represents the Chado biomaterial table. The Chado biomaterial table is a member of the Chado MAGE module. The biomaterial content type is similar to the [BioSample](http://www.ncbi.nlm.nih.gov/books/NBK169436/) content type provided by [NCBI](http://www.ncbi.nlm.nih.gov/). See the biomaterial description at the [GMOD wiki](http://gmod.org/wiki/Chado_Mage_Module#Table:_biomaterial).

3. Array Design - The array design content type represents the Chado arraydesign table. This table is only used when describing the experimental design of data collected from a microarray expression experiment. See the arraydesign description at the [GMOD wiki](http://gmod.org/wiki/Chado_Mage_Module#Table:_arraydesign).

5. Protocol - The protocol content type represents the Chado protocol table. This table is used to describe the protocol, software, and hardware used in different steps of the experiment. See the protocol description at the [GMOD wiki](http://gmod.org/wiki/Chado_Tables#Table:_protocol).

### Administrative Pages
For each of the above content types, this module provides full administrative capabilities which includes the following, administrative list of content, sync, delete, TOC (table of contents), settings, and help pages. These pages were modeled after and created using other Tripal modules. 

### User Searches
A simple anonymous user search (using Views) is also provided for each content type. These searches can be foud at the following urls:

site_name/chado/analysis-expression

site_name/chado/arraydesign

site_name/chado/biomaterial

site_name/chado/protocol

### Data Loaders
Two loaders are provided by this module, a biomaterial loader, and a differential expression loader. The biomaterial loader has the ability to load data from a flat file or from an xml file downloaded from NCBI. The expression loader is included in the analysis: expression content type form.

### Differential Expression Display
Once expression data is loaded. A display will be shown on each feature page that has corresponding biomaterials and expression values.

# Loading Biomaterials
Biomaterials may be loaded from a flat file or from an BioSample xml file downloaded from NCBI. 

### NCBI XML BioSample File
To obtain a xml BioSample file from ncbi go the [NCBI BioSample database](http://www.ncbi.nlm.nih.gov/biosample/). Search for and select the BioSamples you would like to download. 
![Select BioSamples](https://cloud.githubusercontent.com/assets/14822959/12490120/f5223ad8-c041-11e5-93ac-4692e27bf3d1.png)

Click the "Send to:" link. Then select "File" and select "Full XML (text)" as the format. Then click "Create File". 
![Download BioSample XML File](https://cloud.githubusercontent.com/assets/14822959/12490242/8cb8b796-c042-11e5-82dc-7a723867ea7a.png)

To upload the file into Chado/Tripal, Navigate to:  
**Tripal->Extensions->Expression Analysis->Tripal Biomaterial Loader**

Select the organism for which you are uploading expression data. Select the "NCBI biosample xml file" and then write the path in "File Path" field.
