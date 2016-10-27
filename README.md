# Tripal\_analysis\_expression

This is an extension module for the Tripal project. 

# Tripal Analysis: Expression

1. [Introduction](#introduction)
2. [Installation](#installation)
2. [Module Features](#module-features)
3. [Loading Biomaterials](#loading-biomaterials)
4. [Loading Expression Data](#loading-expression-data)
5. [Viewing Data](#viewing-data)
6. [Administrative Pages](#administrative-pages)

# Introduction 
Tripal Analysis: Expression is a [Drupal](https://www.drupal.org/) module built to extend the functionality of the [Tripal](http://tripal.info/) toolset.
The purpose of the module is to visually represent gene expression for Tripal features. This module requires the following Tripal modules:

1. Tripal Core
2. Tripal Views
3. Tripal DB
4. Tripal CV
5. Tripal Analysis
6. Tripal Feature
7. Tripal Organism
8. Tripal Contact

# Installation
1. Click on the green "Clone or download" button on the top right corner of this page to obtain the web URL. Download this module by running ```git clone <URL> ``` on command line. 
2. Place the module folder inside your /sites/all/modules/{modulename} or sites/all/themes/{themename} folder (for more instructions, read the [Drupal documentation page](https://www.drupal.org/node/120641).

# Module Features

### Content Types
The is module provides four content types - Analysis: Expression, Biomaterial, Array Design, and Protocol. 

1. Analysis: Expression - The analysis: expression content type it built by hooking into the Tripal 2 Analysis module. This content type was modeled after the content types provided by the [Tripal InterPro Analysis](https://www.drupal.org/project/tripal_analysis_interpro) module and the [Tripal Blast Analysis](https://www.drupal.org/project/tripal_analysis_blast) module. This content type provides an interface to describe the experiment from which expression data was gathered. This content type provides the ability to describe either microarray expression data or next generation sequencing expression data (such as data obtained from RNASeq). This content type also provides a form to load expression data associated with the analysis.

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
Two loaders are provided by this module, a biomaterial loader, and an expression loader. The biomaterial loader has the ability to load data from a flat file or from an xml file downloaded from NCBI. The expression loader is included in the analysis: expression content type form.

### Expression Display
Once expression data is loaded. A display will be shown on each feature page that has corresponding biomaterials and expression values.

# Loading Biomaterials
Biomaterials may be loaded from a flat file or from an BioSample xml file downloaded from NCBI. The steps for loading biomaterials are as follows (detailed instructions can be found further below):

1. [First download or generate the flat (.csv, .tsv) or .xml file with biomaterials data you want to load](#downloading-xml-biosample-file-from-ncbi).
2. Navigate to the Tripal site's Tripal Biomaterial Loader to submit the job with [a .xml file](#loading-ncbi-xml-biosample-file-into-tripal) or [a flat file](#loading-biomaterials-from-a-flat-file). Run the job via command line with Drush command.
3. [Sync the biomaterial(s) on the Tripal site. Run the sync job via command line with Drush command](#syncing-biomaterials).
4. Verify that the biomaterial(s) loaded correctly by viewing it via "Find content".

### Downloading XML BioSample File From NCBI
To obtain a xml BioSample file from ncbi go the [NCBI BioSample database](http://www.ncbi.nlm.nih.gov/biosample/). Search for and select the BioSamples you would like to download. 
![Select BioSamples](https://cloud.githubusercontent.com/assets/14822959/12490120/f5223ad8-c041-11e5-93ac-4692e27bf3d1.png)

Click the "Send to:" link. Then select "File" and select "Full XML (text)" as the format. Then click "Create File". 
![Download BioSample XML File](https://cloud.githubusercontent.com/assets/14822959/12490242/8cb8b796-c042-11e5-82dc-7a723867ea7a.png)

Click [here to see an example XML BioSample file from NCBI](example_files/sm125.xml).

### Loading NCBI XML BioSample File into Tripal
To upload the file into Chado/Tripal, Navigate to:  
**Tripal->Extensions->Expression Analysis->Tripal Biomaterial Loader**

Select the organism for which you are uploading expression data. Select "NCBI biosample xml file" and then write the path in "File Path" field.

![NCBI XML BioSample Loader](https://cloud.githubusercontent.com/assets/14822959/12991555/a4afaf70-d0dd-11e5-95b1-ebbc6da404dc.png)

After clicking "Submit job", the page should reload with the job status and Drush command to run the job. Copy and paste the Drush command and run it on command line. Upon running the Drush command, any warning/error/success/status message should be displayed.

Similarily, after clicking "Submit job", the page should reload with the job status and Drush command to run the job. Copy and paste the Drush command and run it on command line. Upon running the Drush command, any warning/error/success/status message should be displayed.

### Loading Biomaterials From a Flat File

Altenatively biomaterials may be loaded from a flat file (CSV or TSV). The flat file loader is designed to upload files that are in the [NCBI BioSample submission format](https://submit.ncbi.nlm.nih.gov/biosample/template/) which can be downloaded [here](https://submit.ncbi.nlm.nih.gov/biosample/template/). Download the TSV version of the file. The file must have a header that specifies the type of data in the column. There must be one column labeled "sample\_name". The loader will begin to collect data from the line that follows the line containing "sample\_name" which is assumed to be the header line. Columns are not required to be in any order. Other columns will be either attributes or accessions. Available NCBI [attributes](https://submit.ncbi.nlm.nih.gov/biosample/template/) can be found [here](https://submit.ncbi.nlm.nih.gov/biosample/template/). Available accession headers are bioproject\_accession, sra\_accession, biosample\_accession. All other columns will be uploaded as properties. To upload other accessions use the bulk loader provided with this module labeled, "Biomaterial Accession Term Loader". This loader will load a flat file with 3 columns (sample name, database name, accession term). A Tripal database must be created with the same name as the database name in the upload file.

Click here to see an example of a [CSV file](example_files/CSV_test.csv) and a [TSV file](example_files/test7.tsv).

![Flat File Loader](https://cloud.githubusercontent.com/assets/14822959/12991558/a4b26a30-d0dd-11e5-8419-07216d0cbbc8.png)

### Syncing Biomaterials

After loading, biomaterials must be synced to create nodes for each biomaterial content type. As an administrator or user with correct permissions, navigate to **Tripal->Extensions->Expression Analysis->Tripal Expression Analysis Content Types->Biomaterial->SYNC**. Select the biomaterials to sync and click "Sync Biomaterials".

![Syncing Biomaterials](https://cloud.githubusercontent.com/assets/14822959/12991663/243827d6-d0de-11e5-820c-4cae34974283.png)

Similarily, after clicking "Sync Biomaterials", run the Drush command on command line and monitor for any warnings/error messages.

### Loading a Single Biomaterial
Biomaterials may also be loaded one at a time. As an administer or a user with permission to create content, go to: **Add content->Biomaterial**. Available biomaterial fields include the following. 
* **Biomaterial Name (must be unique - required)**
* **Biomaterial description** - A description of the biomaterial.
* **Biomaterial Provider** - The person or organization responsible for collecting the biomaterial
* **Organism** - The organism from which the biomaterial was collected. 
* **Analysis** - The expression analysis associated with the biomaterial. Note that a biomaterial can be created before an expression analysis.

There is also the ability to add properties or accession values to the biomaterial. 

# Loading Expression Data
The steps for loading expression data are as follows (detailed instructions can be found further below):

1. Upload all associated features to Chado database. To bulk upload features, go to **Tripal->Chado Data Loaders->FASTA file Loader** and upload a fasta file (click here to see an example of [fasta file of transcriptome sequences](http://www.hardwoodgenomics.org/sites/default/files/sequences/sugarMaple022416/Acer_saccharum_022416_transcripts.fasta)). Or upload one feature at a time via **Add content->Feature**. Submit the uploading job(s) and run job(s) with Drush command.
2. [Create the experiment setup](#creating-the-experiment-setup). Provide file path for expression data file or directory and make sure "Submit a job to parse the expression data into Chado" is checked. Click [here to read about the file formats accepted](#data-loader). Save analysis and run the job with Drush command. 
3. [View the expression data](#viewing-data) by going to **Find content**, and clicking into the features just added.  

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

![Analysis Feilds](https://cloud.githubusercontent.com/assets/14822959/12991556/a4afb984-d0dd-11e5-8368-3018c2d80ede.png)
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

#### Data Loader

The data loader fields provide a way for the user to load expression data associated with the experiment. The loader can load data from two types of formats, matrix and column. The matrix format expects a row of data containing biomaterials names. The first column should be unique feature names. Features must already be loaded into the database. Biomaterials will be added if not present. Expression values will map to a library in the column and a feature in the row. Only one matrix file may be loaded at a time. The column format expects the first column to contain features and the second column to be expression values. 

For an example column file, click [here](example_files/SM-80.counts.txt.filtered.rpkm).

The biomaterial name will be taken as the name of the file minus the file extension. Features must already be loaded into the database. Biomaterials will be added if not present. Multiple column format files may be loaded at the same time given that the files are in the same directory and contain the same file suffix. Either format may have header or footer information. Regex can be used in the form to only record data after the header and before the footer. Any file suffix can be used. The data loader fields are the following:
* **Source File Type** - This can be either "Column Format" or "Matrix Format".
* **Checkbox** - Check this box to submit a job to parse the data into Chado.
* **File Type Suffix** - The suffix of the files to load. This is used to submit multiple column format files in the same directory. A suffix is not required for a matrix file.
* **File Path** - The  path to a single matrix or column format file. The path may also be set to a directory, in which case all column files with the "File Type Suffix" specified above will be loaded. When loading multiple files from a file suffix must be specified. 
* **Regex for Start of Data** - If the expression file has a header, use this field to capture the line that occurs before the start of expression data. This line of text and any text preceding this line will be ignored. 
* **Regex for End of Data** - If the expression file has a footer, use this field to capture teh line that occurs after the end of expression data. This line of text and all text following will be ignored.

![Data Loader Fields](https://cloud.githubusercontent.com/assets/14822959/12991553/a4ade58c-d0dd-11e5-97d2-1096d78bb189.png)
# Viewing Data
The following panes are added to the following content types:

### Feature
* **Expression** - After biomaterials and expression data have been loaded the expression pane will appear on the corresponding feature page. The pane will 5 different links: Sort Descending, Sort Ascending, Only Non-Zero Values, Tile/Chart, Reset.
 * **Sort Descending/Sort Ascending** - Sort expression data based on expression values - descending or ascending. 
 * **Only Non-Zero Values** - Remove biomaterials that do not expression the feature.
 * **Tile/Chart** - Toggle figure between a tile heatmap view or a chart view. 
 * **Reset** - Reset the figure. Return the figure to it's original state.

![Expression Tile Map](https://cloud.githubusercontent.com/assets/14822959/13010313/3da1a292-d16f-11e5-9c32-f8d6f43a0c34.png)

### Organism
* **Biomaterial Browser** - After loading biomaterials, a new pane with a list of biomaterials will appear on the corresponding organism page. Biomaterials are not required to be synced when to appear in this list.

### Analysis: Expression
* **Overview (base)** - The generic tripal overview pane.
* **Protocol** - Protocols used in this analysis (acquisition protocol, assay protocol, and quantification protocol).

### Biomaterial
* **Overview (base)** - The generic tripal overview pane.
* **Properties** - Properties associated with the biomaterial.
* **Cross References** - Accession terms associated with the biomaterial.
 
### Array Design
* **Overview (base)** - The generic tripal overview pane.
* **Properties** - Properties associated with the array design.

### Protocol
* **Overview (base)** - The generic tripal overview pane.

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







