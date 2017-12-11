# Tripal\_analysis\_expression

This is an extension module for the Tripal project. 

Please note this module requires **Tripal 3** or greater.  The [Tripal 2 functional module is available for download](https://github.com/tripal/tripal_analysis_expression/releases/tag/1.0.2) but is no longer supported.

New documentation for the new data loaders and module structure is under development.

# Tripal Analysis: Expression

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Module Features](#module-features)
4. [Loading Biomaterials](#loading-biomaterials)
5. [Loading Expression Data](#loading-expression-data)
6. [Viewing Data](#viewing-data)
7. [Searching features and displaying expression data in a heatmap](#Searching-features-and-displaying-expression-data-in-a-heatmap)
8. [Administrative Pages](#administrative-pages)
9. [Example Files](#example-files)

# Introduction 
Tripal Analysis: Expression is a [Drupal](https://www.drupal.org/) module built to extend the functionality of the [Tripal](http://tripal.info/) toolset.
The purpose of the module is to visually represent gene expression for Tripal features. This module requires the following Tripal modules:

1. Tripal
2. Tripal Chado
3. Tripal Protocol (Included)
4. Tripal Biomaterial (Included)


# Installation
1. Click on the green "Clone or download" button on the top right corner of this page to obtain the web URL. Download this module by running ```git clone <URL> ``` on command line. 
2. Place the cloned module folder "tripal_analysis_expression" inside your /sites/all/modules. Then enable the module by running ```drush en tripal_analysis_expression``` (for more instructions, read the [Drupal documentation page](https://www.drupal.org/node/120641)).

# Features

* Provides data loaders for biosamples and expression data
* Controlled Vocabulary tools for biosamples
* Visualization for expression data for individual features
* Heatmap tool to visualize multiple features

### Data Loaders
Two loaders are provided by this module, a biosample loader, and an expression loader. The biosample loader has the ability to load data from a flat file or from an xml file downloaded from NCBI. The expression loader can load expression data in column or matrix format.

### Expression Display
Once expression data is loaded, a display field will be shown on each feature page that has corresponding biosamples and expression values.

### Heatmap
This module provides a search and results block to search for and select features to display in a heatmap.

# Loading Biosamples
Biosamples may be loaded from a flat file or from a BioSample xml file downloaded from NCBI. The steps for loading biosamples are as follows (detailed instructions can be found further below):

1. [First download or generate the flat (.csv, .tsv) or .xml file with biosample data you want to load](#downloading-xml-biosample-file-from-ncbi).
2. Add the organism associated with the biosample if it doesn't exist yet (**Add Tripal content->Organism**).  You may also create an analysis to associate the biosamples with if you choose. 
3. Navigate to the Tripal site's Tripal Biomaterial Loader 
4. Publish the biosamples

### Downloading XML BioSample File From NCBI
To obtain a xml BioSample file from ncbi go the [NCBI BioSample database](http://www.ncbi.nlm.nih.gov/biosample/). Search for and select the BioSamples you would like to download. 
![Select BioSamples](https://cloud.githubusercontent.com/assets/14822959/12490120/f5223ad8-c041-11e5-93ac-4692e27bf3d1.png)

Click the "Send to:" link. Then select "File" and select "Full XML (text)" as the format. Then click "Create File". 
![Download BioSample XML File](https://cloud.githubusercontent.com/assets/14822959/12490242/8cb8b796-c042-11e5-82dc-7a723867ea7a.png)

Click [here to see an example XML BioSample file from NCBI](example_files/sm125.xml).

### Using the Biosample loader
To upload the file into Chado/Tripal, navigate to:  

**Tripal->loaders->chado_biosample_loader**

First, provide the path on the server to the biosample file, or use the file uploader. You must select an Organism to associate the biosamples with.  You may also associate the imported biosamples with an analysis, but this is not required. 

Press the **Check Biosamples** button to preview your biosample properties.  To take advantage of a controlled vocabulary (CV), you must manually assign each property to a CVterm.  The uploader will list all CV terms matching each property, and provide the CV, database (DB) and accession for the match. 
If a match does not exist for your term, use the CVterm browser to identify an appropriate CVterm in your Tripal site, and rename the property in your input file to match the term.  If no term exists in your database, you should use the EBI ontology lookup serice to identify an appropriate term and insert it manually, or, load the corresponding CV.  

After clicking "Submit job", the page should reload with the job status and Drush command to run the job. Copy and paste the Drush command and run it on command line. Upon running the Drush command, any warning/error/success/status message should be displayed.

### Loading Biomaterials From a Flat File

Altenatively biomaterials may be loaded from a flat file (CSV or TSV). The flat file loader is designed to upload files that are in the [NCBI BioSample submission format](https://submit.ncbi.nlm.nih.gov/biosample/template/) which can be downloaded [here](https://submit.ncbi.nlm.nih.gov/biosample/template/). Download the TSV version of the file. The file must have a header that specifies the type of data in the column. There must be one column labeled "sample\_name". The loader will begin to collect data from the line that follows the line containing "sample\_name" which is assumed to be the header line. Columns are not required to be in any order. Other columns will be either attributes or accessions. Available NCBI [attributes](https://submit.ncbi.nlm.nih.gov/biosample/template/) can be found [here](https://submit.ncbi.nlm.nih.gov/biosample/template/). Available accession headers are bioproject\_accession, sra\_accession, biosample\_accession. All other columns will be uploaded as properties. To upload other accessions use the bulk loader provided with this module labeled, "Biomaterial Accession Term Loader". This loader will load a flat file with 3 columns (sample name, database name, accession term). A Tripal database must be created with the same name as the database name in the upload file.

Click here to see an example of a [CSV file](example_files/exampleCSV.csv) and a [TSV file](example_files/exampleTSV.tsv).

![Flat File Loader](https://cloud.githubusercontent.com/assets/14822959/12991558/a4b26a30-d0dd-11e5-8419-07216d0cbbc8.png)

### Publishing Biosamples to the Biological Sample Content Type

After loading, biosamples must be published to create entities for each biosample content type. As an administrator or user with correct permissions, navigate to **Content->Tripal Content->Publish Tripal Content**. Select the biological sample type to publish, apply any optional filtering, and press Publish.

### Loading a Single Biosample
Biosamples may also be loaded one at a time. As an administer or a user with permission to create Tripal content, go to: **Content->Tripal Content -> Add Tripal Content -> Biological Sample**. Available biosamples fields include the following. 
* **Accession** - If the biosample is in a database stored in your Tripal site, the accession can be entered here.  
* **Name (must be unique - required)**
* **Description** - A description of the biosample.
* **Contact** - The person or organization responsible for collecting the biosample.
* **Organism** - The organism from which the biosample was collected. 
* **Properties** - The properties describing this biosample, such as "age" or "geographic location".  Each property type utilizes a CVterm.
 
 ## Biosample properties
  
  Properties inserted into the database using the biosample bulk loader will be made available as new fields.  They can be found by going to admin->structure->Tripal Content Types -> Biological Sample and pressing the + Check for New Fields button in the upper left hand of the screen.
  
  If you would like to create new properties, you may do so in the structure menu.  Using the **Add New Field** row, enter the label and select **Chado Property** for the field type.  After pressing Save, you **must assign a CVterm** to this property in the Controlled Vocabulary Term section.  If an appropriate CVterm does not exist, you must insert it before you can create the field. To do so, navigate to `tripal/loaders/chado_cvterms` and press the *Add Term** button.
  

# Loading Expression Data
The steps for loading expression data are as follows (detailed instructions can be found further below):

1. Obtain expression data. Click [here to read about the file formats accepted for expression data](#data-loader). 
2. Add the organism associated with the expression data if it hasn't been added. 
3. Upload all features in the expression data to the Chado database. To bulk upload features, go to **Tripal->Data Loaders->Chado FASTA Loader** and upload a FASTA file (click here to see an example of [fasta file of transcriptome sequences](http://www.hardwoodgenomics.org/sites/default/files/sequences/sugarMaple022416/Acer_saccharum_022416_transcripts.fasta)). Or upload one feature at a time via **content-> Tripal Content -> Add content**, and select the relevant entity type (such as mRNA).
4. Load the espression data.  This is also the step where you can add experimental design details.

### Creating the Analysis
Before loading data, describe the experimental setup used to collect the data. As an administrator or a user with permission to create content

**Note that program name, program version, and source name must be unique as a whole for analysis to be inserted correctly** (click [here](http://gmod.org/wiki/Chado_Companalysis_Module) to read more about the data structure for analysis).

#### Analysis Fields:

* **Analysis Name (required)**
* **Program, Pipeline Name or Method Name (required, part of unique constraint)**
* **Program, Pipeline or Method version (required, part of unique constraint)**
* **Algorithm**
* **Source Name (required, part of unique constraint)**
* **Source Version**
* **Source URI**
* **Time Executed (required)**
* **Materials & Methods (Description and/or Program Settings)** 


#### Data Loader

The data loader fields provide a way for the user to load expression data associated with the experiment. The loader can load data from two types of formats, matrix and column. The matrix format expects a row of data containing biomaterials names. The first column should be unique feature names. Features must already be loaded into the database. Biomaterials will be added if not present. Expression values will map to a library in the column and a feature in the row. Only one matrix file may be loaded at a time. The column format expects the first column to contain features and the second column to be expression values. 

For an example column file, click [here](example_files/exampleExpressionData.rpkm). For an example matrix file, click [here](example_files/exampleMatrix.tsv).

The biomaterial name will be taken as the name of the file minus the file extension. Features must already be loaded into the database. Biomaterials will be added if not present. Multiple column format files may be loaded at the same time given that the files are in the same directory and contain the same file suffix. Either format may have header or footer information. Regex can be used in the form to only record data after the header and before the footer. Any file suffix can be used. The data loader fields are the following:
* **Source File Type** - This can be either "Column Format" or "Matrix Format".
* **Checkbox** - Check this box to submit a job to parse the data into Chado.
* **File Type Suffix** - The suffix of the files to load. This is used to submit multiple column format files in the same directory. A suffix is not required for a matrix file.
* **File Path** - The  path to a single matrix or column format file. The path may also be set to a directory, in which case all column files with the "File Type Suffix" specified above will be loaded. When loading multiple files from a file suffix must be specified. 
* **Regex for Start of Data** - If the expression file has a header, use this field to capture the line that occurs before the start of expression data. This line of text and any text preceding this line will be ignored. 
* **Regex for End of Data** - If the expression file has a footer, use this field to capture teh line that occurs after the end of expression data. This line of text and all text following will be ignored.


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
* **Quantification Details** - A description of the method used to transform raw expression data into numeric data. 
 * **Date Quantification Run** - The date the quantification was run. 
 * **Quantification URI** - A web address to a page that describes the quantification process.
 * **Quantification Operator** - The person or organization that ran the quantification.
 * **Quantification Protocol** - The quantification protocol used in the experiment. (See protocol description below).

![Experimental Design Fields](https://cloud.githubusercontent.com/assets/14822959/12991557/a4b0228e-d0dd-11e5-93de-2f206be6d5fe.png)
**Protocol Descripton** - The protocol content types can be created by navigating to **Add content->Protocol**. A protocol can be used to add extra detail to an experimental design. A protocol can be used to describe the assay, acquisition, and quantification steps of the experiment design. A protocol can also be used to further describe the array design content type. The fields of a protocol are:
* **Protocol Name (must be unique - required)**
* **Protocol Link** - A web address to a page that describes the protocol.
* **Protocol Description** - A description of the protocol.
* **Hardware Description** - A description of the wardware used in the protocol.
* **Software Description** - A description of the software used in the protocol.
* **Protocol Type (required)** - The protocol type can acquisition, array design, assay, or quantification. The user can also create new protocol types.
* **Publication** - A publication that describes the protocol.


![Data Loader Fields](https://cloud.githubusercontent.com/assets/14822959/12991553/a4ade58c-d0dd-11e5-97d2-1096d78bb189.png)
# Viewing Data
The following panes are added to the following content types:





### Feature

**New expresion feature viewer screenshot**

### Organism
* **Biomaterial Browser** - After loading biomaterials, a new pane with a list of biomaterials will appear on the corresponding organism page. Biomaterials are not required to be synced when to appear in this list.

### Analysis: Expression
* **Overview (base)** - The generic tripal overview pane.
* **Protocol** - Protocols used in this analysis (acquisition protocol, assay protocol, and quantification protocol).

### Biomaterial
* **Overview (base)** - The generic tripal overview pane.
* **Properties** - Properties associated with the biomaterial.
* **Cross References** - Accession terms associated with the biomaterial.



# Searching features and displaying expression data in a heatmap
This module creates two blocks: one for features input and the other displaying a heatmap for the input features.

### Turn On blocks
Go to **Structure->blocks** and find these two blocks: ***tripal_analysis_expression features form for heatmap*** and ***tripal_elasticsearch block for search form: blast_merged_transcripts***. Config these two blocks to let them display at specific region and page(s). The ***tripal_analysis_expression features form for heatmap*** will display a form that allow you to input some feature IDs.

![feature-input-form](https://cloud.githubusercontent.com/assets/1262709/25756810/22149cf4-3196-11e7-8f22-089f316297a0.png)

After you enter some feature IDs, you click the "Display Expression Heatmap" button to generate a heatmap for the features. 

**New heatmap screenshot**


# Administrative Pages

### Content Type Administrative Pages
Each Analysis: Expression content type has administrative pages. As an administrator or a user with correct permissions navigate to the following: **Tripal->Extensions->Expression Analysis->Tripal Expression Analysis Content Types**. Each content type has the following administrative pages. 
* **Administrative Search** - Administrative search to find, create, edit, or delete content type. 
* **Sync** - Page to sync content type from the chado database. Also provides a method to clean up orphaned nodes.
* **Delete** - Page where content type can be deleted in bulk.
* **TOC** - Page to change the default order and display of table of contents and panes for content type pages. 
* **Settings** - Page to set default page titles and default page urls for content type.
* **Help** - Description the content type.

![Administrator Pages for Content Types](https://cloud.githubusercontent.com/assets/14822959/13010514/2d2dc7be-d170-11e5-8670-92bdded6659d.png)

### Expression Display Administrative Page
The display of expression data on feature pages can be configured. To configure the expression figure, navigate to **Tripal->Extensions->Expression Analysis->Tripal Expression Analysis Settings**. Available options are:

* **Hide Expression Figure** - Hide expression figures on all feature pages. With this option you can load expression data without displaying the expression figure.
* **Hide Biomaterial Labels** - Hide the name of the biomaterial under the expression figure tile or column. Biomaterial names will still appear in tooltips.
* **Maximum Label Length** - Set the maximum acceptable biomaterial name length. Biomaterial names that are longer than this length will be truncated.
* **Expession Column Width** - Change the size of the width of the tile or column in the figure. Value must be 15 or greater. 
* **Default Heatmap Display** - The default display can be either a one dimensional heatmap or a bar chart. 

# Example Files

### Biomaterial Loader
1. Flat files: [CSV file](example_files/exampleCSV.csv), [TSV file](example_files/exampleTSV.tsv)
2. [XML file](example_files/sm125.xml)

### Expression Data Loader
1. [Column file](example_files/exampleExpressionData.rpkm)
2. [Matrix file](example_files/exampleMatrix.tsv)
