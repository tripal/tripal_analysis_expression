# tripal_analysis_expression

This is an extension module for the Tripal project. 

# Tripal Analysis: Expression

1. [Introduction](#introduction)
2. [Module Features](#module-features)
3. [Loading Biomaterials](#loading-biomaterials)
4. [Loading Expression Data](#loading-expression-data)
5. [Administrative Pages](#administrative-pages)
6. [Viewing Data](#viewing-data)

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

### Module Administrative Pages
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

### Downloading XML BioSample File From NCBI
To obtain a xml BioSample file from ncbi go the [NCBI BioSample database](http://www.ncbi.nlm.nih.gov/biosample/). Search for and select the BioSamples you would like to download. 
![Select BioSamples](https://cloud.githubusercontent.com/assets/14822959/12490120/f5223ad8-c041-11e5-93ac-4692e27bf3d1.png)

Click the "Send to:" link. Then select "File" and select "Full XML (text)" as the format. Then click "Create File". 
![Download BioSample XML File](https://cloud.githubusercontent.com/assets/14822959/12490242/8cb8b796-c042-11e5-82dc-7a723867ea7a.png)

### Loading NCBI XML BioSample File into Tripal
To upload the file into Chado/Tripal, Navigate to:  
**Tripal->Extensions->Expression Analysis->Tripal Biomaterial Loader**

Select the organism for which you are uploading expression data. Optionaly select an expression analysis associated with the biomaterials. Select "NCBI biosample xml file" and then write the path in "File Path" field.

### Loading Biomaterials From a Flat File
Altenatively biomaterials may be loaded from a flat file (CSV or TSV). The flat file loader is designed to upload files that are in the [NCBI BioSample submission format](https://submit.ncbi.nlm.nih.gov/biosample/template/) which can be downloaded [here](https://submit.ncbi.nlm.nih.gov/biosample/template/). Download the TSV version of the file. The file must have a header that specifies the type of data in the column. There must be one column labeled "sample\_name". The loader will begin to collect data from the line that follows the line containing "sample\_name" which is assumed to be the header line. Columns are not required to be in any order. Other columns will be either attributes or accessions. Available NCBI [attributes](https://submit.ncbi.nlm.nih.gov/biosample/template/) can be found [here](https://submit.ncbi.nlm.nih.gov/biosample/template/). Available accession headers are bioproject\_accession, sra\_accession, biosample\_accession. All other columns will be uploaded as properties. To upload other accessions use the bulk loader provided with this module labeled, "Biomaterial Accession Term Loader". This loader will load a flat file with 3 columns (sample name, database name, accession term). A Tripal database must be created with the same name as the database name in the upload file.   

### Loading a Single Biomaterial
Biomaterials may also be loaded one at a time. As an administer or a user with permission to create content, go to: **Add content->Biomaterial**. Available biomaterial fields include the following. 
* **Biomaterial Name (must be unique - required)**
* **Biomaterial description** - A description of the biomaterial.
* **Biomaterial Provider** - The person or organization responsible for collecting the biomaterial
* **Organism** - The organism from which the biomaterial was collected. 
* **Analysis** - The expression analysis associated with the biomaterial. Note that a biomaterial can be created before an expression analysis.

There is also the ability to add properties or accession values to the biomaterial. 

# Loading Expression Data

### Creating the Experiment Setup
Before loading data, describe the experimental setup used to collect the data. As an administrator or a user with permission to create content, go to: **Add content->Analysis: Expression**. The "Analysis: Expression" content type is a sub-type of the analysis content type. It contains all fields used in the analysis content type as well as fields that allow the description of the experimental design and the data loader. 
#### Analysis Fields:
* **Analysis Name (required)**
* **Program, Pipeline Name or Method Name (required)**
* **Program, Pipeline or Method version (required)**
* **Algorithm**
* **Source Name (required)**
* **Source Version**
* **Source URI**
* **Time Executed (required)**
* **Materials & Methods (Description and/or Program Settings)** 

There is also the ability to add analysis properties to this content type.

#### Experimental Design Fields
The "Experimental Design" fields allow a complete description of the experimental design. The Chado MAGE module which is used by the Analysis: Expression module. The Chado MAGE module uses, the arraydesign, assay, quantification, and acquisition tables to describe an experiment. This is reflected in the following fields available to describe an experiment. 
* **Organism (required)**
* **Biomaterial Provider** - The person or organization responsible for collecting the biomaterial.
* **Array Design** - This is only applicable for microarray expression data. This may be left blank for next generation sequencing expression data. 
* **Assay Details** - A description of the physical instance of the array used in the experiment
 * **Date Assay Run** - The date the assay was run. 
 * **Assay Description** - A short description of the assay.
 * **Assay Operator**  - The person or organization that ran the assay.
 * **Assay Protocol** - The assay protocol used in the experiment. (See protocol description below).
* **Acquisition Details** - The scanning of the experiment.
 * **Data Acquisition Run** - The date the acquisition was run. 
 * **Acquisition URI** - A web address to a page that describes the acquisition process.
 * **Acquisition Protocol** - The acquisition protocol used in the experiment. (See protocol description below).
* **uantification Details** - A description of the method used to transform raw expression data into numeric data. 
 * **Date Quantification Run** - The date the quantification was run. 
 * **Quantification URI** - A web address to a page that describes the quantification process.
 * **Quantification Operator** - The person or organization that ran the quantification.
 * **Quantification Protocol** - The quantification protocol used in the experiment. (See protocol description below).

**Protocol Descripton** - The protocol content types can be created by navigating to **Add content->Protocol**. A protocol can be used to add extra detail to an experimental design. A protocol can be used to describe the assay, acquisition, and quantification steps of the experiment design. A protocol can also be used to further describe the array design content type. The fields of a protocol are:
* **Protocol Name (must be unique - required)**
* **Protocol Link** - A web address to a page that describes the protocol.
* **Protocol Description** - A description of the protocol.
* **Hardware Description** - A description of the wardware used in the protocol.
* **Software Description** - A description of the software used in the protocol.
* **Protocol Type (required)** - The protocol type can acquisition, array design, assay, or quantification. The user can also create new protocol types.
* **Publication** - A publication that describes the protocol.

#### Data Loader
* The data loader fields provide a way for the user to load expression data associated with the experiment. The loader can load data from two types of formats, matrix and column. The matrix format expects a row of data containing biomaterials names. The first column should be unique feature names. Features must already be loaded into the database. Biomaterials will be added if not present. Expression values will map to a library in the column and a feature in the row. Only one matrix file may be loaded at a time. The column format expects the first column to contain features and the second column to be expression values. The biomaterial name will be taken as the name of the file minus the file extension. Features must already be loaded into the database. Biomaterials will be added if not present. Multiple column format files may be loaded at the same time given that the files are in the same directory and contain the same file suffix. Either format may have header or footer information. Regex can be used in the form to only record data after the header and before the footer. Any file suffix can be used. The data loader fields are the following:
* **Source File Type** - This can be either "Column Format" or "Matrix Format".
* **Checkbox** - Check this box to submit a job to parse the data into Chado.
* **File Type Suffix** - The suffix of the files to load. This is used to submit multiple column format files in the same directory.
* **File Path** - The  path to a single matrix or column format file. The path may also be set to a directory, in which case all column files with the "File Type Suffix" specified above will be loaded. If the suffix is not specified then 
* **Regex for Start of Data**
* **Regex for End of Data**
